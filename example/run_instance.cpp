//
// Created by 黄保罗 on 04.12.22.
//

#include "string"
#include "fstream"
#include "core.h"
#include "sstream"
using namespace std;
using namespace raresync;

class Parser {
public:
    bool load(const std::string &fname);
public:
    vector<conf*> confs;
    vector<crypto_kit*> cryptos;
};

bool Parser::load(const std::string &fname) {
    ifstream fs(fname);

    if (!fs.is_open()) return false;

    char c; string key;

    int N; int K;
    int fault_num = 1;
    int D = 1; int d = 1;

    vector<int> ids;
    vector<string> endpoints;
    vector<bid> bls_ids;
    vector<bpk> bls_pks;
    vector<bsk> bls_sks;
    vector<bpk> bls_mpks;

    string line;
    getline(fs, line);
    istringstream is(line);

    is >> key >> c >> N; is >> c;
    is >> key >> c >> K; is >> c;
    is >> key >> c >> fault_num; is >> c;
    is >> key >> c >> D; is >> c;
    is >> key >> d >> D;

    while (!fs.eof()) {
        string line;
        getline(fs, line);

        istringstream is(line);

        int id; string endpoint;
        bid bls_id; bpk bls_pk;
        bsk bls_sk; bpk bls_mpk;

        is >> key >> c >> id; is >> c;

        is >> key >> c >> endpoint; is >> c;

        // this is ridiculous
        // in bls, you can't even deserialize it via istringstream
        is >> key >> c >> bls_id; is >> c;

        is >> key >> c >> bls_pk; is >> c;

        is >> key >> c >> bls_sk; is >> c;

        is >> key >> c >> bls_mpk;

        ids.push_back(id);
        endpoints.push_back(endpoint);
        bls_ids.push_back(bls_id);
        bls_pks.push_back(bls_pk);
        bls_sks.push_back(bls_sk);
        bls_mpks.push_back(bls_mpk);
    }

    vector<shared_ptr<peer_conf>> peer_confs;

    for (auto id : ids) {
        auto conf = new peer_conf();
        conf->pid_ = id;

        auto pos = endpoints[id].find(':');
        conf->address_ = endpoints[id].substr(0, pos);
        conf->port_ = atoi(endpoints[id].substr(pos+1).c_str());
        conf->bid_ = bls_ids[id]; conf->pk_ = bls_pks[id];

        peer_confs.push_back(make_shared<peer_conf>(*conf));
    }

    for (auto id : ids) {
        auto crypto = new crypto_kit();
        crypto->id = bls_ids[id];
        crypto->pk = bls_pks[id];
        crypto->sk = bls_sks[id];
        crypto->mpk = bls_mpks[id];
        this->cryptos.push_back(crypto);
    }

    for (auto id : ids) {
        auto c = new conf();
        c->D = D; c->d = d;
        c->f = fault_num;
        c->peer_confs = peer_confs;
        this->confs.push_back(c);
    }

    return true;
}

int main(int argc, char **argv) {
    auto parser = new Parser();

    int id = atoi(argv[1]);
    parser->load("conf/raresync.conf");

    auto instance = create(
            id, parser->confs[id],
            parser->cryptos[id]
            );

    instance->init();
    instance->start();
//    instance->stop();

    return 0;
}
