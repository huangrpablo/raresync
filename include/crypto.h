//
// Created by 黄保罗 on 29.10.22.
//
#pragma once
#ifndef RARESYNC_CRYPTO_H
#define RARESYNC_CRYPTO_H

#define BLS_ETH
#include <bls/bls384_256.h>
#include "bls/bls.hpp"
#include "vector"
#include "iostream"
#include "unordered_map"

using namespace std;

namespace raresync {
    typedef blsPublicKey bpk;
    typedef blsSecretKey bsk;
    typedef blsSignature bsg;
    typedef blsId bid;

    struct peer_cryt {
        int pid;
        bid id;
        bpk pk;

        peer_cryt(int pid, bid id, bpk pk) : pid(pid), id(id), pk(pk) {}

        bool share_verify(const char* msg, const bsg& sg) const {
            assert(msg != nullptr);

            return blsVerify(&sg, &pk, msg, strlen(msg));
        }
    };

    typedef std::shared_ptr<peer_cryt> peer_cryt_sptr;
    typedef std::unordered_map<int, peer_cryt_sptr> peer_cryt_map;

    struct crypto_kit {
        bid id;
        bsk sk;
        bpk pk;
        bpk mpk;

        bsg share_sign(const char* msg) const {
            assert(msg != nullptr);

            bsg sg;
            blsSign(&sg, &sk, msg, strlen(msg));
            return sg;
        }

        bool share_verify(const char* msg, const bsg& sg) const {
            assert(msg != nullptr);

            return blsVerify(&sg, &pk, msg, strlen(msg));
        }

        static bsg combine(std::vector<bid>& ids, std::vector<bsg>& sgs, int k) {
            assert(k == ids.size() && k == sgs.size());
            assert(k > 0);

            bsg combined;
            blsSignatureRecover(&combined, sgs.data(), ids.data(), k);
            return combined;
        }

        bool combine_verify(const char* msg, const bsg& combined) const {
            assert(msg != nullptr);

            return blsVerify(&combined, &mpk, msg, strlen(msg));
        }


    };

    static vector<byte> serialize(bsg sig) {
        byte buf[128];
        auto sig_sz = blsSignatureSerialize(buf, 128, &sig);
        vector<byte> v; v.reserve(sig_sz);
        for (int i = 0; i < sig_sz; i++) v.push_back(buf[i]);

        return v;
    }

    static bsg deserialize(byte* v, size_t sz) {
        bsg sig;
        blsSignatureDeserialize(&sig, v, sz);
        return sig;
    }

    static void from_str(const std::string& str, bid& id) {
        mclBnFr_setStr(&id.v, str.c_str(), str.size(), 16);
    }

    static string to_str(bid& id) {
        string str;
        str.resize(1024);
        size_t n = mclBnFr_getStr(&str[0], str.size(), &id.v, 16|bls::IoPrefix);
        str.resize(n);
        return str;
    }

    static void from_str(const std::string& str, bsk& sk) {
        mclBnFr_setStr(&sk.v, str.c_str(), str.size(), 16);
    }

    static string to_str(bsk& sk) {
        string str;
        str.resize(1024);
        size_t n = mclBnFr_getStr(&str[0], str.size(), &sk.v, 16|bls::IoPrefix);
        str.resize(n);
        return str;
    }

    static void from_str(const std::string& str, bpk& pk) {
#ifdef BLS_ETH
        int ret = mclBnG1_setStr(&pk.v, str.c_str(), str.size(), 16);
#else
        int ret = mclBnG2_setStr(&pk.v, str.c_str(), str.size(), 16);
#endif
        if (ret != 0) throw std::runtime_error("PublicKey:setStr");
    }

    static string to_str(bpk& pk) {
        string str;
        str.resize(1024);
#ifdef BLS_ETH
        size_t n = mclBnG1_getStr(&str[0], str.size(), &pk.v, 16|bls::IoPrefix);
#else
        size_t n = mclBnG2_getStr(&str[0], str.size(), &self_.v, 16|bls::IoPrefix);
#endif
        if (n == 0) throw std::runtime_error("PublicKey:getStr");
        str.resize(n);
        return str;
    }

    typedef std::unique_ptr<crypto_kit> crypto_uptr;
    typedef std::vector<crypto_kit*> crypto_list;

    class crypto_factory {
    public:
        crypto_factory(int N, int K) : N_(N), K_(K) {}

        static int init() {
        #ifdef BLS_ETH
            puts("BLS_ETH mode");
        #else
            puts("no BLS_ETH mode");
        #endif
            int r = blsInit(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);
            if (r != 0) {
                printf("err blsInit %d\n", r);
                return 1;
            }
        #ifdef BLS_ETH
            r = blsSetETHmode(BLS_ETH_MODE_DRAFT_07);
            if (r != 0) {
                printf("err blsSetETHmode %d\n", r);
                return 1;
            }
        #endif
            return 0;
        }

        crypto_list create(std::vector<int> ids) {
            assert(ids.size() == N_);

            std::vector<blsId> bids(N_);
            blsPublicKey mpk;
            std::vector<blsSecretKey> sks(N_);
            std::vector<blsPublicKey> pks(N_);

            // All ids must be non-zero and different from each other.
            for (int i = 0; i < N_; i++) blsIdSetInt(&bids[i], ids[i]);

            {
                std::vector<blsSecretKey> msk(K_);
                for (int i = 0; i < K_; i++) blsSecretKeySetByCSPRNG(&msk[i]);

                // share secret key
                for (int i = 0; i < N_; i++) blsSecretKeyShare(&sks[i], msk.data(), K_, &bids[i]);

                // get master public key
                blsGetPublicKey(&mpk, &msk[0]);

                // each user gets their own public key
                for (int i = 0; i < N_; i++)  blsGetPublicKey(&pks[i], &sks[i]);
            }

            crypto_list cryptos(N_);
            for (int i = 0; i < N_; i++) {
                cryptos[i] = new crypto_kit{
                    bids[i],
                    sks[i],
                    pks[i],
                    mpk
                };
            }

            return cryptos;
        }

    private:
        int N_;
        int K_;
    };
}

#endif //RARESYNC_CRYPTO_H
