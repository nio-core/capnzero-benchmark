#ifndef CAPNZERO_BENCHMARKFLATBUFFERS_H
#define CAPNZERO_BENCHMARKFLATBUFFERS_H

#include <capnzero/PublisherFlatbuffers.h>
#include "capnzero/SubscriberFlatbuffers.h"

#include <capnzero/Common.h>
#include <chrono>
#include <thread>
#include <signal.h>
#include <string>
#include <sstream>

namespace capnzero {
    class BenchmarkFlatbuffers {
    public:
        BenchmarkFlatbuffers() = default;
        ~BenchmarkFlatbuffers() = default;
        std::string messageSizeBenchmark(std::string message);
        std::string maxMessageRateBenchmark(std::string message, int runs, long nsBetweenMessages);
    };
}



#endif //CAPNZERO_BENCHMARKFLATBUFFERS_H
