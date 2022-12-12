//
// Created by 黄保罗 on 29.10.22.
//
#pragma once
#ifndef RARESYNC_CORE_H
#define RARESYNC_CORE_H

#include <utility>
#include "map"
#include "string"
#include "boost/asio.hpp"
#include "define.h"
#include "conf.h"
#include "network.h"

namespace raresync {

    typedef std::unique_ptr<boost::asio::io_service::work> ios_work_uptr;

class synchronizer : public network::callback {
    public:
        synchronizer(int id, conf* conf, crypto_kit* crypto) :
        id_(id), conf_(conf), crypto_(crypto),
        work_(new boost::asio::io_service::work(ios_)),
        view_timer_(ios_),
        dissemination_timer_(ios_),
        inited_(false),
        started_(false) {}


/* initialize the raresync protocol, configurations, timers*/
        void init();

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

        /* check if self is the leader of this view */
        bool is_leader(int view) const { return leader(view) == id_; }

    private:
        /* configuration */
        conf_uptr conf_;

        /* asio fields */
        boost::asio::io_service ios_;
        /* keep io_service busy */
        ios_work_uptr work_;
        /* global lock for this core object */
        std::mutex mtx_;
        /* measure the duration of the current view */
        boost::asio::steady_timer view_timer_;
        /* measure the duration between two communication steps */
        boost::asio::steady_timer dissemination_timer_;
        /* view duration */
        boost::asio::chrono::seconds view_duration_;
        boost::asio::chrono::seconds dissemination_duration_;

        /* end of asio fields */

        /* server fields */
        bool inited_;
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

    static synchronizer_sptr create(int id, conf* conf, crypto_kit* crypto) {
        return std::make_shared<synchronizer>(id, conf, crypto);
    }
}

#endif //RARESYNC_CORE_H
