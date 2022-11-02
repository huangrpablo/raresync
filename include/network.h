//
// Created by 黄保罗 on 01.11.22.
//

#ifndef RARESYNC_NETWORK_H
#define RARESYNC_NETWORK_H

#include "boost/asio.hpp"

using namespace boost::asio;
using boost::asio::ip::tcp;
using net_error = boost::system::error_code;

namespace raresync {

    class peer_conn {
    public:
        peer_conn(int pid, const std::string& address, int port, io_service& ios) :
                pid_(pid), ep_(tcp::endpoint(ip::make_address(address), port)), socket_(ios) {}

    public:
        void on_connect(net_error error) {

        }

    private:
        int pid_;
        tcp::endpoint ep_;
        tcp::socket socket_;
    };

    typedef std::shared_ptr<peer_conn> peer_conn_sptr;
    typedef std::map<int, peer_conn_sptr> peer_conn_map;


}

#endif //RARESYNC_NETWORK_H
