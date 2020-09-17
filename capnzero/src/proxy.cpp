#include <capnzero/Common.h>

#include <capnzero/SubscriberCapnProto.h>
#include <capnzero/PublisherCapnProto.h>

#include "capnzero-base-msgs/string.capnp.h"
#include <capnp/serialize-packed.h>
#include <zmq.h>
#include <iostream>

#include <capnp/message.h>
#include <kj/array.h>

#include <signal.h>
#include <thread>

//#define DEBUG_PROXY

void callbackCapnProto(::capnp::FlatArrayMessageReader& reader)
{
    std::cout << "Called callback..." << std::endl;
    std::cout << "Message type: CapnProto" << std::endl;
    const char *message = reader.getRoot<capnzero::AlicaEngineInfo>().toString().flatten().cStr();
    std::cout << message << std::endl;
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
    zmq_ctx_term(ctx);
    return 0;
}
