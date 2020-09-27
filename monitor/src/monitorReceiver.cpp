#include <capnzero/Common.h>

//import subscriber
#include <capnzero/SubscriberCapnProto.h>
#include <capnzero/PublisherCapnProto.h>

#include <signal.h>
#include <thread>

//#define DEBUG_SENDER

void* monitor_socket;

void callbackCapnProto(zmq_msg_t &msg)
{
    std::cout << "Called callback..." << std::endl;
    std::cout << "Received CapnZero monitor message" << std::endl;

    //decode and print msg
    int msgSize = zmq_msg_size(&msg);
    auto wordArray = kj::ArrayPtr<capnp::word const>(reinterpret_cast<capnp::word const*>(zmq_msg_data(&msg)), msgSize);
    ::capnp::FlatArrayMessageReader msgReader = ::capnp::FlatArrayMessageReader(wordArray);
    std::string message = msgReader.getRoot<capnzero::String>().toString().flatten().cStr();
    std::cout << message << std::endl;

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
    auto sub = new capnzero::SubscriberCapnProto(ctx, capnzero::Protocol::TCP);
    sub->setTopic("monitoring");
    sub->addAddress("127.0.0.1:12345", false);
    sub->subscribe(&callbackCapnProto);

    while (!interrupted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    delete sub;
    zmq_close(monitor_socket);
    zmq_ctx_term(ctx);
    return 0;
}
