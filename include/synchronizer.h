//
// Created by 黄保罗 on 29.10.22.
//
#pragma once
#ifndef RARESYNC_SYNCHRONIZER_H
#define RARESYNC_SYNCHRONIZER_H

#include <utility>
#include "map"
#include "string"
#include "boost/asio.hpp"
#include "define.h"
#include "conf.h"
#include "network.h"
#include "core.h"

namespace raresync {

    class synchronizer : public network::synchronizer_callback {
    public:
        synchronizer(io_service& ios, int id) :
        id_(id), started_(false),
        view_timer_(ios), dissemination_timer_(ios) {}

        void with_peers(vector<int> peer_ids) {
            peer_ids_ = std::move(peer_ids);
        }

        void with_crypto(crypto_sptr crypto, peer_cryt_map peer_cryptos) {
            crypto_ = std::move(crypto);
            peer_cryts_ = std::move(peer_cryptos);
        }

        void with_parameters(int fault_num, int threshold, int D, int d) {
            fault_num_ = fault_num; threshold_ = threshold;
            view_duration_ = boost::asio::chrono::seconds(D + 2*d);
            dissemination_duration_ = boost::asio::chrono::seconds(d);
        }

        void with_network(network::network_sptr net) {
            net_ = std::move(net);
        }

        void with_core(core* view_core) {
            view_core_ = view_core;
        }

        /* start the raresync protocol */
        void start();

        /* stop the raresync protocol */
        void stop();

        void on_epoch_completion_received(int pid, int e, bsg p_sig);
        void on_epoch_entrance_received(int pid, int e, bsg t_sig);

    public:
        /* other configuration */
        bool gc = true;

    private:

        void measure_view_timer();
        void measure_dissemination_timer();

        void on_view_timer_expired();
        void on_dissemination_timer_expired();

        void broadcast(proto::msg_type type, int e, bsg sig);
        void broadcast_epoch_completion(int e, bsg p_sig);
        void broadcast_epoch_entrance(int e, bsg t_sig);

        void collect_garbage(int e); // release all older than e

        /* enter a view */
        rs_errno advance(int view) const;

        /* a round-robin function */
        int leader(int view) const { return view % int(peer_cryts_.size()) + 1; }


private:
    core* view_core_;

    private:
        /* measure the duration of the current view */
        boost::asio::steady_timer view_timer_;
        /* measure the duration between two communication steps */
        boost::asio::steady_timer dissemination_timer_;
        /* view duration */
        boost::asio::chrono::seconds view_duration_;
        boost::asio::chrono::seconds dissemination_duration_;

        /* end of asio fields */

        /* server fields */
        bool started_;

        /* this server id */
        int id_; int epoch_; int view_;
        bsg epoch_sig_;
        /* lock of epoch, view and epoch_sig_*/
        std::shared_mutex vesmtx_;

        /* crypto fields */
        crypto_sptr crypto_;

        /* end of crypto fields */

        /* # of nodes can fail */
        int fault_num_;

        peer_cryt_map peer_cryts_;
        vector<int> peer_ids_;

        /* end of server fields */

        /* broadcast fields */
        int threshold_;
        std::map<int, std::map<int, bsg>> epoch_completed_;
        std::shared_mutex ecmplmtx_;

        network::network_sptr net_;
    };

    typedef std::shared_ptr<synchronizer> synchronizer_sptr;
}

#endif //RARESYNC_CORE_H
