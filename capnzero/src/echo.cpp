#include <capnzero/Common.h>

//generated flatbuffers header
#include "capnzero-base-msgs/string_generated.h"

//import subscriber
#include <capnzero/SubscriberFlatbuffers.h>
#include <capnzero/SubscriberProtobuf.h>
#include <capnzero/SubscriberSBE.h>
#include <capnzero/SubscriberCapnProto.h>
#include <capnzero/SubscriberMsgPack.h>

#include <signal.h>
#include <thread>

//#define DEBUG_SENDER

void callbackMsgPack(capnzero::Message msg)
{
    std::cout << "Called callback..." << std::endl;
    std::cout << "Message type: MsgPack" << std::endl;
    std::cout << "id: " << msg.id << std::endl;
    std::cout << "Status: " << msg.status << std::endl;
    std::cout << "States: " << std::endl;
    for (auto it = msg.states.begin(); it != msg.states.end(); it++) {
        std::cout << *it << std::endl;
    }
    std::cout << "Message Info: " << msg.messageInfo << '\n' << std::endl;
}

void callbackFlatbuffers(const capnzero::MessageFlatbuffers& msg)
{
    auto id = msg.id()->c_str();
    long status = msg.status();
    auto states = msg.states();
    auto messageInfo = msg.messageInfo()->c_str();

    std::cout << "Called callback..." << std::endl;
    std::cout << "Message type: Flatbuffers" << std::endl;
    std::cout << "id: " << id << std::endl;
    std::cout << "Status: " << status << std::endl;
    std::cout << "States: " << std::endl;
    for (auto it = states->begin(); it != states->end(); it++) {
        std::cout << *it << std::endl;
    }
    std::cout << "Message Info: " << messageInfo << '\n' << std::endl;
}

void callbackProtobuf(capnzero::MessageProtobuf* msg)
{
    std::string id = msg->id();
    long int status = msg->status();
    google::protobuf::RepeatedField<long int> states = msg->states();
    std::string messageInfo = msg->messageinfo();

    std::cout << "Called callback..." << std::endl;
    std::cout << "Message type: Protobuf" << std::endl;
    std::cout << "id: " << id << std::endl;
    std::cout << "Status: " << status << std::endl;
    std::cout << "States: " << std::endl;

    for (auto it = states.begin(); it != states.end(); it++) {
        std::cout << *it << std::endl;
    }
    std::cout << "Message Info: " << messageInfo << '\n' << std::endl;
}

void callbackSBE(sbe::MessageSBE* msg)
{
//    msg.wrapAndApplyHeader(data, 0, bufferLength);
    char id = msg->id();
    long int status = msg->status();
    sbe::MessageSBE::States states = msg->states();
    std::string messageInfo = msg->messageInfo();

    std::cout << "Called callback..." << std::endl;
    std::cout << "Message type: SBE" << std::endl;
    std::cout << "id: " << id << std::endl;
    std::cout << "Status: " << status << std::endl;
    std::cout << "States: " << std::endl;

    std::cout << "Message Info: " << messageInfo << '\n' << std::endl;
}

void callbackCapnProto(::capnp::FlatArrayMessageReader& reader)
{
    std::cout << "Called callback..." << std::endl;
    std::cout << "Message type: CapnProto" << std::endl;
    std::cout << reader.getRoot<capnzero::String>().toString().flatten().cStr() << std::endl;
}

static bool interrupted = false;
static void s_signal_handler(int signal_value)
{
    interrupted = true;
}

