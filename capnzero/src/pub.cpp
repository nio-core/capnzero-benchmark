#include <capnzero/PublisherFlatbuffers.h>
#include <capnzero/PublisherProtobuf.h>
#include <capnzero/PublisherSBE.h>

#include <capnzero/Common.h>
#include <chrono>
#include <thread>
#include <signal.h>

//#define DEBUG_PUB

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

    std::cerr << argc << std::endl;

    if (argc <= 3) {
        std::cerr << "Synopsis: rosrun capnzero pub \"topic\" "
                     "\"String that should be published!\" \"encodingID\"" << std::endl;
        std::cerr << "Encodings:" << std::endl;
        std::cerr << "0: Flatbuffers" << std::endl;
        std::cerr << "1: Protobuf" << std::endl;
        std::cerr << "2: SBE" << std::endl;
        return -1;
    }

    for (size_t i = 0; i < argc; i++) {
        std::cout << "Param " << i << ": '" << argv[i] << "'" << std::endl;
    }

#ifdef DEBUG_PUB
    std::cout << "pub: Message to send: " << beaconMsgBuilder.toString().flatten().cStr() << std::endl;
#endif
    int encodingID = std::stoi(argv[3]);
    if (encodingID == 0) {
        std::cout << "Selected encoding: Flatbuffers" << std::endl;
        void* ctx = zmq_ctx_new();
        auto pub = capnzero::PublisherFlatbuffers(ctx, capnzero::Protocol::UDP);
        auto msgBuilder = pub.createMessage(argv[2]);
        pub.setDefaultTopic(argv[1]);
        pub.addAddress("224.0.0.2:5555");

        while (!interrupted) {
            int numBytesSent = pub.send(msgBuilder);
#ifdef DEBUG_PUB
            std::cout << "pub: " << numBytesSent << " Bytes sent!" << std::endl;
#endif
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // wait until everything is send
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (encodingID == 1) {
        std::cout << "Selected encoding: Protobuf" << std::endl;
        void* ctx = zmq_ctx_new();
        auto pub = capnzero::PublisherProtobuf(ctx, capnzero::Protocol::UDP);
        auto msgBuilder = pub.createMessage(argv[2]);
        pub.setDefaultTopic(argv[1]);
        pub.addAddress("224.0.0.2:5555");

        while (!interrupted) {
            int numBytesSent = pub.send(msgBuilder);
#ifdef DEBUG_PUB
            std::cout << "pub: " << numBytesSent << " Bytes sent!" << std::endl;
#endif
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // wait until everything is send
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (encodingID == 2) {
        std::cout << "Selected encoding: SBE" << std::endl;
        void* ctx = zmq_ctx_new();
        auto pub = capnzero::PublisherSBE(ctx, capnzero::Protocol::UDP);
        auto msgBuilder = pub.createMessage(argv[2]);
        pub.setDefaultTopic(argv[1]);
        pub.addAddress("224.0.0.2:5555");

        while (!interrupted) {
            int numBytesSent = pub.send(msgBuilder);
#ifdef DEBUG_PUB
            std::cout << "pub: " << numBytesSent << " Bytes sent!" << std::endl;
#endif
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // wait until everything is send
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
