#ifndef CAPNZERO_BENCHMARKPROTOBUF_H
#define CAPNZERO_BENCHMARKPROTOBUF_H

#include "capnzero-base-msgs/string.pb.h"

#include "capnzero/PublisherProtobuf.h"
#include "capnzero/SubscriberProtobuf.h"

#include <capnzero/Common.h>
#include <chrono>
#include <thread>
#include <signal.h>
#include <string>
#include <sstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace capnzero {
    class BenchmarkProtobuf {
    public:
        BenchmarkProtobuf() = default;
        ~BenchmarkProtobuf() = default;
        json messageSizeBenchmark(std::string message);
        json maxMessageRateBenchmark(std::string message, int runs, long nsBetweenMessages);
        json encodeDecodeBenchmark(std::string message, int runs);
    };
}


#endif //CAPNZERO_BENCHMARKPROTOBUF_H
