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

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace capnzero {
    class BenchmarkFlatbuffers {
    public:
        BenchmarkFlatbuffers() = default;
        ~BenchmarkFlatbuffers() = default;
        json messageSizeBenchmark(std::string message);
        json maxMessageRateBenchmark(std::string message, int runs, long nsBetweenMessages);
        json encodeDecodeBenchmark(std::string message, int runs);
    };
}



#endif //CAPNZERO_BENCHMARKFLATBUFFERS_H
