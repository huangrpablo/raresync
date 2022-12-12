//
// Created by 黄保罗 on 09.12.22.
//

#include "core.h"

#include <utility>
using namespace raresync;

proto::qcert* core::QC(vector<proto::message*> vote_messages) {
    if (vote_messages.empty()) return nullptr;

    auto qc = new proto::qcert();

    qc->type = vote_messages[0]->type;
    qc->value = vote_messages[0]->value;
    qc->view = vote_messages[0]->view;

    vector<bid> ids;
    vector<bsg> sgs;

    for (auto vote_message: vote_messages) {
        int peer = vote_message->from;
        ids.push_back(peer_cryts_[peer]->id);
        sgs.push_back(vote_message->sig);
    }

    qc->sig = crypto_->combine(ids, sgs, threshold_);

    return qc;
}

proto::message* core::msg(proto::msg_type type, string value, proto::qcert *qc, int view) {
    auto msg = new proto::message();
    msg->type = type;
    msg->value = std::move(value);
    msg->qc = qc;
    msg->view = view;

    return msg;
}

proto::message* core::vote_msg(proto::msg_type type, string value, proto::qcert *qc, int view) {
    auto msg = new proto::message();

    msg->type = type;
    msg->value = std::move(value);
    msg->qc = qc;
    msg->view = view;

    ostringstream os;
    os << int(msg->type) << "," << msg->value << "," << view;
    msg->sig = crypto_->share_sign(os.str().c_str());

    return msg;
}

bool core::matching_msg(proto::message *msg, proto::msg_type type, int view) {
    return msg->type == type && msg->view == view;
}

bool core::matching_qc(proto::qcert *qc, proto::msg_type type, int view) {
    return qc->type == type && qc->view == view;
}

int core::leader(int view) {
    return view % peer_cryts_.size();
}

// Pi’s proposal
void core::init(string proposal) {
    this->proposal = new string(std::move(proposal));
}

void core::start_executing(int view) {
    {
        write_lock wl(mtx_);
        view_ = view;
    }

    prepare(view);
}

void core::send(proto::message *msg) {
    if (msg->to == id_) send_to_myself(msg);

    else {
        vector<proto::message*> msgs({msg});
        net_->send(msgs);
    }
}

void core::send_to_myself(proto::message *msg) {
    int view = msg->view;

    switch (msg->type) {
        case proto::VIEW_CHANGE: {
            write_lock wl(vcmtx_);
            if (!view_change_todos_.count(view))
                view_change_todos_[view] = vector<proto::message*>();

            view_change_todos_[view].push_back(msg);
        } break;

        case proto::PREPARE: {
            write_lock wl(prepmtx_);
            if (!prepare_todos_.count(view))
                prepare_todos_[view] = vector<proto::message*>();

            prepare_todos_[view].push_back(msg);
        } break;

        case proto::PRE_COMMIT: {
            write_lock wl(precmtmtx_);
            if (!precommit_todos_.count(view))
                precommit_todos_[view] = vector<proto::message*>();

            precommit_todos_[view].push_back(msg);
        } break;

        case proto::COMMIT: {
            write_lock wl(cmmtx_);
            if (!commit_todos_.count(view))
                commit_todos_[view] = vector<proto::message*>();

            commit_todos_[view].push_back(msg);
        } break;
    }
}

void core::broadcast(proto::msg_type type, string proposal, proto::qcert *qc, int view) {
    // send to my peers
    vector<proto::message*> msgs(peer_ids_.size());

    for (int i = 0; i < peer_ids_.size(); i++) {
        auto m = msg(type, proposal, qc, view);
        m->to = peer_ids_[i]; m->from = id_;

        msgs[i] = m;
    }

    net_->send(msgs);
}

void core::prepare(int view) {
    // send prepare to the leader
    proto::qcert* qc = nullptr;

    {
        read_lock rl(mtx_);
        qc = prepareQC;
    }

    auto m = msg(proto::VIEW_CHANGE, "", qc, view);
    m->to = leader(view); m->from = id_;

    // send it to the leader
    send(m);
}

