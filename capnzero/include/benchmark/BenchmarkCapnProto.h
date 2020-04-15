#ifndef CAPNZERO_BENCHMARKCAPNPROTO_H
#define CAPNZERO_BENCHMARKCAPNPROTO_H

#include <capnzero/PublisherCapnProto.h>
#include <capnzero/SubscriberCapnProto.h>

#include "capnzero-base-msgs/string.capnp.h"
#include <capnp/message.h>
#include <kj/array.h>

#include <capnzero/Common.h>
#include <chrono>
#include <thread>
#include <signal.h>
#include <string>
#include <sstream>

namespace capnzero {
    class BenchmarkCapnProto {
    public:
        BenchmarkCapnProto() = default;
        ~BenchmarkCapnProto() = default;
        std::string messageSizeBenchmark(std::string message);
        std::string maxMessageRateBenchmark(std::string message, int runs, long nsBetweenMessages);
        std::string encodeDecodeBenchmark(std::string message, int runs);
    };
}



#endif //CAPNZERO_BENCHMARKCAPNPROTO_H
