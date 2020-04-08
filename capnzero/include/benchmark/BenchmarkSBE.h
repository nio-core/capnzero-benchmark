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

namespace capnzero {
    class BenchmarkSBE {
    public:
        BenchmarkSBE() = default;
        ~BenchmarkSBE() = default;
        std::string messageSizeBenchmark(std::string message);
        std::string maxMessageRateBenchmark(std::string message, int runs, long nsBetweenMessages);
    };
};


#endif //CAPNZERO_BENCHMARKSBE_H
