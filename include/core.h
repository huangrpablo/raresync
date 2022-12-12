//
// Created by 黄保罗 on 08.12.22.
//

#ifndef RARESYNC_CORE_H
#define RARESYNC_CORE_H

#include "crypto.h"
#include "proto.h"
#include "vector"
#include "network.h"
#include "unordered_map"
#include "define.h"

using namespace std;

namespace raresync {

// QUAD's view core module
class core {
public:
    string* proposal = nullptr;
    proto::qcert* prepareQC = nullptr;
    proto::qcert* lockedQC = nullptr;
    shared_mutex mtx_;
    int view_;

public:
    void init(string proposal);
    void start_executing(int view);
    void decide(string& value) const;

private:
    crypto_sptr crypto_;
    peer_cryt_map peer_cryts_;
    vector<int> peer_ids_;

    int threshold_; int id_;

    // for leader to collect msgs
    unordered_map<int, vector<proto::message*>> view_change_todos_;
    shared_mutex vcmtx_;

    unordered_map<int, vector<proto::message*>> prepare_todos_;
    shared_mutex prepmtx_;

    unordered_map<int, vector<proto::message*>> precommit_todos_;
    shared_mutex precmtmtx_;

    unordered_map<int, vector<proto::message*>> commit_todos_;
    shared_mutex cmmtx_;

    network::network_sptr net_;

private:
    // All the messages in M have the same type, value and view
    proto::qcert* QC(vector<proto::message*> vote_messages);
    static proto::message* msg(proto::msg_type type, string value, proto::qcert* qc, int view);
    proto::message* vote_msg(proto::msg_type type, string value, proto::qcert* qc, int view);
    bool matching_msg(proto::message* msg, proto::msg_type type, int view);
    bool matching_qc(proto::qcert* qc, proto::msg_type type, int view);

    void send(proto::message* msg);
    void send_to_myself(proto::message* msg);

    void broadcast(proto::msg_type type, string proposal, proto::qcert* qc, int view);

    void prepare(int view);

    void on_leader_prepare(int view);
    void on_process_prepare(int view, proto::message* msg);

    void on_leader_precommit(int view);
    void on_process_precommit(int view, proto::message* msg);

    void on_leader_commit(int view);
    void on_process_commit(int view, proto::message* msg);

    void on_leader_decide(int view);
    void on_process_decide(int view, proto::message* msg);

    void on_view_change_received(proto::message* msg);
    void on_prepare_received(proto::message* msg);
    void on_precommit_received(proto::message* msg);
    void on_commit_received(proto::message* msg);
    void on_decide_received(proto::message* msg);

    int leader(int view);
};

}



#endif //RARESYNC_CORE_H
