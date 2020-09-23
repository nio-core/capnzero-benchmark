#include <capnzero/PublisherFlatbuffers.h>
#include <capnzero/PublisherProtobuf.h>
#include <capnzero/PublisherSBE.h>
#include <capnzero/PublisherCapnProto.h>
#include <capnzero/PublisherMsgPack.h>

#include "capnzero-base-msgs/string.capnp.h"
#include <capnp/message.h>
#include <kj/array.h>

#include "capnzero-base-msgs/string.pb.h"

#include "capnzero-base-msgs/string_generated.h"

#include <capnzero/Common.h>
#include <chrono>
#include <thread>
#include <signal.h>

#include <msgpack.hpp>
#include "capnzero/Message.h"

//#define DEBUG_PUB

static bool interrupted = false;

static void cleanUpMsgData(void *data, void *hint) {
    delete reinterpret_cast<kj::Array <capnp::word> *>(hint);
}

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

int main(int argc, char **argv) {
    s_catch_signals();

    std::cerr << argc << std::endl;

    std::cerr << argv[0] << argv[1] << std::endl;

    if (argc <= 3) {
        std::cerr << "Synopsis: rosrun capnzero pub \"topic\" "
                     "\"String that should be published!\" \"encodingID\"" << std::endl;
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

#ifdef DEBUG_PUB
    std::cout << "pub: Message to send: " << beaconMsgBuilder.toString().flatten().cStr() << std::endl;SBE
#endif
    int encodingID = std::stoi(argv[3]);
    if (encodingID == 0) {
        std::cout << "Selected encoding: Flatbuffers" << std::endl;
        void *ctx = zmq_ctx_new();
        auto pub = capnzero::PublisherFlatbuffers(ctx, capnzero::Protocol::UDP);

        //Create message
        flatbuffers::FlatBufferBuilder msgBuilder;
        auto id = msgBuilder.CreateString("uuid-1");
        auto status = 1234;
        auto messageInfo = msgBuilder.CreateString(argv[2]);

        std::vector<long> vec = {1561, 152116, 151616, 7848};
        auto states = msgBuilder.CreateVector(vec);

        auto msg = capnzero::CreateMessageFlatbuffers(msgBuilder, id, status, states, messageInfo);
        msgBuilder.Finish(msg);

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
        // compatible with the version of the headers we compiled against.
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        std::cout << "Selected encoding: Protobuf" << std::endl;
        void *ctx = zmq_ctx_new();
        auto pub = capnzero::PublisherProtobuf(ctx, capnzero::Protocol::UDP);

        capnzero::MessageProtobuf msgBuilder;
        msgBuilder.set_id("uuid-1234");
        msgBuilder.set_status(12151);
        msgBuilder.add_states(245124);
        msgBuilder.add_states(42591865196);
        msgBuilder.add_states(17589169814);
        msgBuilder.set_messageinfo(argv[2]);

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
        int statesCount = 2;
        std::cout << "Selected encoding: SBE" << std::endl;
        void *ctx = zmq_ctx_new();
        auto pub = capnzero::PublisherSBE(ctx, capnzero::Protocol::UDP);

        sbe::MessageSBE msg;
        sbe::MessageHeader hdr;
        int size = 21 +
                   + sbe::MessageSBE::States::stateEncodingLength() * statesCount
                   + strlen(argv[2])
                   + sbe::MessageSBE::messageInfoHeaderLength();
        char buffer[size];
        int bufferLength = sizeof(buffer);

        hdr.wrap(buffer, 0, 0, bufferLength)
                .blockLength(sbe::MessageSBE::sbeBlockLength())
                .templateId(sbe::MessageSBE::sbeTemplateId())
                .schemaId(sbe::MessageSBE::sbeSchemaId())
                .version(sbe::MessageSBE::sbeSchemaVersion());
        msg.wrapForEncode(buffer, hdr.encodedLength(), bufferLength);

        msg.id('u')
                .status(123156)
                .statesCount(statesCount)
                .next().state(9876)
                .next().state(5432);
        msg.putMessageInfo(argv[2], strlen(argv[2]));

        pub.setDefaultTopic(argv[1]);
        pub.addAddress("224.0.0.2:5555");

        while (!interrupted) {
            int numBytesSent = pub.send(msg);
#ifdef DEBUG_PUB
            std::cout << "pub: " << numBytesSent << " Bytes sent!" << std::endl;
#endif
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }


    }

    if (encodingID == 3) {
        std::cout << "Selected encoding: CapnProto" << std::endl;
        void *ctx = zmq_ctx_new();
        auto pub = capnzero::PublisherCapnProto(ctx, capnzero::Protocol::UDP);

        // init builder
        ::capnp::MallocMessageBuilder msgBuilder;
        capnzero::MessageCapnp::Builder beaconMsgBuilder = msgBuilder.initRoot<capnzero::MessageCapnp>();

        // set content
        beaconMsgBuilder.setMessageInfo(argv[2]);
        beaconMsgBuilder.setId("uuid-1234");
        beaconMsgBuilder.setStatus(14515141);

        auto states = beaconMsgBuilder.initStates(2);
        states.set(0, 142471971L);
        states.set(1, 89730762L);

        pub.setDefaultTopic(argv[1]);
        pub.addAddress("224.0.0.2:5555");

        while (!interrupted) {
            //pack msg into zmq_msg_t
            zmq_msg_t msg;
            kj::Array <capnp::word> wordArray = capnp::messageToFlatArray(msgBuilder);
            kj::Array <capnp::word> *wordArrayPtr = new kj::Array<capnp::word>(
                    kj::mv(wordArray)); // will be delete by zero-mq
            capnzero::check(zmq_msg_init_data(&msg, wordArrayPtr->begin(), wordArrayPtr->size() * sizeof(capnp::word),
                                              &cleanUpMsgData, wordArrayPtr), "zmq_msg_init_data");

            int numBytesSent = pub.send(msg);
#ifdef DEBUG_PUB
            std::cout << "pub: " << numBytesSent << " Bytes sent!" << std::endl;
#endif
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // wait until everything is send
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (encodingID == 4) {
        std::cout << "Selected encoding: MsgPack" << std::endl;
        void *ctx = zmq_ctx_new();
        auto pub = capnzero::PublisherMsgPack(ctx, capnzero::Protocol::UDP);

        capnzero::Message message;
        message.id = "uuid-1234";
        message.status = 14515141;
        message.messageInfo = argv[2];
        message.states.push_back(3151512);
        message.states.push_back(2415123);

        pub.setDefaultTopic(argv[1]);
        pub.addAddress("224.0.0.2:5555");

        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, message);

        while (!interrupted) {
            int numBytesSent = pub.send(sbuf);
#ifdef DEBUG_PUB
            std::cout << "pub: " << numBytesSent << " Bytes sent!" << std::endl;
#endif
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // wait until everything is send
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}