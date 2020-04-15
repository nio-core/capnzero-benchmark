#include "../../include/benchmark/BenchmarkFlatbuffers.h"

namespace capnzero {

    bool hasReceivedFlatbuffers;
    int messagesReceivedFlatbuffers = 0;
    std::chrono::high_resolution_clock::time_point endFlatbuffers;

    void callback(const capnzero::MessageFlatbuffers& msg)
    {
        auto id = msg.id()->c_str();
        long status = msg.status();
        auto states = msg.states();
        auto messageInfo = msg.messageInfo()->c_str();
        endFlatbuffers = std::chrono::high_resolution_clock::now();
        hasReceivedFlatbuffers = true;
        messagesReceivedFlatbuffers++;
    }

    PublisherFlatbuffers createPublisherFlatbuffers() {
        void *ctx = zmq_ctx_new();
        PublisherFlatbuffers pub = capnzero::PublisherFlatbuffers(ctx, capnzero::Protocol::UDP);
        pub.setDefaultTopic("benchmark");
        pub.addAddress("224.0.0.2:5555");
        return pub;
    }

    SubscriberFlatbuffers* createSubscriberFlatbuffers() {
        void* ctx = zmq_ctx_new();
        capnzero::SubscriberFlatbuffers* sub = new capnzero::SubscriberFlatbuffers(ctx, capnzero::Protocol::UDP);

        sub->setTopic("benchmark");
        sub->addAddress("224.0.0.2:5555");
        sub->subscribe(&callback);

        return sub;
    }

    std::string BenchmarkFlatbuffers::messageSizeBenchmark(std::string message) {
        //Setup
        int messageSizeInBytes = 0;
        int sendBytes = 0;
        int retries = 0;
        std::chrono::high_resolution_clock::time_point start;
        hasReceivedFlatbuffers = false;

        PublisherFlatbuffers pub = createPublisherFlatbuffers();

        SubscriberFlatbuffers* sub = createSubscriberFlatbuffers();

        //running
        std::cout << "running flatbuffers benchmark with size " << message.size() << std::endl;

        do {
            start = std::chrono::high_resolution_clock::now();

            flatbuffers::FlatBufferBuilder msgBuilder;
            auto id = msgBuilder.CreateString("uuid-1");
            auto status = 1234;
            auto messageInfo = msgBuilder.CreateString(message);

            std::vector<long> vec = {1561,152116,151616,7848};
            auto states = msgBuilder.CreateVector(vec);

            auto msg = CreateMessageFlatbuffers(msgBuilder, id, status, states, messageInfo);
            msgBuilder.Finish(msg);

            messageSizeInBytes = pub.send(msgBuilder);

            std::this_thread::sleep_for(std::chrono::seconds(1));
        } while (!hasReceivedFlatbuffers);

        auto time = std::chrono::duration_cast<std::chrono::microseconds>(endFlatbuffers - start).count();
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

    std::string BenchmarkFlatbuffers::maxMessageRateBenchmark(std::string message, int runs, long nsBetweenMessages) {
        int messagesSend = 0;
        int messageSize = 0;

        std::cout << "max message rate flatbuffers benchmark with " << nsBetweenMessages << "ns and " << runs << " runs" << std::endl;
        PublisherFlatbuffers pub = createPublisherFlatbuffers();
        SubscriberFlatbuffers* sub = createSubscriberFlatbuffers();

        messagesReceivedFlatbuffers = 0;

        while (runs > messagesSend) {
            flatbuffers::FlatBufferBuilder msgBuilder;
            auto id = msgBuilder.CreateString("uuid-1234");
            auto status = 1234;
            auto messageInfo = msgBuilder.CreateString(message);

            std::vector<long> vec = {1561,152116,151616,7848};
            auto states = msgBuilder.CreateVector(vec);

            auto msg = CreateMessageFlatbuffers(msgBuilder, id, status, states, messageInfo);
            msgBuilder.Finish(msg);
            messageSize = pub.send(msgBuilder);
            messagesSend++;
            std::this_thread::sleep_for(std::chrono::nanoseconds(nsBetweenMessages));
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "messages send: " << messagesSend << std::endl;
        std::cout << "messages received: " << messagesReceivedFlatbuffers << std::endl;

        std::stringstream ss;
        ss << "\t\tns between messages: " << nsBetweenMessages;
        ss << "\n\t\t\tmessage size in bytes: " << messageSize << "\n";
        ss << "\t\t\tsend: " << messagesSend << "\n";
        ss << "\t\t\treceived: " << messagesReceivedFlatbuffers << "\n";
        delete sub;

        return ss.str();
    }

    std::string BenchmarkFlatbuffers::encodeDecodeBenchmark(std::string message, int runs) {
        std::cout << "encode decode benchmark flatbuffers with " << runs << " encodes / decodes and size " << message.size() << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < runs; i++) {
            flatbuffers::FlatBufferBuilder msgBuilder;
            auto id = msgBuilder.CreateString("uuid-1");
            auto status = 1234;
            auto messageInfo = msgBuilder.CreateString(message);

            std::vector<long> vec = {1561,152116,151616,7848};
            auto states = msgBuilder.CreateVector(vec);

            auto msg = CreateMessageFlatbuffers(msgBuilder, id, status, states, messageInfo);
            msgBuilder.Finish(msg);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto timeEncoding = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "encode " << runs << " messages: " << timeEncoding << "ms" << std::endl;

        /*
         * setup message to decode
         */
        flatbuffers::FlatBufferBuilder msgBuilder;
        auto id = msgBuilder.CreateString("uuid-1");
        auto status = 1234;
        auto messageInfo = msgBuilder.CreateString(message);

        std::vector<long> vec = {1561,152116,151616,7848};
        auto states = msgBuilder.CreateVector(vec);

        auto messageFlatbuffers = CreateMessageFlatbuffers(msgBuilder, id, status, states, messageInfo);
        msgBuilder.Finish(messageFlatbuffers);

        zmq_msg_t msg;
        zmq_msg_init_data(&msg, msgBuilder.GetBufferPointer(), msgBuilder.GetSize(), nullptr, msgBuilder.GetBufferPointer());

        start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < runs; i++) {
            auto messageFlatbuffers = GetMessageFlatbuffers(zmq_msg_data(&msg));
            auto id = messageFlatbuffers->id()->c_str();
            long status = messageFlatbuffers->status();
            auto states = messageFlatbuffers->states();
            auto messageInfo = messageFlatbuffers->messageInfo()->c_str();
        }

        end = std::chrono::high_resolution_clock::now();
        auto timeDecoding = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "decode " << runs << " messages: " << timeDecoding << "ms\n" << std::endl;

        std::stringstream ss;
        ss << "\n\t\t\tsize: " << message.size();
        ss << "\n\t\t\truns: " << runs << "\n";
        ss << "\t\t\ttime encoding: " << timeEncoding << "ms" << "\n";
        ss << "\t\t\ttime decoding: " << timeDecoding << "ms" << "\n";
        return ss.str();
    }
}
