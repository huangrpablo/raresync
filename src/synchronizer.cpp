//
// Created by 黄保罗 on 30.10.22.
//

#include "synchronizer.h"
#include <memory>
#include "boost/asio.hpp"
#include "log.h"

using namespace boost::asio;
using timer_error = boost::system::error_code;

void raresync::synchronizer::init() {
    if (inited_) return;

    inited_ = true;

    fault_num_ = conf_->fault_num;

    auto self_conf = conf_->peer_confs[id_];

    peer_cryts_ = peer_cryt_map();

    for (auto & peer_conf : conf_->peer_confs) {
        peer_cryts_[peer_conf->pid_] =
                std::make_shared<peer_cryt>(
                peer_cryt(peer_conf->pid_,
                          peer_conf->bid_,
                          peer_conf->pk_));

        if (peer_conf->pid_ != id_)
            peer_ids_.push_back(peer_conf->pid_);
    }

    /* crypto */

    /* broadcast */
    threshold_ = 2 * fault_num_ + 1;
    epoch_completed_ = std::map<int, std::map<int, bsg>>();

    net_ = network::network::create(this, id_);
    net_->start(self_conf->address_, self_conf->port_);

    for (auto & peer_conf : conf_->peer_confs) {
        if (peer_conf->pid_ != id_)
            net_->add_peer(
                    peer_conf->pid_,
                    peer_conf->address_,
                    peer_conf->port_
                    );
    }

    /* asio service */
    view_duration_ = boost::asio::chrono::seconds(conf_->D + 2 * conf_->d);
    dissemination_duration_ = boost::asio::chrono::seconds(conf_->d);
}

void raresync::synchronizer::start() {
    if (started_) return;

    started_ = true;

    {
        write_lock wl(vesmtx_);
        epoch_ = 1;
        view_ = 1;
    }

    LOG_INFO("synchronizer[%d] starts: epoch=%d, view=%d", id_, 1, 1);

    /* run asio service */

    /* set the timers */
    // measure the duration of the first view
    measure_view_timer();

    /* enter the view */
    // enter the first view
    advance(1);
    ios_.run();
}

void raresync::synchronizer::stop() {

}

rs_errno raresync::synchronizer::advance(int view) const {
    // do something
    LOG_INFO("synchronizer[%d] advances epoch=%d, view=%d", id_, epoch_, view);

    return GOOD;
}

void raresync::synchronizer::measure_view_timer() {
    view_timer_.expires_from_now(view_duration_);
    view_timer_.async_wait([this](const timer_error &e) {
        on_view_timer_expired();
    });

    LOG_INFO("synchronizer[%d] measures a view timer", id_);
}

void raresync::synchronizer::measure_dissemination_timer() {
    dissemination_timer_.expires_from_now(dissemination_duration_);
    dissemination_timer_.async_wait([this](const timer_error &e) {
        on_dissemination_timer_expired();
    });

    LOG_INFO("synchronizer[%d] measures a dissemination timer", id_);
}

void raresync::synchronizer::on_view_timer_expired() {
    LOG_INFO("synchronizer[%d] view timer expired", id_);

    int view; int epoch;
    {
        write_lock wl(vesmtx_);
        view = view_;
        epoch = epoch_;

        // check if the current view is not the last view of the current epoch
        if (view < fault_num_ + 1) view_++;
    }

    // check if the current view is not the last view of the current epoch
    if (view < fault_num_ + 1) {
        int view_to_advance = (epoch - 1) * (fault_num_ + 1) + (view + 1);

        // measure the duration of the view
        measure_view_timer();

        // enter the next view
        advance(view_to_advance);
        return;
    }

    // inform other processes that the epoch is completed
    auto msg = std::to_string(epoch);
    bsg p_sig = crypto_->share_sign(msg.c_str());

    // store locally first
    {
        write_lock wl(ecmplmtx_);
        if (!epoch_completed_.count(epoch))
            epoch_completed_[epoch] = std::map<int, bsg>();

        epoch_completed_[epoch][id_] = p_sig;
    }

    // broadcast to the rest
    broadcast_epoch_completion(epoch, p_sig);
}

void raresync::synchronizer::on_dissemination_timer_expired() {
    LOG_INFO("synchronizer[%d] dissemination timer expired", id_);

    int epoch;
    bsg epoch_sig;
    {
        read_lock rv(vesmtx_);
        epoch = epoch_;
        epoch_sig = epoch_sig_;
    }

    broadcast_epoch_entrance(epoch, epoch_sig);

    // reset the current view to 1
    {
        write_lock wl(vesmtx_);
        view_ = 1;
    }

    int view_to_advance = (epoch - 1) * (fault_num_ + 1) + 1;

    // measure the duration of the view
    measure_view_timer();

    // enter the first view of the new epoch
    advance(view_to_advance);
}

