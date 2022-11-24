//
// Created by 黄保罗 on 01.11.22.
//
#pragma once
#ifndef RARESYNC_NETWORK_H
#define RARESYNC_NETWORK_H

#include "boost/asio.hpp"
#include "map"
#include "proto.h"
#include "log.h"

using namespace boost::asio;

namespace raresync::network {
        class callback {
        public:
            virtual ~callback() = default;

            virtual void on_epoch_completion_received(int pid, int e, bsg p_sig) = 0;
            virtual void on_epoch_entrance_received(int pid, int e, bsg t_sig) = 0;
        };

        /* network manages a receiver and multiple senders */
        class network {
        public:
            virtual ~network() = default;

            virtual void start(const std::string& address, int port) = 0;

            virtual void stop() = 0;

            // send to remote peers
            // each message has a to field, corresponding the target peer id
            virtual void send(std::vector<proto::message*>& msgs) = 0;

            virtual void add_peer(int pid, const std::string& address, int port) = 0;

            virtual void remove_peer(int pid) = 0;

            static std::shared_ptr<network> create(callback* cb, int id);
        };

        typedef std::shared_ptr<network> network_sptr;

        /* peer defines a sender */
        class peer {
        public:
            virtual ~peer() = default;
            virtual void start() = 0;
            virtual void send(proto::message* msg) = 0;
            virtual void stop() = 0;
            virtual bool active() = 0;

            static std::shared_ptr<peer> create(int pid, const std::string& address, int port, io_service& ios);
        };

        typedef std::shared_ptr<peer> peer_sptr;

        class server {
        public:
            virtual void start() = 0;
            virtual void stop() = 0;

            static std::shared_ptr<server> create(int id, const std::string& address, int port, io_service& ios, callback* cb);
        };

        typedef std::shared_ptr<server> server_sptr;
    }

#endif //RARESYNC_NETWORK_H
