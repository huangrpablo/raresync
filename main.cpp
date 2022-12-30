
#include "quad.h"
#include "vector"
#include "thread"
using namespace raresync;
using namespace std;

peer_conf_sptr new_peer_conf(int pid, std::string& address, int port, bid bid_, bpk pk_) {
    auto conf = new peer_conf();
    conf->pid_ = pid;
    conf->address_ = address;
    conf->port_ = port;
    conf->bid_ = bid_;
    conf->pk_ = pk_;

    return std::shared_ptr<peer_conf>(conf);
}

conf* new_conf(int D, int d, int f, std::vector<peer_conf_sptr> peer_confs) {
    auto c = new conf();
    c->D = D; c->d = d;
    c->fault_num = f; c->peer_confs = peer_confs;
    return c;
}

vector<conf*> new_confs(int D, int d, int n, int f, crypto_list crypto_lis) {
    string address = "127.0.0.1";
    vector<int> ports;
    for (int i = 1; i <= n; i++) ports.push_back(63790 + i);

    vector<peer_conf_sptr> peer_confs;
    peer_confs.reserve(n);
    for (int i = 0; i < n; i++)
        peer_confs.push_back(new_peer_conf(i, address, ports[i], crypto_lis[i]->id, crypto_lis[i]->pk));

    vector<conf*> confs;
    confs.reserve(n);
    for (int i = 0; i < n; i++)
        confs.push_back(new_conf(D, d, f, peer_confs));

    return confs;
}

//void run_raresync_instance(int id, conf* cf, crypto_kit* cry) {
//    auto instance = create(id, cf, cry);
//    instance->init();
//    instance->start();
//    instance->stop();
//}

void run_raresync_with_core(int id, conf* cf, crypto_kit* cry) {
    auto instance = new quad(id, cf, cry);
    instance->init("cow" + to_string(id));
    instance->start();
}

int main() {
    int n = 4; int k = 3; int f = 1;

    vector<int> ids;
    ids.reserve(n);
    for (int i = 0; i < n; i++)
        ids.push_back(i+1);

    auto crypto_fac = new crypto_factory(n, k);
    crypto_fac->init();
    auto crypto_lis = crypto_fac->create(ids);

    auto confs = new_confs(8, 1, n, f, crypto_lis);

    thread t1([&]() { run_raresync_with_core(0, confs[0], crypto_lis[0]); });
    thread t2([&]() { run_raresync_with_core(1, confs[1], crypto_lis[1]); });
    thread t3([&]() { run_raresync_with_core(2, confs[2], crypto_lis[2]); });
    thread t4([&]() { run_raresync_with_core(3, confs[3], crypto_lis[3]); });

    t1.join(); t2.join(); t3.join(); t4.join();

    LOG_INFO("done");
    return 0;
}
