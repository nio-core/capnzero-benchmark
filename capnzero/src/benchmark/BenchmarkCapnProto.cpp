#include "../../include/benchmark/BenchmarkCapnProto.h"

namespace capnzero {

    int messagesReceivedCapnProto = 0;
    int receivedBytes = 0;
    bool hasReceivedCapnProto;
    std::chrono::high_resolution_clock::time_point endCapnProto;

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

    std::string BenchmarkCapnProto::messageSizeBenchmark(std::string message) {
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

        std::stringstream ss;
        ss << "\n\t\t\tmessage size: " << messageSizeInBytes << " bytes" ;
        ss << "\n\t\t\ttime: " << time << "us" << "\n";
        ss << "\t\t\tretries: " << retries << "\n";
        ss << "\t\t\ttime - time for retries: " << time - (1000000 * retries) << "us" << "\n";

        return ss.str();
    }

    std::string BenchmarkCapnProto::maxMessageRateBenchmark(std::string message, int runs, long nsBetweenMessages) {
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


        std::stringstream ss;
        ss << "\t\tns between messages: " << nsBetweenMessages;
        ss << "\n\t\t\tmessage size in bytes: " << messageSize << "\n";
        ss << "\t\t\tsend: " << messagesSend << "\n";
        ss << "\t\t\treceived: " << messagesReceivedCapnProto << "\n";
        return ss.str();
    }
}