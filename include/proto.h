//
// Created by 黄保罗 on 15.11.22.
//

#include "string"
#include "crypto.hpp"
#include <boost/format.hpp>

#ifndef RARESYNC_PROTO_H
#define RARESYNC_PROTO_H

namespace raresync::proto {
        typedef uint8_t msg_type;
        const msg_type EMPTY = 0;
        const msg_type EPOCH_COMPLETION = 1;
        const msg_type EPOCH_ENTRANCE = 2;

        std::string msg_type_to_string(msg_type type) {
            switch (type) {
                case EPOCH_COMPLETION: return "Epoch Completion";
                case EPOCH_ENTRANCE: return "Epoch Entrance";
            }
        }

        struct message {
            message() : type(EMPTY),to(0), from(0), epoch(-1), sig() {}

            message(const string& encoded) {
                int start = 0; int end = encoded.find("::");
                int sz = encoded.size();
                type = std::atoi(encoded.substr(start, end).c_str());

                start = end+1; end = encoded.substr(start, sz-start).find("::");
                to = std::atoi(encoded.substr(start, end).c_str());

                start = end+1; end = encoded.substr(start, sz-start).find("::");
                from = std::atoi(encoded.substr(start, end).c_str());

                start = end+1; end = encoded.substr(start, sz-start).find("::");
                epoch = std::atoi(encoded.substr(start, end).c_str());

                start = end+1; end = encoded.substr(start, sz-start).find("::");
                sig_sz = std::atoi(encoded.substr(start, end).c_str());

                start = end+1;
                auto buf = encoded.substr(start, sig_sz).c_str();
                blsSignatureDeserialize(&sig, buf, sig_sz);
            }


            std::string to_string() {
                char buf[64];
                sig_sz = blsSignatureSerialize(buf, 64, &sig);
                auto s = boost::format("%1%::%2%::%3%::%4%::%5%::%6%") % type % to % from % epoch % sig_sz % buf;
                return s.str();
            }

            msg_type type;
            int to;
            int from;
            int epoch;
            bsg sig;

            size_t sig_sz;
        };

        typedef std::shared_ptr<message> message_sptr;

        // copied from asio.chat_room example
        struct packet {
        public:
            enum {header_length = 4};
            enum {max_body_length = 512};

            packet() : body_length_(0) {}

            const char* data() const { return data_; }

            char* data() { return data_; }

            size_t length() { return header_length + body_length_; }

            const char* body() const { return data_ + header_length; }

            char* body()  { return data_ + header_length; }

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
                strncat(header, data_, header_length);
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
                sprintf(header, "%4d", body_length_);
                memcpy(data_, header, header_length);
            }

        private:
            char data_[header_length + max_body_length];
            size_t body_length_;
        };

        typedef std::shared_ptr<packet> packet_sptr;
    }

#endif //RARESYNC_PROTO_H
