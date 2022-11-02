//
// Created by 黄保罗 on 29.10.22.
//

#ifndef RARESYNC_CONF_H
#define RARESYNC_CONF_H

#include "string"


namespace raresync {
    struct server {
        int id;
        std::string address;
        int port;
    };

    typedef std::shared_ptr<server> server_sptr;

    struct conf {
        /* view duration: D + 2d */
        int D;
        int d;
        /* # of nodes can fail */
        int f;

        std::vector<server_sptr> servers;
    };

    typedef std::unique_ptr<conf> conf_uptr;
}

#endif //RARESYNC_CONF_H
