//
// Created by 黄保罗 on 08.12.22.
//

#ifndef RARESYNC_CORE_H
#define RARESYNC_CORE_H

#include "crypto.h"
#include "proto.h"
#include "vector"

using namespace std;

namespace raresync {

// QUAD's view core module
class core {
private:
    crypto_sptr crypto_;
    peer_cryt_map peer_cryts_;
    int threshold_;

private:
    // All the messages in M have the same type, value and view
    proto::qcert* QC(vector<proto::message*> vote_messages);
    proto::message* vote_msg(proto::msg_type type, string value, proto::qcert* qc, int view);
    bool matching_msg(proto::message* msg, proto::msg_type type, int view);
    bool matching_qc(proto::qcert* qc, proto::msg_type type, int view);
};

}



#endif //RARESYNC_CORE_H
