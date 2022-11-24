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

        struct message {
            message() : type(EMPTY),to(0), from(0), epoch(-1) {}

            message(byte* encoded) {
                string str((char*)encoded);

                int start = 0; int end = (int)str.find("::");

                string type_str(str.substr(start, end));
                type = std::atoi(type_str.c_str());

                start = end+2; end = (int)str.find("::", start);
                string to_str(str.substr(start, end-start));
                to = std::atoi(to_str.c_str());

                start = end+2; end = (int)str.find("::", start);
                string from_str(str.substr(start, end-start));
                from = std::atoi(from_str.c_str());

                start = end+2; end = (int)str.find("::", start);
                string epoch_str(str.substr(start, end-start));
                epoch = std::atoi(epoch_str.c_str());

                start = end+2; end = (int)str.find("::", start);
                string sig_sz_str(str.substr(start, end-start));
                sig_sz = std::atoi(sig_sz_str.c_str());

                start = end+2;
                sig = deserialize(encoded + start, sig_sz);
            }

            vector<byte> to_raw() {
                auto pre = (boost::format("%1%::%2%::%3%::%4%::%5%::") % int(type) % to % from % epoch % sig_sz).str();
                vector<byte> v;
                for (char i : pre) v.push_back(byte(i));
                for (byte i : sig_hex) v.push_back(i);

                return v;
            }


            std::string to_log() {
                return (boost::format("type=%1%::to=%2%::from=%3%::epoch=%4%") % msg_type_to_string(type) % to % from % epoch).str();
            }

            msg_type type;
            int to;
            int from;
            int epoch;
            size_t sig_sz;
            vector<byte> sig_hex;

            bsg sig;
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
