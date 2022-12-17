//
// Created by 黄保罗 on 02.11.22.
//

#include "network.h"
#include "unordered_map"
#include "define.h"
#include "log.h"

using namespace boost::asio;
using boost::asio::ip::tcp;
using net_error = boost::system::error_code;

using namespace std;

namespace raresync::network {
    class network_impl : public network {
    public:
        explicit network_impl(synchronizer_callback* synchronizer, core_callback* core, int id) :
        synchronizer_(synchronizer), core_(core), id_(id) {}

        ~network_impl() final {
            if (io_thread_.joinable()) {
                io_thread_.join();
                LOG_INFO("transport stopped");
            }
        }

        void start(const std::string& address, int port) final {
            server_ = server::create(id_, address, port, ios_, synchronizer_, core_);
            server_->start();

            io_thread_ = std::thread([this]() {this->ios_.run();});
            LOG_INFO("synchronizer[%d] network[%d] starts...", id_, port);
        }

        void stop() final {
            ios_.stop();
            LOG_INFO("synchronizer[%d] network stops...", id_);
        }

        void send(std::vector<proto::message*>& msgs) final {
            auto task = [this](const std::vector<proto::message*>& msgs) {
                if (msgs.empty()) return;
                for (int i = 0; i < msgs.size(); i++) {
                    if (peers_.count(msgs[i]->to) == 0)
                        continue;

                    peers_[msgs[i]->to]->send(msgs[i]);
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
        synchronizer_callback* synchronizer_;
        core_callback* core_;
    };

    std::shared_ptr<network> network::create(synchronizer_callback* synchronizer, core_callback* core, int id) {
        return std::make_shared<network_impl>(synchronizer, core, id);
    }
}

