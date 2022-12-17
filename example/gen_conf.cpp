//
// Created by 黄保罗 on 05.12.22.
//

#include "vector"
#include "crypto.h"
#include "fstream"
using namespace std;
using namespace raresync;

int main(int argc, char **argv) {
    int N = 4;
    int K = 3;
    int fault_num = 1;
    int D = 8;
    int d = 1;

    string address = "127.0.0.1";
    int port = 63790;

    vector<int> ids; ids.reserve(N);
    for (int i = 0; i < N; i ++) ids.push_back(i+1);

    auto crypto_fac = new raresync::crypto_factory(N, K);
    crypto_fac->init();

    auto crypto_lis = crypto_fac->create(ids);

    ofstream os;
    os.open("../conf/raresync.conf");

    if (!os) return -1;

    os << N << " " << K  << " " << fault_num << " " << D << " " << d << endl;


    for (int i = 0; i < N; i++) {
        os << i << "," << address << ":" << port+i;
        os << "," << to_str(crypto_lis[i]->id) << "," << to_str(crypto_lis[i]->pk);
        os << "," << to_str(crypto_lis[i]->sk) << "," << to_str(crypto_lis[i]->mpk);
        os << endl;
    }

    os.close();

    return 0;
}