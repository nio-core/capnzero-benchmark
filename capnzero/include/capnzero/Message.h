#ifndef CAPNZERO_MESSAGE_H
#define CAPNZERO_MESSAGE_H

#include <msgpack.hpp>

namespace capnzero
{
    class Message
    {
    public:
        std::string id;
        long status;
        std::string messageInfo;
        std::vector<long> states;
        MSGPACK_DEFINE(id, status, messageInfo, states);
    };
}

#endif //CAPNZERO_MESSAGE_H
