//
// Created by 黄保罗 on 02.11.22.
//

#include "network.h"
#include "unordered_map"

#include "define.h"


using namespace boost::asio;
using boost::asio::ip::tcp;
using net_error = boost::system::error_code;

using namespace std;

namespace raresync::network {

    typedef std::unique_ptr<boost::asio::io_service::work> ios_work_uptr;

    class network_impl : public network {
    public:
        explicit network_impl(callback_sptr cb, int id) : cb_(cb), id_(id) {}

        void start(const std::string& address, int port) final {
            server_ = server::create(address, port, ios_, cb_);
            server_->start();

            io_thread_ = std::thread([this]() {this->ios_.run();});
        }

        void stop() final {
            ios_.stop();
        }

        void send(std::vector<proto::message_sptr> msgs) final {
            auto task = [this](const std::vector<proto::message_sptr>& msgs) {
                if (msgs.empty()) return;
                for (const auto& msg : msgs) {
                    if (peers_.count(msg->to) == 0)
                        continue;

                    peers_[msg->to]->send(msg);
                }
            };

            ios_.post(std::bind(task, std::move(msgs)));
        }

        void add_peer(int pid, const std::string& address, int port) final {
            {
                write_lock wl(mtx_);
                if (peers_.count(pid) != 0) return;

                auto p = peer::create(pid, address, port, ios_);
                p->start(); peers_[pid] = p;
            }
        }

        void remove_peer(int pid) final {
            {
                write_lock wl(mtx_);
                peers_.erase(pid);
            }
        }

    private:
        int id_;
        std::thread io_thread_;
        io_service ios_;
        std::shared_mutex mtx_;
        unordered_map<int, peer_sptr> peers_;
        server_sptr server_;
        callback_sptr cb_;
    };

    std::shared_ptr<network> network::create(callback_sptr cb, int id) {
        std::shared_ptr<network_impl> impl(new network_impl(cb, id));
        return impl;
    }
}

