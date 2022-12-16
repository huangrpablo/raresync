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
    explicit server_session(int id, io_service& ios, asio_server* server)
    : id_(id), socket_(ios), server_(server) {}

    void read_header() {
        auto self = shared_from_this();
        read_packet_.clear();
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

        async_read(self->socket_, buf , handler);
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

        async_read(self->socket_, buf,  handler);
    }

    void on_packet_received();

    tcp::socket socket_;
private:
    int id_;
    asio_server* server_;

    proto::packet read_packet_;
};

typedef std::shared_ptr<server_session> server_session_sptr;

class asio_server : public network::server {
public:
    explicit asio_server(int id, io_service& ios, const std::string& address, int port,
                         network::synchronizer_callback* synchronizer, network::core_callback* core) :
            id_(id), ios_(ios), acceptor_(ios), synchronizer_(synchronizer), core_(core) {
        LOG_INFO("address=%d", port);
        auto ep = tcp::endpoint(ip::address::from_string(address), port);
        acceptor_.open(ep.protocol());
        acceptor_.set_option(tcp::acceptor::reuse_address(1));
        acceptor_.bind(ep);
        acceptor_.listen();
    }

    void start() final {
        server_session_sptr session(new server_session(id_, ios_, this));
        auto handler = [this, session](const net_error& nerr) {
            if (nerr) return;

            this->start();
            session->read_header();
        };

        acceptor_.async_accept(session->socket_,  handler);
    }

    void stop() final {}

    // dispatch messages based on message_type
    void on_message(proto::message* msg) {
        switch (msg->type) {
            case proto::EPOCH_COMPLETION:
                synchronizer_->on_epoch_completion_received(
                        msg->from, msg->epoch, msg->sig
                        );
                break;

            case proto::EPOCH_ENTRANCE:
                synchronizer_->on_epoch_entrance_received(
                        msg->from, msg->epoch, msg->sig
                        );
                break;

            case proto::VIEW_CHANGE:
                core_->on_view_change_received(msg);
                break;

            case proto::PREPARE:
                core_->on_prepare_received(msg);
                break;

            case proto::PRE_COMMIT:
                core_->on_precommit_received(msg);
                break;

            case proto::COMMIT:
                core_->on_commit_received(msg);
                break;

            case proto::DECIDE:
                core_->on_decide_received(msg);
                break;
        }
    }

public:
    int id_;

private:
    io_service& ios_;
    tcp::acceptor acceptor_;
    network::synchronizer_callback* synchronizer_;
    network::core_callback* core_;
};

void server_session::on_packet_received() {
    auto msg = new proto::message((char*)read_packet_.body());


//    LOG_INFO("core[%d] receives msg from %d: %s", msg->to, msg->from, msg->to_log().c_str());
    server_->on_message(msg);

    read_header();
}

std::shared_ptr<network::server> network::server::create(int id, const std::string &address, int port, io_service &ios,
                                                         synchronizer_callback* synchronizer, core_callback* core) {
    return std::make_shared<asio_server>(asio_server(id, ios, address, port, synchronizer, core));
}

}