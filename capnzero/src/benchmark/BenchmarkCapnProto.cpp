#include "../../include/benchmark/BenchmarkCapnProto.h"

namespace capnzero {

    int messagesReceivedCapnProto = 0;
    int receivedBytes = 0;
    bool hasReceivedCapnProto;
    std::chrono::high_resolution_clock::time_point endCapnProto;

    static void cleanUpMsgData(void *data, void *hint) {
        delete reinterpret_cast<kj::Array <capnp::word> *>(hint);
    }

    void callback(std::string msg)
    {
        endCapnProto = std::chrono::high_resolution_clock::now();
        hasReceivedCapnProto = true;
        messagesReceivedCapnProto++;
    }

    PublisherCapnProto createPublisherCapnProto() {
        void *ctx = zmq_ctx_new();
        PublisherCapnProto pub = capnzero::PublisherCapnProto(ctx, capnzero::Protocol::UDP);
        pub.setDefaultTopic("benchmark");
        pub.addAddress("224.0.0.2:5555");
        return pub;
    }

    SubscriberCapnProto* createSubscriberCapnProto() {
        void* ctx = zmq_ctx_new();
        capnzero::SubscriberCapnProto* sub = new capnzero::SubscriberCapnProto(ctx, capnzero::Protocol::UDP);

        sub->setTopic("benchmark");
        sub->addAddress("224.0.0.2:5555");
        sub->subscribe(&callback);

        return sub;
    }

    json BenchmarkCapnProto::messageSizeBenchmark(std::string message) {
        //Setup
        int messageSizeInBytes = 0;
        int sendBytes = 0;
        int retries = 0;
        std::chrono::high_resolution_clock::time_point start;
        hasReceivedCapnProto = false;

        PublisherCapnProto pub = createPublisherCapnProto();

        SubscriberCapnProto* sub = createSubscriberCapnProto();

        //running
        std::cout << "running capnproto benchmark with size " << message.size() << std::endl;

        void *ctx = zmq_ctx_new();

        do {
            start = std::chrono::high_resolution_clock::now();

            // init builder
            ::capnp::MallocMessageBuilder msgBuilder;
            capnzero::MessageCapnp::Builder beaconMsgBuilder = msgBuilder.initRoot<capnzero::MessageCapnp>();

            // set content
            beaconMsgBuilder.setMessageInfo(message);
            beaconMsgBuilder.setId("uuid-1234");
            beaconMsgBuilder.setStatus(14515141);

            auto states = beaconMsgBuilder.initStates(2);
            states.set(0, 142471971L);
            states.set(1, 89730762L);

            messageSizeInBytes = pub.send(msgBuilder);

            std::this_thread::sleep_for(std::chrono::seconds(1));
        } while (!hasReceivedCapnProto);

        auto time = std::chrono::duration_cast<std::chrono::microseconds>(endCapnProto - start).count();
        std::cout << "time: " << time << "us" << std::endl;
        std::cout << "retries: " << retries << std::endl;

        std::cout << "Message size: " << messageSizeInBytes << " bytes\n" <<  std::endl;
        delete sub;

        json result;
        result["size_in_bytes"] = messageSizeInBytes;
        result["time_in_us"] = time;
        result["retries"] = retries;
        result["time_adjusted"] = (time - (1000000 * retries));

        return result;
    }

