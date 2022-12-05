//
// Created by 黄保罗 on 05.12.22.
//

#include <unistd.h>
#include "string"
#include "vector"
#include "crypto.h"
#include "fstream"
using namespace std;

int main(int argc, char **argv) {
    int N = 4;
    int K = 3;
    int fault_num = 1;
    int D = 1;
    int d = 1;

    string address = "127.0.0.1";
    int port = 10000;

    vector<int> ids; ids.reserve(N);
    for (int i = 0; i < N; i ++) ids.push_back(i+1);

    auto crypto_fac = new raresync::crypto_factory(N, K);
    crypto_fac->init();

    auto crypto_lis = crypto_fac->create(ids);

    ofstream os;
    os.open("conf/raresync.conf");

    if (!os) return -1;

    os << "N=" << N << ",K=" << K << ",fault_num=" << fault_num;
    os << ",D=" << D << ",d=" << d << endl;

    for (int i = 0; i < N; i++) {
        os << "id=" << i << ",endpoint=" << address << ":" << port+i;
        os << ",bls_id=" << crypto_lis[i]->id << ",bls_pk=" << crypto_lis[i]->pk;
        os << ",bls_sk=" << crypto_lis[i]->sk << ",bls_mpk=" << crypto_lis[i]->mpk;
        os << endl;
    }

    os.close();

    return 0;
}