static void s_catch_signals(void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

static void initializeSubscriberMsgPack(char** argv) {
    std::cout << "Selected encoding: MsgPack" << std::endl;
    void* ctx = zmq_ctx_new();
    capnzero::SubscriberMsgPack* sub = new capnzero::SubscriberMsgPack(ctx, capnzero::Protocol::UDP);
    sub->setTopic(argv[1]);

//    sub->addAddress("@capnzero.ipc");
    sub->addAddress("224.0.0.2:5555");
//    sub->addAddress("127.0.0.1:5555");

    sub->subscribe(&callbackMsgPack);
    while (!interrupted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    delete sub;

    zmq_ctx_term(ctx);
}


static void initializeSubscriberFlatbuffers(char** argv) {
    std::cout << "Selected encoding: Flatbuffers" << std::endl;
    void* ctx = zmq_ctx_new();
    capnzero::SubscriberFlatbuffers* sub = new capnzero::SubscriberFlatbuffers(ctx, capnzero::Protocol::UDP);
    sub->setTopic(argv[1]);

//    sub->addAddress("@capnzero.ipc");
    sub->addAddress("224.0.0.2:5555");
//    sub->addAddress("127.0.0.1:5555");

    sub->subscribe(&callbackFlatbuffers);
    while (!interrupted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    delete sub;

    zmq_ctx_term(ctx);
}

static void initializeSubscriberProtobuf(char** argv) {
    std::cout << "Selected encoding: Protobuf" << std::endl;
    void* ctx = zmq_ctx_new();
    capnzero::SubscriberProtobuf* sub = new capnzero::SubscriberProtobuf(ctx, capnzero::Protocol::UDP);
    sub->setTopic(argv[1]);

//    sub->addAddress("@capnzero.ipc");
    sub->addAddress("224.0.0.2:5555");
//    sub->addAddress("127.0.0.1:5555");

    sub->subscribe(&callbackProtobuf);
    while (!interrupted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    delete sub;

    zmq_ctx_term(ctx);
}

static void initializeSubscriberSBE(char** argv) {
    std::cout << "Selected encoding: SBE" << std::endl;
    void* ctx = zmq_ctx_new();
    auto sub = new capnzero::SubscriberSBE(ctx, capnzero::Protocol::UDP);
    sub->setTopic(argv[1]);

//    sub->addAddress("@capnzero.ipc");
    sub->addAddress("224.0.0.2:5555");
//    sub->addAddress("127.0.0.1:5555");

    sub->subscribe(&callbackSBE);
    while (!interrupted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    delete sub;

    zmq_ctx_term(ctx);
}

static void initializeSubscriberCapnProto(char** argv) {
    std::cout << "Selected encoding: CapnProto" << std::endl;
    void* ctx = zmq_ctx_new();
    auto sub = new capnzero::SubscriberCapnProto(ctx, capnzero::Protocol::UDP);
    sub->setTopic(argv[1]);

//    sub->addAddress("@capnzero.ipc");
    sub->addAddress("224.0.0.2:5555");
//    sub->addAddress("127.0.0.1:5555");

    sub->subscribe(&callbackCapnProto);
    while (!interrupted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    delete sub;

    zmq_ctx_term(ctx);
}

int main(int argc, char** argv)
{
    s_catch_signals();

    if (argc <= 2) {
        std::cerr << "Synopsis: rosrun capnzero echo \"Topic that should be listened to!\" encodingID" << std::endl;
        std::cerr << "Encodings:" << std::endl;
        std::cerr << "0: Flatbuffers" << std::endl;
        std::cerr << "1: Protobuf" << std::endl;
        std::cerr << "2: SBE" << std::endl;
        std::cerr << "3: CapnProto" << std::endl;
        std::cerr << "4: MsgPack" << std::endl;
        return -1;
    }

    for (size_t i = 0; i < argc; i++) {
        std::cout << "Param " << i << ": '" << argv[i] << "'" << std::endl;
    }

    int encoding = std::stoi(argv[2]);
    if (encoding == 0) {
        initializeSubscriberFlatbuffers(argv);
        return 0;
    }

    if (encoding == 1) {
        initializeSubscriberProtobuf(argv);
        return 0;
    }

    if (encoding == 2) {
        initializeSubscriberSBE(argv);
        return 0;
    }

    if (encoding == 3) {
        initializeSubscriberCapnProto(argv);
    }

    if (encoding == 4) {
        initializeSubscriberMsgPack(argv);
    }

    std::cerr << encoding << " is not a correct encodingID" << std::endl;

    return 0;
}
