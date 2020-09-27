#ifndef CAPNZERO_BENCHMARKMSGPACK_H
#define CAPNZERO_BENCHMARKMSGPACK_H

#include "capnzero/PublisherMsgPack.h"
#include "capnzero/SubscriberMsgPack.h"

#include <capnzero/Common.h>
#include <chrono>
#include <thread>
#include <signal.h>
#include <string>
#include <sstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace capnzero {
    class BenchmarkMsgPack {
    public:
        BenchmarkMsgPack() = default;
        ~BenchmarkMsgPack() = default;
        json messageSizeBenchmark(std::string message);
        json maxMessageRateBenchmark(std::string message, int runs, long nsBetweenMessages);
        json encodeDecodeBenchmark(std::string message, int runs);
        //void callback(const capnzero::Message msg);
        PublisherMsgPack createPublisher();
        SubscriberMsgPack* createSubscriber();
//        bool hasReceived = false;
//        int messagesReceived = 0;
    };
}



#endif //CAPNZERO_BENCHMARKMSGPACK_H
