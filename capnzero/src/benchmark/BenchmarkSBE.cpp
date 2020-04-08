//
// Created by bjoern on 30.03.20.
//

#include "../../include/benchmark/BenchmarkSBE.h"
namespace capnzero {
    int messagesReceivedSBE = 0;
    bool hasReceivedSBE;
    std::chrono::high_resolution_clock::time_point endSBE;

    void callback(sbe::MessageSBE* msg)
    {
        char id = msg->id();
        long int status = msg->status();
        sbe::MessageSBE::States &states = msg->states();
        while (states.hasNext()) {
            states.next();
        }
        std::string messageInfo = msg->getMessageInfoAsString();
        endSBE = std::chrono::high_resolution_clock::now();
        hasReceivedSBE = true;
        messagesReceivedSBE++;
    }

    capnzero::PublisherSBE createPublisherSBE() {
        void *ctx = zmq_ctx_new();
        PublisherSBE pub = capnzero::PublisherSBE(ctx, capnzero::Protocol::UDP);
        pub.setDefaultTopic("benchmark");
        pub.addAddress("224.0.0.2:5555");
        return pub;
    }

    capnzero::SubscriberSBE *createSubscriberSBE() {
        void *ctx = zmq_ctx_new();
        capnzero::SubscriberSBE *sub = new capnzero::SubscriberSBE(ctx, capnzero::Protocol::UDP);

        sub->setTopic("benchmark");
        sub->addAddress("224.0.0.2:5555");
        sub->subscribe(&callback);

        return sub;
    }

    std::string BenchmarkSBE::messageSizeBenchmark(std::string message) {
        //Setup
        int statesCount = 2;
        int messageSizeInBytes = 0;
        int sendBytes = 0;
        int retries = 0;
        std::chrono::high_resolution_clock::time_point start;
        hasReceivedSBE = false;

        capnzero::PublisherSBE pub = createPublisherSBE();

        capnzero::SubscriberSBE *sub = createSubscriberSBE();

        //running
        std::cout << "running sbe benchmark with size " << message.size() << std::endl;

        void *ctx = zmq_ctx_new();

        do {
            start = std::chrono::high_resolution_clock::now();

            sbe::MessageSBE msg;
            sbe::MessageHeader hdr;
            int size = 21 +
                       + sbe::MessageSBE::States::stateEncodingLength() * statesCount
                       + message.size()
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
                    .next().state(9070150)
                    .next().state(5432);
            msg.putMessageInfo(message.c_str(), strlen(message.c_str()));

            messageSizeInBytes = pub.send(msg);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } while (!hasReceivedSBE);

        auto time = std::chrono::duration_cast<std::chrono::microseconds>(endSBE - start).count();
        std::cout << "time: " << time << "us" << std::endl;
        std::cout << "retries: " << retries << std::endl;

        std::cout << "Message size: " << messageSizeInBytes << " bytes\n" << std::endl;
        delete sub;

        std::stringstream ss;
        ss << "\n\t\t\tmessage size: " << messageSizeInBytes << " bytes" ;
        ss << "\n\t\t\ttime: " << time << "us" << "\n";
        ss << "\t\t\tretries: " << retries << "\n";
        ss << "\t\t\ttime - time for retries: " << time - (1000000 * retries) << "us" << "\n";

        return ss.str();
    }

    std::string BenchmarkSBE::maxMessageRateBenchmark(std::string message, int runs, long nsBetweenMessages) {
        int messagesSend = 0;
        int messageSize = 0;
        int statesCount = 2;
        std::cout << "max message rate sbe benchmark with " << nsBetweenMessages << "ns and " << runs << " runs" << std::endl;
        capnzero::PublisherSBE pub = createPublisherSBE();
        capnzero::SubscriberSBE *sub = createSubscriberSBE();

        messagesReceivedSBE = 0;
        while (runs > messagesSend) {
            sbe::MessageSBE msg;
            sbe::MessageHeader hdr;
            int size = 21 +
                       + sbe::MessageSBE::States::stateEncodingLength() * statesCount
                       + message.size()
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
                    .next().state(9070150)
                    .next().state(5432);
            msg.putMessageInfo(message.c_str(), strlen(message.c_str()));
            messageSize = pub.send(msg);
            messagesSend++;
            std::this_thread::sleep_for(std::chrono::nanoseconds(nsBetweenMessages));
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "messages send: " << messagesSend << std::endl;
        std::cout << "messages received: " << messagesReceivedSBE << std::endl;
        delete sub;

        std::stringstream ss;
        ss << "\t\tns between messages: " << nsBetweenMessages;
        ss << "\n\t\t\tmessage size in bytes: " << messageSize << "\n";
        ss << "\t\t\tsend: " << messagesSend << "\n";
        ss << "\t\t\treceived: " << messagesReceivedSBE << "\n";
        return ss.str();
    }
}
