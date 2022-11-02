#include <iostream>
#include "crypto.hpp"
#include "bls/bls.h"

int main() {

    raresync::crypto_factory* factory = new raresync::crypto_factory(5, 3);

    const char *msg = "abc";
    factory->init();
    auto list = factory->create({1, 2, 3, 4, 5});

    auto sg0 = list[0]->share_sign(msg);
    auto sg1 = list[1]->share_sign(msg);
   // auto sg2 = list[2]->share_sign(msg);

    int ok = list[0]->share_verify(msg, sg0);

    std::vector<blsId> ids = {list[0]->id, list[1]->id};
    std::vector<blsSignature> sgs = {sg0, sg1};
    auto combined = list[0]->combine(ids, sgs, 2);
    int ok1 = list[0]->combine_verify(msg, combined);

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
