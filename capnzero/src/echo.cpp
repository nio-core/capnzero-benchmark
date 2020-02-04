#include <capnzero/Common.h>

// import flatbuffers stuff
#include <capnzero/SubscriberFlatbuffers.h>
#include "capnzero-base-msgs/string_generated.h"

#include <signal.h>
#include <thread>

//#define DEBUG_SENDER

void callback(std::string string)
{
    std::cout << "Called callback..." << std::endl;
    std::cout << string << std::endl;
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


static void initializeSubscriberFlatbuffers(char** argv) {
    std::cout << "Selected encoding: Flatbuffers" << std::endl;
    void* ctx = zmq_ctx_new();
    capnzero::SubscriberFlatbuffers* sub = new capnzero::SubscriberFlatbuffers(ctx, capnzero::Protocol::UDP);
    sub->setTopic(argv[1]);

//    sub->addAddress("@capnzero.ipc");
    sub->addAddress("224.0.0.2:5555");
//    sub->addAddress("127.0.0.1:5555");

    sub->subscribe(&callback);
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
        std::cerr << "Flatbuffers: 0" << std::endl;
        return -1;
    }

    for (size_t i = 0; i < argc; i++) {
        std::cout << "Param " << i << ": '" << argv[i] << "'" << std::endl;
    }

    int encoding = std::stoi(argv[2]);
    if (encoding == 0) {
        initializeSubscriberFlatbuffers(argv);
    } else {
        std::cerr << encoding << " is not a correct encodingID" << std::endl;
    }

    return 0;
}
