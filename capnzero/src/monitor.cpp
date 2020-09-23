#include <capnzero/Common.h>

//import subscriber
#include <capnzero/SubscriberCapnProto.h>

#include <signal.h>
#include <thread>

//#define DEBUG_SENDER

void* monitor_socket;

void callbackCapnProto(zmq_msg_t &msg)
{
    std::cout << "Called callback..." << std::endl;
    std::cout << "Message type: CapnProto" << std::endl;

    zmq_msg_send(&msg, monitor_socket, ZMQ_DONTWAIT);
    zmq_msg_close(&msg);
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
    monitor_socket = zmq_socket(ctx, ZMQ_PUB);
    zmq_bind(monitor_socket, "tcp://*:12345");
    zmq_sleep(1);

    auto sub = new capnzero::SubscriberCapnProto(ctx, capnzero::Protocol::UDP);
    sub->setTopic("monitoring");
    sub->addAddress("224.0.0.2:9876");
    sub->subscribe(&callbackCapnProto);
    while (!interrupted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    delete sub;
    zmq_close(monitor_socket);
    zmq_ctx_term(ctx);
    return 0;
}
