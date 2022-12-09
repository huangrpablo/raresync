//
// Created by 黄保罗 on 09.12.22.
//

#include "core.h"
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

proto::message* core::vote_msg(proto::msg_type type, string value, proto::qcert *qc, int view) {
    auto msg = new proto::message();

    msg->type = type;
    msg->value = value;
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
