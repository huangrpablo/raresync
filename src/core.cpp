//
// Created by 黄保罗 on 30.10.22.
//

#include "core.h"
#include "define.h"
#include <iostream>
#include <memory>
#include "boost/asio.hpp"

using namespace boost::asio;
using timer_error = boost::system::error_code;

rs_errno raresync::core::init() {
    if (inited_) return GOOD;

    inited_ = true;

    f_ = conf_->f;

    auto self = conf_->servers[id_];
    ep_ = tcp::endpoint(ip::make_address(self->address), self->port);

    peers_ = peer_map();
    for (auto & server : conf_->servers) {
        peers_[server->id] = std::make_shared<peer>(
                peer(
                        server->id,
                        server->address,
                        server->port,
                        ios_
                        )
                );
    }


    /* crypto */
    epoch_sig_ = nullptr;

    /* broadcast */
    threshold_ = 2 * f_ + 1;
    epoch_completed_ = std::map<int, std::map<int, todo>>();

    /* asio service */
    view_duration_ = boost::asio::chrono::seconds(conf_->D + 2 * conf_->d);
    dissemination_duration_ = boost::asio::chrono::seconds(conf_->d);

    // do something with tcp
    // listen on connection
}

rs_errno raresync::core::start() {
    if (started_) return GOOD;

    started_ = true;

    {
        write_lock wl(vmtx_);
        epoch_ = 1;
        view_ = 1;
    }

    /* run asio service */

    /* set the timers */
    // measure the duration of the first view
    measure_view_timer();

    /* enter the view */
    // enter the first view
    advance(1);
}

rs_errno raresync::core::advance(int view) const {
    // do something
    std::cout << id_ << " advances " << view << std::endl;
    return GOOD;
}

void raresync::core::measure_view_timer() {
    view_timer_.expires_from_now(view_duration_);
    view_timer_.async_wait([this](const timer_error &e) {
        on_view_timer_expired();
    });
}

void raresync::core::measure_dissemination_timer() {
    dissemination_timer_.expires_from_now(dissemination_duration_);
    dissemination_timer_.async_wait([this](const timer_error &e) {
        on_dissemination_timer_expired();
    });
}

void raresync::core::on_view_timer_expired() {
    int view; int epoch;
    {
        write_lock wl(vmtx_);
        view = view_;
        epoch = epoch_;

        // check if the current view is not the last view of the current epoch
        if (view < f_ + 1) view_++;
    }

    // check if the current view is not the last view of the current epoch
    if (view < f_ + 1) {
        int view_to_advance = (epoch - 1) * (f_ + 1) + view;

        // measure the duration of the view
        measure_view_timer();

        // enter the next view
        advance(view_to_advance);
        return;
    }

    // inform other processes that the epoch is completed
    auto msg = std::to_string(epoch);
    bsg p_sig = crypto_->share_sign(msg.c_str());

    broadcast_epoch_completion(epoch, p_sig);
}

void raresync::core::on_dissemination_timer_expired() {
    int epoch;
    bsg epoch_sig;
    {
        read_lock rv(vmtx_);
        epoch = epoch_;
        epoch_sig = *epoch_sig_;
    }

    broadcast_epoch_entrance(epoch, epoch_sig);

    // reset the current view to 1
    {
        write_lock wl(vmtx_);
        view_ = 1;
    }

    int view_to_advance = (epoch - 1) * (f_ + 1) + 1;

    // measure the duration of the view
    measure_view_timer();

    // enter the first view of the new epoch
    advance(view_to_advance);
}

void raresync::core::broadcast_epoch_completion(int e, bsg p_sig) {

}

void raresync::core::broadcast_epoch_entrance(int e, bsg t_sig) {

}

void raresync::core::on_epoch_entrance_receive(int pid, int e, bsg t_sig) {
    // verify whether this t_sig is combined from 2f+1 peers
    auto msg = std::to_string(e);
    if (!crypto_->combine_verify(msg.c_str(), t_sig)) return;

    {
        write_lock wv(vmtx_);
        if (epoch_ >= e) return;

        epoch_ = e;
        // t_sig is a threshold signature of epoch e − 1
        *epoch_sig_ = t_sig;
    }

    view_timer_.cancel();
    dissemination_timer_.cancel();

    // wait δ time before broadcasting enter-epoch
    measure_dissemination_timer();
}

void raresync::core::on_epoch_completion_received(int pid, int e, bid p_bid, bsg p_sig) {
    

    int epoch;
    {
        read_lock rv(vmtx_);
        epoch = epoch_;

        if (epoch > e) return;

        write_lock wec(ecmtx_);
        if (!epoch_completed_.count(e))
            epoch_completed_[e] = std::map<int, todo>();

        epoch_completed_[e][pid] = todo(p_bid, p_sig);
    }

    int new_epoch;
    bsg new_epoch_sig;
    bool found = false;

    {
        read_lock rec(ecmtx_);
        for (auto & it : epoch_completed_) {
            if (
                    it.first < epoch ||
                    it.second.size() < threshold_
            )
                continue;

            std::vector<bid> pids(threshold_);
            std::vector<bsg> sigs(threshold_);

            auto iit = it.second.begin();
            for (int i = 0; i < threshold_; i++, iit++) {
                pids[i] = iit->second.first;
                sigs[i] = iit->second.second;
            }

            new_epoch = it.first + 1;
            new_epoch_sig = crypto_->combine(pids, sigs, threshold_);
            found = true;
            break;
        }
    }

    if (!found) return;

    {
        write_lock wv(vmtx_);
        epoch_ = new_epoch;
        *epoch_sig_ = new_epoch_sig;
    }

    view_timer_.cancel();
    dissemination_timer_.cancel();
    // wait δ time before broadcasting enter-epoch
    measure_dissemination_timer();
}