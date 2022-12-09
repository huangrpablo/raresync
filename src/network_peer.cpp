//
// Created by 黄保罗 on 15.11.22.
//

#include "network.h"
#include "define.h"
#include "queue"

using namespace boost::asio;
using boost::asio::ip::tcp;
using net_error = boost::system::error_code;

using namespace std;

namespace raresync {
    class asio_peer;

    /* client_session is an instantaneous connection
     * can be closed due to error
     * */
    class client_session {
    public:
        explicit client_session(io_service& ios, asio_peer* peer);

        ~client_session() {}

        void close_session();

        void send(proto::message* msg) {
            auto chars = msg->to_raw_str();
//            auto bytes = msg->to_raw();
            auto pkt = new proto::packet();
            pkt->body_length(chars.size());
            memcpy(pkt->body(), chars.data(), pkt->body_length());
            pkt->encode_header();

            {
                write_lock wl(msmtx_);
                messages_.push(pkt);
            }

            if (connected_) try_write();
        }

        void connect() {
            auto handler = [this](const net_error& nerr) {
                this->connected_ = true; try_write();
            };

            socket_.async_connect(ep_, handler);
        }

        void try_write() {
            auto handler = [this](const net_error& nerr, std::size_t bytes) {
                if (nerr || bytes == 0) {
                    close_session(); return;
                }

                try_write();
            };

            proto::packet* message;

            {
                write_lock wl(msmtx_);
                if (messages_.empty()) return;

                message = messages_.front();
                messages_.pop();
            }

//            LOG_INFO("core[%d] writes msg: %s", id_, message->data());

            auto buf = buffer(
                    message->data(),
                    message->length()
                    );

            async_write(socket_, buf, handler);
        }

    private:
        tcp::socket socket_;
        tcp::endpoint ep_;
        asio_peer *peer_;
        int id_;
        bool connected_;

        // unsent messages
        // how to deal with missing messages if session is closed ?
        std::shared_mutex msmtx_;
        queue<proto::packet*> messages_;
    };

    /* peer_impl maintains connections to a peer,
     * establish a new connection if necessary
     * */
class asio_peer : public network::peer {
    public:
        asio_peer(int pid, const std::string& address, int port, io_service& ios) :
                id_(pid), ios_(ios), ep_(ip::address::from_string(address), port) {}

        void start() final {}

        void send(proto::message* msg) {
            // recreate connection if necessary
            if (!session_sptr_) {
                session_sptr_ = std::make_shared<client_session>(ios_, this);
                session_sptr_->connect();
            }

            session_sptr_->send(msg);
        }

        bool active() final { return false; }

        void stop() final {}

    private:
        int id_;
        io_service& ios_;
        ip::tcp::endpoint ep_;
        friend class client_session;
        std::shared_ptr<client_session> session_sptr_;
    };

    client_session::client_session(io_service& ios, asio_peer* peer) :
            socket_(ios), ep_(peer->ep_), peer_(peer), id_(peer->id_), connected_(false) {}

    void client_session::close_session() { peer_->session_sptr_ = nullptr; }

    std::shared_ptr<network::peer> network::peer::create(int pid, const std::string &address, int port, io_service &ios) {
        return std::make_shared<asio_peer>(asio_peer(pid, address, port, ios));
    }
}
