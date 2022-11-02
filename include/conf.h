//
// Created by 黄保罗 on 29.10.22.
//

#ifndef RARESYNC_CONF_H
#define RARESYNC_CONF_H

#include "string"
#include "crypto.hpp"


namespace raresync {
    struct peer_conf {
        int pid_; // peer id

        /* network */
        std::string address_; // peer address
        int port_; // peer port

        /* crypto */
        bid bid_;
        bpk pk_;
    };

    typedef std::shared_ptr<peer_conf> peer_conf_sptr;

    struct conf {
        /* view duration: D + 2d */
        int D;
        int d;
        /* # of nodes can fail */
        int f;

        std::vector<peer_conf_sptr> peer_confs;
    };

    typedef std::unique_ptr<conf> conf_uptr;
}

#endif //RARESYNC_CONF_H
