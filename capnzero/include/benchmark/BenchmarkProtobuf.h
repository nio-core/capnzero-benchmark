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

namespace capnzero {
    class BenchmarkProtobuf {
    public:
        BenchmarkProtobuf() = default;
        ~BenchmarkProtobuf() = default;
        std::string messageSizeBenchmark(std::string message);
        std::string maxMessageRateBenchmark(std::string message, int runs, long nsBetweenMessages);
    };
}


#endif //CAPNZERO_BENCHMARKPROTOBUF_H
