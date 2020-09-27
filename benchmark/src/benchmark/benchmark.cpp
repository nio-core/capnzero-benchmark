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
#include "benchmark/BenchmarkMsgPack.h"

#include <sstream>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <sys/wait.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

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
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    int encodeDecodeRuns = 1000000;
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
    filename << "benchmark_" << std::asctime(std::localtime(&timestamp)) << ".json";

    std::ofstream file;
    file.open(filename.str());

    capnzero::BenchmarkFlatbuffers benchmarkFlatbuffers;
    json emptyMessageFlatbuffersResult = benchmarkFlatbuffers.messageSizeBenchmark(emptyMessage);
    json smallMessageFlatbuffersResult = benchmarkFlatbuffers.messageSizeBenchmark(smallMessage);
    json mediumMessageFlatbuffersResult = benchmarkFlatbuffers.messageSizeBenchmark(mediumMessage);
    json largeMessageFlatbuffersResult = benchmarkFlatbuffers.messageSizeBenchmark(largeMessage);

    json emptyMessageFlatbuffersEncodeDecode = benchmarkFlatbuffers.encodeDecodeBenchmark(emptyMessage, encodeDecodeRuns);
    json smallMessageFlatbuffersEncodeDecode = benchmarkFlatbuffers.encodeDecodeBenchmark(smallMessage, encodeDecodeRuns);
    json mediumMessageFlatbuffersEncodeDecode = benchmarkFlatbuffers.encodeDecodeBenchmark(mediumMessage, encodeDecodeRuns);
    json largeMessageFlatbuffersEncodeDecode = benchmarkFlatbuffers.encodeDecodeBenchmark(largeMessage, encodeDecodeRuns);

    capnzero::BenchmarkCapnProto benchmarkCapnProto;
    json emptyMessageCapnProtoResult = benchmarkCapnProto.messageSizeBenchmark(emptyMessage);
    json smallMessageCapnProtoResult = benchmarkCapnProto.messageSizeBenchmark(smallMessage);
    json mediumMessageCapnProtoResult = benchmarkCapnProto.messageSizeBenchmark(mediumMessage);
    json largeMessageCapnProtoResult = benchmarkCapnProto.messageSizeBenchmark(largeMessage);

    json emptyMessageCapnProtoEncodeDecode = benchmarkCapnProto.encodeDecodeBenchmark(emptyMessage, encodeDecodeRuns);
    json smallMessageCapnProtoEncodeDecode = benchmarkCapnProto.encodeDecodeBenchmark(smallMessage, encodeDecodeRuns);
    json mediumMessageCapnProtoEncodeDecode = benchmarkCapnProto.encodeDecodeBenchmark(mediumMessage, encodeDecodeRuns);
    json largeMessageCapnProtoEncodeDecode = benchmarkCapnProto.encodeDecodeBenchmark(largeMessage, encodeDecodeRuns);

    capnzero::BenchmarkProtobuf benchmarkProtobuf;
    json emptyMessageProtobufResult = benchmarkProtobuf.messageSizeBenchmark(emptyMessage);
    json smallMessageProtobufResult = benchmarkProtobuf.messageSizeBenchmark(smallMessage);
    json mediumMessageProtobufResult = benchmarkProtobuf.messageSizeBenchmark(mediumMessage);
    json largeMessageProtobufResult = benchmarkProtobuf.messageSizeBenchmark(largeMessage);

    json emptyMessageProtobufEncodeDecode = benchmarkProtobuf.encodeDecodeBenchmark(emptyMessage, encodeDecodeRuns);
    json smallMessageProtobufEncodeDecode = benchmarkProtobuf.encodeDecodeBenchmark(smallMessage, encodeDecodeRuns);
    json mediumMessageProtobufEncodeDecode = benchmarkProtobuf.encodeDecodeBenchmark(mediumMessage, encodeDecodeRuns);
    json largeMessageProtobufEncodeDecode = benchmarkProtobuf.encodeDecodeBenchmark(largeMessage, encodeDecodeRuns);

    capnzero::BenchmarkSBE benchmarkSBE;
    json emptyMessageSBEResult = benchmarkSBE.messageSizeBenchmark(emptyMessage);
    json smallMessageSBEResult = benchmarkSBE.messageSizeBenchmark(smallMessage);
    json mediumMessageSBEResult = benchmarkSBE.messageSizeBenchmark(mediumMessage);
    json largeMessageSBEResult = benchmarkSBE.messageSizeBenchmark(largeMessage);

    json emptyMessageSBEEncodeDecode = benchmarkSBE.encodeDecodeBenchmark(emptyMessage, encodeDecodeRuns);
    json smallMessageSBEEncodeDecode = benchmarkSBE.encodeDecodeBenchmark(smallMessage, encodeDecodeRuns);
    json mediumMessageSBEEncodeDecode = benchmarkSBE.encodeDecodeBenchmark(mediumMessage, encodeDecodeRuns);
    json largeMessageSBEEncodeDecode = benchmarkSBE.encodeDecodeBenchmark(largeMessage, encodeDecodeRuns);

    capnzero::BenchmarkMsgPack benchmarkMsgPack;
    json emptyMessageMsgPackResult = benchmarkSBE.messageSizeBenchmark(emptyMessage);
    json smallMessageMsgPackResult = benchmarkSBE.messageSizeBenchmark(smallMessage);
    json mediumMessageMsgPackResult = benchmarkSBE.messageSizeBenchmark(mediumMessage);
    json largeMessageMsgPackResult = benchmarkSBE.messageSizeBenchmark(largeMessage);

    json emptyMessageMsgPackEncodeDecode = benchmarkMsgPack.encodeDecodeBenchmark(emptyMessage, encodeDecodeRuns);
    json smallMessageMsgPackEncodeDecode = benchmarkMsgPack.encodeDecodeBenchmark(smallMessage, encodeDecodeRuns);
    json mediumMessageMsgPackEncodeDecode = benchmarkMsgPack.encodeDecodeBenchmark(mediumMessage, encodeDecodeRuns);
    json largeMessageMsgPackEncodeDecode = benchmarkMsgPack.encodeDecodeBenchmark(largeMessage, encodeDecodeRuns);


    json fbMessageSize;
    fbMessageSize["empty"] = emptyMessageFlatbuffersResult;
    fbMessageSize["small"] = smallMessageFlatbuffersResult;
    fbMessageSize["medium"] = mediumMessageFlatbuffersResult;
    fbMessageSize["large"] = largeMessageFlatbuffersResult;

    json fbMaxRate;
    long ns = 1000;
    int runs = 20000;
    while (ns >= 1) {
        fbMaxRate[std::to_string(ns)] = benchmarkFlatbuffers.maxMessageRateBenchmark(largeMessage, runs, ns);
        ns /= 10;
    }

    json fbEncodeDecode;
    fbEncodeDecode["empty"] = emptyMessageFlatbuffersEncodeDecode;
    fbEncodeDecode["small"] = smallMessageFlatbuffersEncodeDecode;
    fbEncodeDecode["medium"] = mediumMessageFlatbuffersEncodeDecode;
    fbEncodeDecode["large"] = largeMessageFlatbuffersEncodeDecode;

    json flatbuffers;
    flatbuffers["message_size"] = fbMessageSize;
    flatbuffers["encode_decode"] = fbEncodeDecode;
    flatbuffers["max_rate"] = fbMaxRate;

    json pbMessageSize;
    pbMessageSize["empty"] = emptyMessageProtobufResult;
    pbMessageSize["small"] = smallMessageProtobufResult;
    pbMessageSize["medium"] = mediumMessageProtobufResult;
    pbMessageSize["large"] = largeMessageProtobufResult;

    json pbEncodeDecode;
    pbEncodeDecode["empty"] = emptyMessageProtobufEncodeDecode;
    pbEncodeDecode["small"] = smallMessageProtobufEncodeDecode;
    pbEncodeDecode["medium"] = mediumMessageProtobufEncodeDecode;
    pbEncodeDecode["large"] = largeMessageProtobufEncodeDecode;

    json pbMaxRate;
    ns = 1000;
    while (ns >= 1) {
        pbMaxRate[std::to_string(ns)] = benchmarkProtobuf.maxMessageRateBenchmark(largeMessage, runs, ns);
        ns /= 10;
    }

    json protobuf;
    protobuf["message_size"] = pbMessageSize;
    protobuf["encode_decode"] = pbEncodeDecode;
    protobuf["max_rate"] = pbMaxRate;

    json capnpMessageSize;
    capnpMessageSize["empty"] = emptyMessageCapnProtoResult;
    capnpMessageSize["small"] = smallMessageCapnProtoResult;
    capnpMessageSize["medium"] = mediumMessageCapnProtoResult;
    capnpMessageSize["large"] = largeMessageCapnProtoResult;

    json capnpEncodeDecode;
    capnpEncodeDecode["empty"] = emptyMessageCapnProtoEncodeDecode;
    capnpEncodeDecode["small"] = smallMessageCapnProtoEncodeDecode;
    capnpEncodeDecode["medium"] = mediumMessageCapnProtoEncodeDecode;
    capnpEncodeDecode["large"] = largeMessageCapnProtoEncodeDecode;

    json capnpMaxRate;
    ns = 1000;
    while (ns >= 1) {
        capnpMaxRate[std::to_string(ns)] = benchmarkCapnProto.maxMessageRateBenchmark(largeMessage, runs, ns);
        ns /= 10;
    }

    json capnp;
    capnp["message_size"] = capnpMessageSize;
    capnp["encode_decode"] = capnpEncodeDecode;
    capnp["max_rate"] = capnpMaxRate;

    json sbeMessageSize;
    sbeMessageSize["empty"] = emptyMessageSBEResult;
    sbeMessageSize["small"] = smallMessageSBEResult;
    sbeMessageSize["medium"] = mediumMessageSBEResult;
    sbeMessageSize["large"] = largeMessageSBEResult;

    json sbeEncodeDecode;
    sbeEncodeDecode["empty"] = emptyMessageSBEEncodeDecode;
    sbeEncodeDecode["small"] = smallMessageSBEEncodeDecode;
    sbeEncodeDecode["medium"] = mediumMessageSBEEncodeDecode;
    sbeEncodeDecode["large"] = largeMessageSBEEncodeDecode;

    json sbeMaxRate;
    ns = 1000;
    while (ns >= 1) {
        sbeMaxRate[std::to_string(ns)] = benchmarkSBE.maxMessageRateBenchmark(largeMessage, runs, ns);
        ns /= 10;
    }

    json sbe;
    sbe["message_size"] = sbeMessageSize;
    sbe["encode_decode"] = sbeEncodeDecode;
    sbe["max_rate"] = sbeMaxRate;

    json msgPackMessageSize;
    msgPackMessageSize["empty"] = emptyMessageMsgPackResult;
    msgPackMessageSize["small"] = smallMessageMsgPackResult;
    msgPackMessageSize["medium"] = mediumMessageMsgPackResult;
    msgPackMessageSize["large"] = largeMessageMsgPackResult;

    json msgPackEncodeDecode;
    msgPackEncodeDecode["empty"] = emptyMessageMsgPackEncodeDecode;
    msgPackEncodeDecode["small"] = smallMessageMsgPackEncodeDecode;
    msgPackEncodeDecode["medium"] = mediumMessageMsgPackEncodeDecode;
    msgPackEncodeDecode["large"] = largeMessageMsgPackEncodeDecode;

    json msgPackMaxRate;
//    ns = 1000;
//    while (ns >= 1) {
//        msgPackMaxRate[std::to_string(ns)] = benchmarkMsgPack.maxMessageRateBenchmark(largeMessage, runs, ns);
//        ns /= 10;
//    }

    json msgPack;
    msgPack["message_size"] =msgPackMessageSize;
    msgPack["encode_decode"] = msgPackEncodeDecode;
    msgPack["max_rate"] = msgPackMaxRate;

    std::cout << "finished benchmark!" << std::endl;

    json root;
    root["flatbuffers"] = flatbuffers;
    root["protobuf"] = protobuf;
    root["capnp"] = capnp;
    root["sbe"] = sbe;
    root["msgpack"] = msgPack;
    file << root.dump();

    file.flush();

     //wait until everything is send
    std::this_thread::sleep_for(std::chrono::seconds(1));

    file.close();

    return 0;
}