// 2f + 1 view-change messages received
void core::on_leader_prepare(int view) {
    vector<proto::message*> msgs;

    {
        read_lock rl(vcmtx_);
        msgs = view_change_todos_[view];
    }

    while (msgs.size() > threshold_)
        msgs.pop_back();

    proto::qcert* highQC = nullptr;

    for (auto msg : msgs) {
        if (highQC == nullptr || msg->qc->view > highQC->view)
            highQC = msg->qc;
    }

    string prop = highQC->value;

    if (prop.empty()) {
        read_lock rl(mtx_);
        if (proposal != nullptr)
            prop = *proposal;
    }

    // broadcast to peers
    broadcast(proto::PREPARE, prop, highQC, view);

    // execute as a process
    auto m = msg(proto::PREPARE, prop, highQC, view);
    m->to = id_; m->from = id_;

    on_process_prepare(view, m);
}

// every process executes this part of the pseudocode
void core::on_process_prepare(int view, proto::message* msg) {
    if (leader(view) != msg->from) return;

    if (!matching_msg(msg, proto::PREPARE, view)) return;

    if (msg->qc->value != msg->value) return;

    {
        read_lock rl(mtx_);
        if (
                lockedQC != nullptr
                && lockedQC->value != msg->value
                && msg->qc->view <= lockedQC->view
                )
            return;
    }

    auto m = vote_msg(proto::PREPARE, msg->value, nullptr, view);
    m->to = leader(view); m->from = id_;

    send(m);
}

void core::on_leader_precommit(int view) {
    vector<proto::message*> vote_msgs;

    {
        read_lock rl(prepmtx_);
        vote_msgs = prepare_todos_[view];
    }

    while (vote_msgs.size() > threshold_)
        vote_msgs.pop_back();

    auto qc = QC(vote_msgs);

    // broadcast to peers
    broadcast(proto::PRE_COMMIT, "", qc, view);

    // execute as a process
    auto m = msg(proto::PRE_COMMIT, "", qc, view);
    m->to = id_; m->from = id_;

    on_process_precommit(view, m);
}

void core::on_process_precommit(int view, proto::message *msg) {
    if (leader(view) != msg->from) return;

    if (!matching_qc(msg->qc, proto::PRE_COMMIT, view)) return;

    {
        write_lock wl(mtx_);
        prepareQC = msg->qc;
    }

    auto m = vote_msg(proto::PRE_COMMIT, msg->qc->value, nullptr, view);
    m->to = leader(view); m->from = id_;

    send(m);
}

void core::on_leader_commit(int view) {
    vector<proto::message*> vote_msgs;

    {
        read_lock rl(precmtmtx_);
        vote_msgs = precommit_todos_[view];
    }

    while (vote_msgs.size() > threshold_)
        vote_msgs.pop_back();

    auto qc = QC(vote_msgs);

    // broadcast to peers
    broadcast(proto::COMMIT, "", qc, view);

    // execute as a process
    auto m = msg(proto::COMMIT, "", qc, view);
    m->to = id_; m->from = id_;

    on_process_commit(view, m);
}

void core::on_process_commit(int view, proto::message *msg) {
    if (leader(view) != msg->from) return;

    if (!matching_qc(msg->qc, proto::COMMIT, view)) return;

    {
        write_lock wl(mtx_);
        lockedQC = msg->qc;
    }

    auto m = vote_msg(proto::COMMIT, msg->qc->value, nullptr, view);
    m->to = leader(view); m->from = id_;

    send(m);
}

void core::on_leader_decide(int view) {
    vector<proto::message*> vote_msgs;

    {
        read_lock rl(cmmtx_);
        vote_msgs = commit_todos_[view];
    }

    while (vote_msgs.size() > threshold_)
        vote_msgs.pop_back();

    auto qc = QC(vote_msgs);

    broadcast(proto::DECIDE, "", qc, view);

    auto m = msg(proto::DECIDE, "", qc, view);
    m->to = id_; m->from = id_;

    on_process_decide(view, m);
}

void core::on_process_decide(int view, proto::message *msg) {
    if (leader(view) != msg->from) return;

    if (!matching_qc(msg->qc, proto::DECIDE, view)) return;

    decide(msg->qc->value);
}

void core::decide(string& value) const {
    LOG_INFO("core[%d] decides value=%s", id_, value.c_str());
}

void core::on_view_change_received(proto::message *msg) {

}

