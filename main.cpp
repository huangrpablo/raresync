
#include "synchronizer.h"
#include "vector"
#include "thread"
using namespace raresync;
using namespace std;

//int main() {
//
//    raresync::crypto_factory* factory = new raresync::crypto_factory(5, 3);
//
//    const char *msg = "abc";
//    factory->init();
//    auto list = factory->create({1, 2, 3, 4, 5});
//
//    auto sg0 = list[0]->share_sign(msg);
//    auto sg1 = list[1]->share_sign(msg);
//   // auto sg2 = list[2]->share_sign(msg);
//
//    int ok = list[0]->share_verify(msg, sg0);
//
//    std::vector<blsId> ids = {list[0]->id, list[1]->id};
//    std::vector<blsSignature> sgs = {sg0, sg1};
//    auto combined = list[0]->combine(ids, sgs, 2);
//    int ok1 = list[0]->combine_verify(msg, combined);
//
//    std::cout << "Hello, World!" << std::endl;
//    return 0;
//}

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
    c->f = f; c->peer_confs = peer_confs;
    return c;
}

vector<conf*> new_confs(int D, int d, int n, int f, crypto_list crypto_lis) {
//    vector<int> ids;
//    for (int i = 1; i <= n; i++) ids.push_back(i);

    string address = "127.0.0.1";
    vector<int> ports;
    for (int i = 1; i <= n; i++) ports.push_back(63790 + i);

//    auto crypto_fac = new crypto_factory(n, k);
//    crypto_fac->init();
//    auto crypto_lis = crypto_fac->create(ids);

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

void run_raresync_instance(int id, conf* cf, crypto_kit* cry) {
    auto instance = create(id, cf, cry);
    instance->init();
    instance->start();
    instance->stop();
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

    auto confs = new_confs(1, 1, n, f, crypto_lis);

    thread t1([&]() { run_raresync_instance(0, confs[0], crypto_lis[0]); });
    thread t2([&]() { run_raresync_instance(1, confs[1], crypto_lis[1]); });
    thread t3([&]() { run_raresync_instance(2, confs[2], crypto_lis[2]); });
    thread t4([&]() { run_raresync_instance(3, confs[3], crypto_lis[3]); });

    t1.join(); t2.join(); t3.join(); t4.join();

    LOG_INFO("done");
    return 0;
}