void raresync::synchronizer::broadcast(proto::msg_type type, int e, bsg sig) {

    std::vector<proto::message*> msgs(peer_ids_.size());
    for (int i = 0; i < peer_ids_.size(); i++) {
        auto msg = new proto::message();
        msg->type = type;
        msg->to = peer_ids_[i];
        msg->from = id_;
        msg->epoch = e;
        msg->sig = sig;

        msgs[i] = msg;
    }

    LOG_DEBUG("code[%d] ready to sends", id_);
    net_->send(msgs);
}

void raresync::synchronizer::broadcast_epoch_completion(int e, bsg p_sig) {
    LOG_INFO("synchronizer[%d] broadcasts EPOCH_COMPLETION: epoch=%d", id_, e);

    broadcast(proto::EPOCH_COMPLETION, e, p_sig);
}

void raresync::synchronizer::broadcast_epoch_entrance(int e, bsg t_sig) {
    LOG_INFO("synchronizer[%d] broadcasts EPOCH_ENTRANCE: epoch=%d", id_, e);

    broadcast(proto::EPOCH_ENTRANCE, e, t_sig);
}


void raresync::synchronizer::on_epoch_entrance_received(int pid, int e, bsg t_sig) {
    LOG_INFO("synchronizer[%d] receives EPOCH_ENTRANCE: pid=%d, epoch=%d", pid, id_, e);

    // verify whether this t_sig is combined from 2f+1 peers
    auto msg = std::to_string(e);
    if (!crypto_->combine_verify(msg.c_str(), t_sig)) return;

    {
        write_lock wv(vesmtx_);
        if (epoch_ >= e) return;

        epoch_ = e;
        // t_sig is a threshold signature of epoch e − 1
        epoch_sig_ = t_sig;
    }

    view_timer_.cancel();
    dissemination_timer_.cancel();

    // wait δ time before broadcasting enter-epoch
    measure_dissemination_timer();
}

void raresync::synchronizer::on_epoch_completion_received(int pid, int e, bsg p_sig) {
    LOG_INFO("synchronizer[%d] receives EPOCH_COMPLETION: pid=%d, epoch=%d", pid, id_, e);

    // verify whether this p_sig is signed by pid
    auto msg = std::to_string(e);
    if (!peer_cryts_.count(pid) ||
    !peer_cryts_[pid]->share_verify(msg.c_str(), p_sig)) {
        LOG_INFO("verify signature fails");
        return;
    }

    int epoch;
    {
        read_lock rv(vesmtx_);
        epoch = epoch_;

        if (epoch > e) return;
    }

    {
        write_lock wec(ecmplmtx_);
        if (!epoch_completed_.count(e))
            epoch_completed_[e] = std::map<int, bsg>();

        epoch_completed_[e][pid] = p_sig;
    }

    int new_epoch;
    bsg new_epoch_sig;
    bool found = false;

    {
        read_lock rec(ecmplmtx_);

        for (auto & it : epoch_completed_) {
            if (
                    it.first < epoch ||
                    it.second.size() < threshold_
            )
                continue;

            std::vector<bid> ids(threshold_);
            std::vector<bsg> sgs(threshold_);

            auto iit = it.second.begin();
            for (int i = 0; i < threshold_; i++, iit++) {
                ids[i] = peer_cryts_[iit->first]->id;
                sgs[i] = iit->second;
            }

            LOG_INFO("synchronizer[%d] collects {EPOCH_COMPLETION}{%d} from %d peers", id_, it.first, it.second.size());

            new_epoch = it.first + 1;
            new_epoch_sig = crypto_->combine(ids, sgs, threshold_);
            found = true;
            break;
        }
    }

    if (!found) return;

    {
        write_lock wv(vesmtx_);
        epoch_ = new_epoch;
        epoch_sig_ = new_epoch_sig;
    }

    if (gc) collect_garbage(new_epoch);

    view_timer_.cancel();
    dissemination_timer_.cancel();
    // wait δ time before broadcasting enter-epoch
    measure_dissemination_timer();
}

void raresync::synchronizer::collect_garbage(int e) {
    // erase old epochs in epoch_completed list
    {
        write_lock wl(ecmplmtx_);
        std::vector<int> obsoletes;

        for (auto & it : epoch_completed_) {
            if (it.first < e)
                obsoletes.push_back(it.first);
        }

        for (auto obsolete : obsoletes)
            epoch_completed_.erase(obsolete);
    }
}
