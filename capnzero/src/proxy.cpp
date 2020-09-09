#include <capnzero/Common.h>

//import subscriber
#include <capnzero/SubscriberCapnProto.h>
#include <capnzero/PublisherCapnProto.h>

#include <signal.h>
#include <thread>

//#define DEBUG_SENDER

capnzero::PublisherCapnProto* monitor_socket;
capnzero::PublisherCapnProto* pub;

void callbackCapnProto(::capnp::FlatArrayMessageReader& reader)
{
    std::cout << "Called callback..." << std::endl;
    std::cout << "Message type: CapnProto" << std::endl;
    reader.getRoot<capnzero::AlicaEngineInfo>().toString().flatten().cStr();
//    const char *message = reader.getRoot<alica_msgs::AlicaEngineInfo>().toString().flatten().cStr();
//    std::cout << message << std::endl;

//    ::capnp::MallocMessageBuilder msgBuilder;
//    msgBuilder.setRoot()
//    capnzero::MessageCapnp::Builder beaconMsgBuilder = msgBuilder.initRoot<capnzero::AlicaEngineInfo>();
//    beaconMsgBuilder.set

    // init builder
//    ::capnp::MallocMessageBuilder msgBuilder;
//    msgBuilder.setRoot()
//    capnzero::MessageCapnp::Builder beaconMsgBuilder = msgBuilder.initRoot<capnzero::MessageCapnp>();
//
//    // set content
//    beaconMsgBuilder.setMessageInfo(argv[2]);
//    beaconMsgBuilder.setId("uuid-1234");
//    beaconMsgBuilder.setStatus(14515141);
//
//    auto states = beaconMsgBuilder.initStates(2);
//    states.set(0, 142471971L);
//    states.set(1, 89730762L);
//
//    const char *message = reader.getRoot<alica_msgs::AlicaEngineInfo>().toString().flatten().cStr();
//    std::cout << message << std::endl;
//    zmq_msg_t out_msg;
//    zmq_msg_init_size(&out_msg, strlen(message));
//    memcpy(zmq_msg_data(&out_msg), message, strlen(message));
//    zmq_msg_send(&out_msg, monitor_socket, ZMQ_DONTWAIT);
//    zmq_msg_close(&out_msg);
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

int main(int argc, char** argv)
{
    s_catch_signals();
    for (size_t i = 0; i < argc; i++) {
        std::cout << "Param " << i << ": '" << argv[i] << "'" << std::endl;
    }

    void *ctx = zmq_init(1);
    auto sub = new capnzero::SubscriberCapnProto(ctx, capnzero::Protocol::UDP);
    sub->setTopic("/AE/AEInfo");
    sub->addAddress("224.0.0.2:5555");
    sub->subscribe(&callbackCapnProto);

    auto pub = new capnzero::PublisherCapnProto(ctx, capnzero::Protocol::TCP);
    pub->setDefaultTopic("/AE/AEInfo");
    pub->addAddress("*:12345", true);

    while (!interrupted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    delete sub;
//    zmq_close(monitor_socket);
    zmq_ctx_term(ctx);
    return 0;
}
