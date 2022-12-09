//
// Created by 黄保罗 on 15.11.22.
//

#pragma once
#ifndef RARESYNC_PROTO_H
#define RARESYNC_PROTO_H

#include "string"
#include "crypto.h"
#include <boost/format.hpp>
#include "sstream"
using namespace std;

namespace raresync::proto {
        typedef uint8_t msg_type;
        const msg_type EMPTY = 0;
        const msg_type EPOCH_COMPLETION = 1;
        const msg_type EPOCH_ENTRANCE = 2;

        static std::string msg_type_to_string(msg_type type) {
            switch (type) {
                case EPOCH_COMPLETION: return "Epoch Completion";
                case EPOCH_ENTRANCE: return "Epoch Entrance";
                default: return "Empty";
            }
        }

        // quorum certificate
        struct qcert {
            msg_type type;
            string value; int view; bsg sig;

            qcert() : type(EMPTY), value("v"), view(-1) {}
        };

        struct message {
            message() : type(EMPTY),to(0), from(0), epoch(-1), value("v"), view(-1), qc(nullptr) {}

            message(char* encoded) {
                istringstream is(encoded);

                /*
                 * type, to, from
                 * epoch, sig
                 * value, view, QC: type, value, view, sig
                 *
                 * */

                string token;
                getline(is, token, ',');
                type = atoi(token.c_str());

                getline(is, token, ',');
                to = atoi(token.c_str());

                getline(is, token, ',');
                from = atoi(token.c_str());

                getline(is, token, ',');
                epoch = atoi(token.c_str());

                getline(is, token, ',');
                from_str(token, sig);

                getline(is, token, ',');
                value = token;

                getline(is, token, ',');
                view = atoi(token.c_str());

                qc = nullptr;

                if (is.eof()) return;

                qc = new qcert();

                getline(is, token, ',');
                qc->type = atoi(token.c_str());

                getline(is, token, ',');
                qc->value = token;

                getline(is, token, ',');
                qc->view = atoi(token.c_str());

                getline(is, token, ',');
                from_str(token, qc->sig);
            }

            string to_raw_str() {
                /*
                 * type, to, from
                 * epoch, sig
                 * value, view, QC: type, value, view, sig
                 *
                 * */

                ostringstream os;
                os << int(type) << "," << to << "," << from << ",";
                os << epoch << "," << to_str(sig) << ",";
                os << value << "," << view;

                if (qc == nullptr) return os.str();

                os << ",";
                os << int(qc->type) << "," << qc->value << ",";
                os << qc->view << "," << to_str(qc->sig);

                return os.str();
            }

            std::string to_log() {
                return (boost::format("type=%1%::to=%2%::from=%3%::epoch=%4%") % msg_type_to_string(type) % to % from % epoch).str();
            }

            msg_type type; int to; int from;
            int epoch; bsg sig;

            string value; int view; qcert* qc;

        };

        // copied from asio.chat_room example
        struct packet {
        public:
            enum {header_length = 4};
            enum {max_body_length = 512};

            packet() : body_length_(0) {}

            const byte* data() const { return data_; }

            byte* data() { return data_; }

            size_t length() { return header_length + body_length_; }

            const byte* body() const { return data_ + header_length; }

            byte* body()  { return data_ + header_length; }

            size_t body_length() {return body_length_; }

            void body_length(size_t length)
            {
                body_length_ = length;
                if (body_length_ > max_body_length)
                    body_length_ = max_body_length;
            }

            bool decode_header()
            {
                using namespace std; // For strncat and atoi.
                char header[header_length + 1] = "";
                strncat(header, (char*)(data_), header_length);
                body_length_ = atoi(header);
                if (body_length_ > max_body_length)
                {
                    body_length_ = 0;
                    return false;
                }
                return true;
            }

            void encode_header()
            {
                using namespace std; // For sprintf and memcpy.
                char header[header_length + 1] = "";
                sprintf(header, "%4d", static_cast<int>(body_length_));
                memcpy(data_, (byte*)header, header_length);
            }

            void clear() {
                memset(data_, 0, sizeof(data_));
            }

        private:
            byte data_[header_length + max_body_length];
            size_t body_length_;
        };

    }

#endif //RARESYNC_PROTO_H
