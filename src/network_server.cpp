//
// Created by 黄保罗 on 15.11.22.
//

#include "network.h"
#include "queue"

using namespace boost::asio;
using boost::asio::ip::tcp;
using net_error = boost::system::error_code;

using namespace std;

namespace raresync {

    class asio_server;
class server_session : public std::enable_shared_from_this<server_session> {
public:
    explicit server_session(io_service& ios, asio_server* server)
    : socket_(ios), server_(server) {}

    void read_header() {
        auto self = shared_from_this();
        auto buf = buffer(
                read_packet_.data(),
                proto::packet::header_length
                );

        auto handler = [self](const net_error& nerr, size_t bytes) {
            if (nerr || bytes == 0) return;

            if (bytes != proto::packet::header_length) return;

            if (!self->read_packet_.decode_header()) return;

            self->read_message();
        };

        async_read(socket_, buf, transfer_exactly(proto::packet::header_length), handler);
    }

    void read_message() {
        auto self = shared_from_this();
        auto buf = buffer(
                read_packet_.body(),
                read_packet_.body_length()
                );

        auto handler = [self](const net_error& nerr, size_t bytes) {
            if (nerr || bytes == 0) return;

            if (bytes != self->read_packet_.body_length()) return;

            self->on_packet_received();
        };

        async_read(socket_, buf, transfer_exactly(read_packet_.body_length()), handler);
    }

    void on_packet_received();

    tcp::socket socket_;
private:
    asio_server* server_;

    proto::packet read_packet_;
};

typedef std::shared_ptr<server_session> server_session_sptr;

class asio_server : public network::server {
public:
    explicit asio_server(io_service& ios, const std::string& address, int port, network::callback_sptr cb) :
    ios_(ios), acceptor_(ios), cb_(cb) {
        auto ep = tcp::endpoint(ip::address::from_string(address), port);
        acceptor_.open(ep.protocol());
        acceptor_.set_option(tcp::acceptor::reuse_address(1));
        acceptor_.bind(ep);
        acceptor_.listen();
    }

    void start() final {
        server_session_sptr session(new server_session(ios_, this));
        auto handler = [this, session](const net_error& nerr) {
            if (nerr) return;

            this->start();
            session->read_header();
        };

        acceptor_.async_accept(session->socket_,  handler);
    }

    void stop() final {}

    // dispatch messages based on message_type
    void on_message(const proto::message_sptr& msg) {
        switch (msg->type) {
            case proto::EPOCH_COMPLETION:
                cb_->on_epoch_completion_received(
                        msg->from, msg->epoch, msg->sig
                        );
                break;

            case proto::EPOCH_ENTRANCE:
                cb_->on_epoch_entrance_received(
                        msg->from, msg->epoch, msg->sig
                        );
                break;
        }
    }

private:
    io_service& ios_;
    tcp::acceptor acceptor_;
    network::callback_sptr cb_;
};

void server_session::on_packet_received() {
    string msg_str(read_packet_.body());
    proto::message_sptr msg(new proto::message(msg_str));
    server_->on_message(msg);

    read_header();
}

std::shared_ptr<network::server> network::server::create(const std::string &address, int port, io_service &ios, callback_sptr cb) {
    return std::make_shared<asio_server>(asio_server(ios, address, port, cb));
}

}