#include "../../include/benchmark/BenchmarkMsgPack.h"

namespace capnzero {

    std::chrono::high_resolution_clock::time_point endMsgPack;
    bool hasReceivedMsgPack = false;
    int messagesReceivedMsgPack = 0;

    void callbackMsgPack(capnzero::Message msg)
    {
        auto id = msg.id;
        long status = msg.status;
        auto states = msg.states;
        auto messageInfo = msg.messageInfo;
        endMsgPack = std::chrono::high_resolution_clock::now();
        hasReceivedMsgPack = true;
        messagesReceivedMsgPack++;
    }

    PublisherMsgPack BenchmarkMsgPack::createPublisher() {
        void *ctx = zmq_ctx_new();
        PublisherMsgPack pub = capnzero::PublisherMsgPack(ctx, capnzero::Protocol::UDP);
        pub.setDefaultTopic("benchmark");
        pub.addAddress("224.0.0.2:5555");
        return pub;
    }

    SubscriberMsgPack* BenchmarkMsgPack::createSubscriber() {
        void* ctx = zmq_ctx_new();
        capnzero::SubscriberMsgPack* sub = new capnzero::SubscriberMsgPack(ctx, capnzero::Protocol::UDP);

        sub->setTopic("benchmark");
        sub->addAddress("224.0.0.2:5555");
        sub->subscribe(&callbackMsgPack);

        return sub;
    }

    json BenchmarkMsgPack::messageSizeBenchmark(std::string message) {
        //Setup
        int messageSizeInBytes = 0;
        int sendBytes = 0;
        int retries = 0;
        std::chrono::high_resolution_clock::time_point start;
        hasReceivedMsgPack = false;

        PublisherMsgPack pub = createPublisher();

        SubscriberMsgPack* sub = createSubscriber();

        capnzero::Message msgObject;
        msgObject.id = "uuid-1";
        msgObject.status = 1234;
        msgObject.messageInfo = message;
        msgObject.states = {1561,152116,151616,7848};

        //running
        std::cout << "running msgpack benchmark with size " << message.size() << std::endl;

        do {
            start = std::chrono::high_resolution_clock::now();

            msgpack::sbuffer sbuf;
            msgpack::pack(sbuf, message);

            messageSizeInBytes = pub.send(sbuf);

            std::this_thread::sleep_for(std::chrono::seconds(1));
        } while (!hasReceivedMsgPack);

        auto time = std::chrono::duration_cast<std::chrono::microseconds>(endMsgPack - start).count();
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

    json BenchmarkMsgPack::maxMessageRateBenchmark(std::string message, int runs, long nsBetweenMessages) {
        int messagesSend = 0;
        int messageSize = 0;

        std::cout << "max message rate msgpack benchmark with " << nsBetweenMessages << "ns and " << runs << " runs" << std::endl;
        PublisherMsgPack pub = createPublisher();
        SubscriberMsgPack* sub = createSubscriber();

        messagesReceivedMsgPack = 0;

        Message msgObject;
        msgObject.id = "uuid-1";
        msgObject.status = 1234;
        msgObject.messageInfo = message;
        msgObject.states = {1561,152116,151616,7848};

        while (runs > messagesSend) {
            msgpack::sbuffer sbuf;
            msgpack::pack(sbuf, msgObject);
            messageSize = pub.send(sbuf);
            messagesSend++;
            std::this_thread::sleep_for(std::chrono::nanoseconds(nsBetweenMessages));
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "messages send: " << messagesSend << std::endl;
        std::cout << "messages received: " << messagesReceivedMsgPack << std::endl;
        delete sub;

        json result;

        result["ns_between_messages"] = nsBetweenMessages;
        result["size_in_bytes"] = messageSize;
        result["messages_send"] = messagesSend;
        result["messages_received"] = messagesReceivedMsgPack;

        return result;
    }

    json BenchmarkMsgPack::encodeDecodeBenchmark(std::string message, int runs) {
        std::cout << "encode decode benchmark msgpack with " << runs << " encodes / decodes and size " << message.size() << std::endl;

        Message msgObject;
        msgObject.id = "uuid-1";
        msgObject.status = 1234;
        msgObject.messageInfo = message;
        msgObject.states = {1561,152116,151616,7848};

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < runs; i++) {
            msgpack::sbuffer sbuf;
            msgpack::pack(sbuf, msgObject);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto timeEncoding = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "encode " << runs << " messages: " << timeEncoding << "ms" << std::endl;

        /*
         * setup message to decode
         */
        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, msgObject);

        zmq_msg_t msg;
        zmq_msg_init_data(&msg, sbuf.data(), sbuf.size(), nullptr, sbuf.data());

        start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < runs; i++) {
            msgpack::object_handle oh = msgpack::unpack(static_cast<const char *>(zmq_msg_data(&msg)), zmq_msg_size(&msg));
            msgpack::object obj = oh.get();
            Message unpackedMessage;
            obj.convert(unpackedMessage);
        }

        endMsgPack = std::chrono::high_resolution_clock::now();
        auto timeDecoding = std::chrono::duration_cast<std::chrono::milliseconds>(endMsgPack - start).count();

        std::cout << "decode " << runs << " messages: " << timeDecoding << "ms\n" << std::endl;

        json result;
        result["size"] = message.size();
        result["runs"] = runs;
        result["time_encoding_ms"] = timeEncoding;
        result["time_decoding_ms"] = timeDecoding;
        return result;
    }
}
