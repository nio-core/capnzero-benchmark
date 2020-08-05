#include <capnzero/Common.h>

//import subscriber
#include <capnzero/SubscriberCapnProto.h>

#include <signal.h>
#include <thread>

//#define DEBUG_SENDER

void* monitor_socket;

void callbackCapnProto(std::string msg)
{
    std::cout << "Called callback..." << std::endl;
    std::cout << "Message type: CapnProto" << std::endl;

    zmq_msg_t zmq_msg;
    zmq_msg_init(&zmq_msg);
    std::cout << "wait for request..." << std::endl;
    int rc = zmq_msg_recv(&zmq_msg, monitor_socket, 0);
    if (rc != -1) {
        std::cout << "received request" << std::endl;
        std::string msg_str(static_cast<char*>(zmq_msg_data(&zmq_msg)), zmq_msg_size(&zmq_msg));
        std::cout << msg_str << std::endl;
        zmq_send(monitor_socket, msg.c_str(), msg.length(), 0);
    } else {
        std::cerr << "not received request: error code " << zmq_errno() << std::endl;
    }
    zmq_msg_close(&zmq_msg);
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

    void* ctx = zmq_ctx_new();
    monitor_socket = zmq_socket(ctx, ZMQ_REP);
    int timeout = 1000;
    zmq_setsockopt(monitor_socket, ZMQ_SNDTIMEO, &timeout, sizeof(int));
    zmq_setsockopt(monitor_socket, ZMQ_RCVTIMEO, &timeout, sizeof(int));

    int rc = zmq_bind(monitor_socket, "tcp://*:12345");
    if (rc == 1) {
        std::cerr << "error connecting to address tcp://*:12345"
                  << ": error code " << zmq_errno() << std::endl;
        return -1;
    }

    auto sub = new capnzero::SubscriberCapnProto(ctx, capnzero::Protocol::UDP);
    sub->setTopic("monitoring");
    sub->addAddress("224.0.0.2:9876");
    sub->subscribe(&callbackCapnProto);
    while (!interrupted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    delete sub;
    free(monitor_socket);
    zmq_ctx_term(ctx);
    return 0;
}