    json BenchmarkCapnProto::maxMessageRateBenchmark(std::string message, int runs, long nsBetweenMessages) {
        int messageSize = 0;
        int messagesSend = 0;

        std::cout << "max message rate capnproto benchmark with " << nsBetweenMessages << "ns and " << runs << " runs" << std::endl;
        PublisherCapnProto pub = createPublisherCapnProto();
        SubscriberCapnProto* sub = createSubscriberCapnProto();

        messagesReceivedCapnProto = 0;
        while (runs > messagesSend) {
            // init builder
            ::capnp::MallocMessageBuilder msgBuilder;
            capnzero::MessageCapnp::Builder beaconMsgBuilder = msgBuilder.initRoot<capnzero::MessageCapnp>();

            // set content
            beaconMsgBuilder.setMessageInfo(message);
            beaconMsgBuilder.setId("uuid-1234");
            beaconMsgBuilder.setStatus(14515141);

            auto states = beaconMsgBuilder.initStates(2);
            states.set(0, 142471971L);
            states.set(1, 89730762L);

            messageSize = pub.send(msgBuilder);
            messagesSend++;
            std::this_thread::sleep_for(std::chrono::nanoseconds(nsBetweenMessages));
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "messages send: " << messagesSend << std::endl;
        std::cout << "messages received: " << messagesReceivedCapnProto << std::endl;
        delete sub;

        json result;
        result["ns_between_messages"] = nsBetweenMessages;
        result["size_in_bytes"] = messageSize;
        result["messages_send"] = messagesSend;
        result["messages_received"] = messagesReceivedCapnProto;

        return result;
    }

    json BenchmarkCapnProto::encodeDecodeBenchmark(std::string message, int runs) {
        std::cout << "encode decode benchmark capnp with " << runs << " encodes / decodes and size " << message.size()  << std::endl;

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < runs; i++) {
            /*
             * Encode message
             */
            // init builder
            ::capnp::MallocMessageBuilder msgBuilder;
            capnzero::MessageCapnp::Builder beaconMsgBuilder = msgBuilder.initRoot<capnzero::MessageCapnp>();

            // set content
            beaconMsgBuilder.setMessageInfo(message);
            beaconMsgBuilder.setId("uuid-1234");
            beaconMsgBuilder.setStatus(14515141);

            auto states = beaconMsgBuilder.initStates(2);
            states.set(0, 142471971L);
            states.set(1, 89730762L);

        }

        auto end = std::chrono::high_resolution_clock::now();
        auto timeEncoding = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "encode " << runs << " messages: " << timeEncoding << "ms" << std::endl;

        /*
         * setup message to decode
         */
        // init builder
        ::capnp::MallocMessageBuilder msgBuilder;
        capnzero::MessageCapnp::Builder beaconMsgBuilder = msgBuilder.initRoot<capnzero::MessageCapnp>();

        // set content
        beaconMsgBuilder.setMessageInfo(message);
        beaconMsgBuilder.setId("uuid-1234");
        beaconMsgBuilder.setStatus(14515141);

        auto states = beaconMsgBuilder.initStates(2);
        states.set(0, 142471971L);
        states.set(1, 89730762L);

        kj::Array <capnp::word> wordArrayEncode = capnp::messageToFlatArray(msgBuilder);
        kj::Array <capnp::word> *wordArrayPtr = new kj::Array<capnp::word>(
                kj::mv(wordArrayEncode));

        zmq_msg_t msg;
        zmq_msg_init_data(&msg, wordArrayPtr->begin(), wordArrayPtr->size() * sizeof(capnp::word),
                          &cleanUpMsgData, wordArrayPtr);

        start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < runs; i++) {
            /*
             * Decode message
             */
            int msgSize = zmq_msg_size(&msg);
            auto wordArray = kj::ArrayPtr<capnp::word const>(reinterpret_cast<capnp::word const*>(zmq_msg_data(&msg)), msgSize);
            ::capnp::FlatArrayMessageReader msgReader = ::capnp::FlatArrayMessageReader(wordArray);
            std::string message = msgReader.getRoot<capnzero::MessageCapnp>().toString().flatten().cStr();
        }

        end = std::chrono::high_resolution_clock::now();
        auto timeDecoding = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        free(wordArrayPtr);

        std::cout << "decode " << runs << " messages: " << timeDecoding << "ms\n" << std::endl;

        json result;
        result["size"] = message.size();
        result["runs"] = runs;
        result["time_encoding_ms"] = timeEncoding;
        result["time_decoding_ms"] = timeDecoding;
        return result;
    }
}