//
// Created by 黄保罗 on 04.12.22.
//

#include "string"
#include "fstream"
#include "quad.h"
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

    int N; int K;
    int fault_num = 1;
    int D = 1; int d = 1;

    vector<int> ids;
    vector<string> endpoints;
    vector<bid> bls_ids;
    vector<bpk> bls_pks;
    vector<bsk> bls_sks;
    vector<bpk> bls_mpks;

    string line0;
    getline(fs, line0);
    istringstream is(line0);

    is >> N >> K >> fault_num >> D >> d;

    while (!fs.eof()) {
        string line;
        getline(fs, line);

        if (line.empty()) break;

        istringstream is(line);

        int id; string endpoint;
        string id_str; string pk_str;
        string sk_str; string mpk_str;

        bid bls_id; bpk bls_pk;
        bsk bls_sk; bpk bls_mpk;

        string token;
        getline(is, token, ',');
        id = atoi(token.c_str());

        getline(is, endpoint, ',');
        getline(is, id_str, ',');
        getline(is, pk_str, ',');
        getline(is, sk_str, ',');
        getline(is, mpk_str, ',');

        from_str(id_str, bls_id);
        from_str(pk_str, bls_pk);
        from_str(sk_str, bls_sk);
        from_str(mpk_str, bls_mpk);

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
        c->fault_num = fault_num;
        c->peer_confs = peer_confs;
        this->confs.push_back(c);
    }

    return true;
}

int main(int argc, char **argv) {
    auto parser = new Parser();

    int id = atoi(argv[1]);
    LOG_INFO("%d", id);
    crypto_factory::init();
    parser->load("../conf/raresync.conf");

    auto instance = new quad(
            id, parser->confs[id],
            parser->cryptos[id]
    );

    instance->init("cow" + to_string(id));
    instance->start();

    return 0;
}
