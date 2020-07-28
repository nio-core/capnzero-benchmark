#include <capnzero/Common.h>

//import subscriber
#include <capnzero/SubscriberCapnProto.h>

#include <signal.h>
#include <thread>

//#define DEBUG_SENDER

void* pairSocket;

void callbackCapnProto(std::string msg)
{
    std::cout << "Called callback..." << std::endl;
    std::cout << "Message type: CapnProto" << std::endl;
    std::cout << msg << std::endl;

    zmq_msg_t message;
    zmq_send(pairSocket, strdup(msg.c_str()),
             strlen(msg.c_str()), ZMQ_DONTWAIT);
    zmq_msg_close(&message);
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

    std::cout << "Selected encoding: CapnProto" << std::endl;
    void* ctx = zmq_ctx_new();

    pairSocket = zmq_socket(ctx, ZMQ_PAIR);
    zmq_bind(pairSocket, "tcp://127.0.0.1:45678");

    auto sub = new capnzero::SubscriberCapnProto(ctx, capnzero::Protocol::UDP);
    sub->setTopic("monitoring");

//    sub->addAddress("@capnzero.ipc");
    sub->addAddress("224.0.0.2:9876");
//    sub->addAddress("127.0.0.1:5555");

    sub->subscribe(&callbackCapnProto);
    while (!interrupted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    delete sub;

    zmq_ctx_term(ctx);
    return 0;
}
