//
// Created by bjoern on 30.03.20.
//

#ifndef CAPNZERO_BENCHMARKSBE_H
#define CAPNZERO_BENCHMARKSBE_H

#include "capnzero/PublisherSBE.h"
#include "capnzero/SubscriberSBE.h"

#include <capnzero/Common.h>
#include <chrono>
#include <thread>
#include <signal.h>
#include <string>
#include <sstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace capnzero {
    class BenchmarkSBE {
    public:
        BenchmarkSBE() = default;
        ~BenchmarkSBE() = default;
        json messageSizeBenchmark(std::string message);
        json maxMessageRateBenchmark(std::string message, int runs, long nsBetweenMessages);
        json encodeDecodeBenchmark(std::string message, int runs);
    };
};


#endif //CAPNZERO_BENCHMARKSBE_H
