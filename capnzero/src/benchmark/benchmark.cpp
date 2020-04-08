#include "capnzero-base-msgs/string.capnp.h"
#include <capnp/message.h>
#include <kj/array.h>

#include <capnzero/Common.h>
#include <chrono>
#include <thread>
#include <signal.h>
#include <ctime>

#include "benchmark/BenchmarkFlatbuffers.h"
#include "benchmark/BenchmarkCapnProto.h"
#include "benchmark/BenchmarkProtobuf.h"
#include "benchmark/BenchmarkSBE.h"

#include <sstream>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <sys/wait.h>

static bool interrupted = false;

static void s_signal_handler(int signal_value) {
    interrupted = true;
}

static void s_catch_signals(void) {
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

static std::string generateRandomMessage(int size) {
    std::stringstream ss;

    for (int i = 0; i < size; i++) {
        char c = '!' + (rand() % (127 - 33));
        ss << c;
    }
    return ss.str();
}

int main(int argc, char** argv)
{
    s_catch_signals();
    std::cerr << argc << std::endl;

    for (int i = 0; i < argc; i++) {
        std::cerr << i << ": " << argv[i] << std::endl;
    }

    std::cout << "generating messages..." << std::endl;
    std::string emptyMessage = "";
    std::string smallMessage = generateRandomMessage(10);
    std::string mediumMessage = generateRandomMessage(100);
    std::string largeMessage = generateRandomMessage(1000);
    std::cout << "finished generating messages!\n" << std::endl;

    std::time_t timestamp = std::time(nullptr);
    std::stringstream filename;
    filename << "benchmark_" << std::asctime(std::localtime(&timestamp));

    std::ofstream file;
    file.open(filename.str());

    capnzero::BenchmarkFlatbuffers benchmarkFlatbuffers;
    std::string emptyMessageFlatbuffersResult = benchmarkFlatbuffers.messageSizeBenchmark(emptyMessage);
    std::string smallMessageFlatbuffersResult = benchmarkFlatbuffers.messageSizeBenchmark(smallMessage);
    std::string mediumMessageFlatbuffersResult = benchmarkFlatbuffers.messageSizeBenchmark(mediumMessage);
    std::string largeMessageFlatbuffersResult = benchmarkFlatbuffers.messageSizeBenchmark(largeMessage);

    capnzero::BenchmarkCapnProto benchmarkCapnProto;
    std::string emptyMessageCapnProtoResults = benchmarkCapnProto.messageSizeBenchmark(emptyMessage);
    std::string smallMessageCapnProtoResults = benchmarkCapnProto.messageSizeBenchmark(smallMessage);
    std::string mediumMessageCapnProtoResults = benchmarkCapnProto.messageSizeBenchmark(mediumMessage);
    std::string largeMessageCapnProtoResults = benchmarkCapnProto.messageSizeBenchmark(largeMessage);

    capnzero::BenchmarkProtobuf benchmarkProtobuf;
    std::string emptyMessageProtobufResult = benchmarkProtobuf.messageSizeBenchmark(emptyMessage);
    std::string smallMessageProtobufResult = benchmarkProtobuf.messageSizeBenchmark(smallMessage);
    std::string mediumMessageProtobufResult = benchmarkProtobuf.messageSizeBenchmark(mediumMessage);
    std::string largeMessageProtobufResult = benchmarkProtobuf.messageSizeBenchmark(largeMessage);

    capnzero::BenchmarkSBE benchmarkSBE;
    std::string emptyMessageSBEResult = benchmarkSBE.messageSizeBenchmark(emptyMessage);
    std::string smallMessageSBEResult = benchmarkSBE.messageSizeBenchmark(smallMessage);
    std::string mediumMessageSBEResult = benchmarkSBE.messageSizeBenchmark(mediumMessage);
    std::string largeMessageSBEResult = benchmarkSBE.messageSizeBenchmark(largeMessage);

    file << "Flatbuffers";
    file << "\n\tmessage size: ";
    file << "\n\t\tempty: " << emptyMessageFlatbuffersResult;
    file << "\n\t\tsmall: " << smallMessageFlatbuffersResult;
    file << "\n\t\tmedium: " << mediumMessageFlatbuffersResult;
    file << "\n\t\tlarge: " << largeMessageFlatbuffersResult;
    file.flush();

    pid_t flatbuffers_pid = fork();
    if (flatbuffers_pid == 0) {
        long ns = 1000;
        int runs = 20000;
        file << "\n\tmax rate:\n";
        while (ns >= 1) {
            std::string result = benchmarkFlatbuffers.maxMessageRateBenchmark(largeMessage, runs, ns);
            file << result;
            std::cout << "success: " << ns << "ns\n" << std::endl;
            ns /= 10;
        }
        return 0;
    } else {
        wait(NULL);
    }

    file << "\n\nProtobuf";
    file << "\n\tmessage size: ";
    file << "\n\t\tempty: " << emptyMessageProtobufResult;
    file << "\n\t\tsmall: " << smallMessageProtobufResult;
    file << "\n\t\tmedium: " << mediumMessageProtobufResult;
    file << "\n\t\tlarge: " << largeMessageProtobufResult;
    file.flush();

    pid_t protobuf_pid = fork();
    if (protobuf_pid == 0) {
        long ns = 1000;
        int runs = 20000;
        file << "\n\tmax rate:\n";
        while (ns >= 1) {
            std::string result = benchmarkProtobuf.maxMessageRateBenchmark(largeMessage, runs, ns);
            file << result;
            std::cout << "success: " << ns << "ns\n" << std::endl;
            ns /= 10;
        }
        return 0;
    } else {
        wait(NULL);
    }

    file << "\n\nCapnProto";
    file << "\n\tmessage size: ";
    file << "\n\t\tempty: " << emptyMessageCapnProtoResults;
    file << "\n\t\tsmall: " << smallMessageCapnProtoResults;
    file << "\n\t\tmedium: " << mediumMessageCapnProtoResults;
    file << "\n\t\tlarge: " << largeMessageCapnProtoResults;
    file.flush();

    pid_t capnproto_pid = fork();
    if (capnproto_pid == 0) {
        long ns = 1000;
        int runs = 20000;
        file << "\n\tmax rate:\n";
        while (ns >= 1) {
            std::string result = benchmarkCapnProto.maxMessageRateBenchmark(largeMessage, runs, ns);
            file << result;
            std::cout << "success: " << ns << "ns\n" << std::endl;
            ns /= 10;
        }
        return 0;
    } else {
        wait(NULL);
    }

    file << "\n\nSBE";
    file << "\n\tmessage size: ";
    file << "\n\t\tempty: " << emptyMessageSBEResult;
    file << "\n\t\tsmall: " << smallMessageSBEResult;
    file << "\n\t\tmedium: " << mediumMessageSBEResult;
    file << "\n\t\tlarge: " << largeMessageSBEResult;
    file.flush();

    pid_t sbe_pid = fork();
    if (sbe_pid == 0) {
        long ns = 1000;
        int runs = 20000;
        file << "\n\tmax rate:\n";
        while (ns >= 1) {
            std::string result = benchmarkSBE.maxMessageRateBenchmark(largeMessage, runs, ns);
            file << result;
            std::cout << "success: " << ns << "ns\n" << std::endl;
            ns /= 10;
        }
        return 0;
    } else {
        wait(NULL);
    }

    std::cout << "finished benchmark!" << std::endl;

     //wait until everything is send
    std::this_thread::sleep_for(std::chrono::seconds(1));

    file.close();

    return 0;
}

