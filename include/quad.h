//
// Created by 黄保罗 on 14.12.22.
//

#ifndef RARESYNC_QUAD_H
#define RARESYNC_QUAD_H

#include "synchronizer.h"

namespace raresync {
    typedef std::unique_ptr<boost::asio::io_service::work> ios_work_uptr;

    class quad {
    public:

    quad(int id, conf* conf, crypto_kit* crypto):
    id(id), crypto(crypto), work_(new io_service::work(ios)) {

        // init parameters
        fault_num = conf->fault_num;
        threshold = 2*fault_num + 1;

        // init peer infos
        for (auto & peer_conf : conf->peer_confs) {

            int peer_id = peer_conf->pid_;

            if (peer_id != id) peer_ids.push_back(peer_id);

            peer_cryptos[peer_id] =
                    make_shared<peer_cryt>(
                            peer_cryt(peer_conf->pid_,
                                      peer_conf->bid_,
                                      peer_conf->pk_)
                            );
        }

        // init modules
        view_core = new core(id);
        view_core->with_peers(peer_ids);
        view_core->with_crypto(this->crypto, peer_cryptos);
        view_core->with_parameters(threshold);

        view_synchronizer = new synchronizer(ios, id);
        view_synchronizer->with_peers(peer_ids);
        view_synchronizer->with_crypto(this->crypto, peer_cryptos);
        view_synchronizer->with_parameters(fault_num, threshold, conf->D, conf->d);

        // init network
        net = network::network::create(view_synchronizer, view_core, id);
        net->start(conf->peer_confs[id]->address_, conf->peer_confs[id]->port_);

        for (auto & peer_conf : conf->peer_confs) {

            int peer_id = peer_conf->pid_;

            if (peer_id == id) continue;

            net->add_peer(peer_conf->pid_,
                          peer_conf->address_,
                          peer_conf->port_);
        }

        // enable network in modules
        view_core->with_network(net);
        view_synchronizer->with_network(net);

        view_synchronizer->with_core(view_core);
    }

    void init(string proposal) { view_core->init(proposal); }

    void start() {
        view_synchronizer->start();
        ios.run();

        LOG_INFO("quad[%d] io finishes", id);
    }

private:
    core* view_core;
    synchronizer* view_synchronizer;

    network::network_sptr net;

private:
    io_service ios;
    ios_work_uptr work_;

    int fault_num; int threshold;

    int id; vector<int> peer_ids;
    crypto_sptr crypto; peer_cryt_map peer_cryptos;
};

}

#endif //RARESYNC_QUAD_H
