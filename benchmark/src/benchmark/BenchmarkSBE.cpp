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

    json BenchmarkSBE::messageSizeBenchmark(std::string message) {
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

        json result;
        result["size_in_bytes"] = messageSizeInBytes;
        result["time_in_us"] = time;
        result["retries"] = retries;
        result["time_adjusted"] = (time - (1000000 * retries));

        return result;
    }

    json BenchmarkSBE::maxMessageRateBenchmark(std::string message, int runs, long nsBetweenMessages) {
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

        json result;
        result["ns_between_messages"] = nsBetweenMessages;
        result["size_in_bytes"] = messageSize;
        result["messages_send"] = messagesSend;
        result["messages_received"] = messagesReceivedSBE;

        return result;
    }

    json BenchmarkSBE::encodeDecodeBenchmark(std::string message, int runs) {
        std::cout << "encode decode benchmark sbe with " << runs << " encodes / decodes and size " << message.size() << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < runs; i++) {
            /*
             * Encode message
             */
            sbe::MessageSBE msg;
            sbe::MessageHeader hdr;
            int size = 21 +
                       + sbe::MessageSBE::States::stateEncodingLength() * 2
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
                    .statesCount(2)
                    .next().state(9070150)
                    .next().state(5432);
            msg.putMessageInfo(message.c_str(), strlen(message.c_str()));

        }

        auto end = std::chrono::high_resolution_clock::now();
        auto timeEncoding = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "encode " << runs << " messages: " << timeEncoding << "ms" << std::endl;

//        /*
//         * setup message to decode
//         */
//        sbe::MessageSBE msgSBE;
//        sbe::MessageHeader hdr;
//        int size = 21 +
//                   + sbe::MessageSBE::States::stateEncodingLength() * 2
//                   + message.size()
//                   + sbe::MessageSBE::messageInfoHeaderLength();
//        char buffer[size];
//        std::cout << "size: " << size << std::endl;
//        int bufferLength = sizeof(buffer);
//
//        hdr.wrap(buffer, 0, 0, bufferLength)
//                .blockLength(sbe::MessageSBE::sbeBlockLength())
//                .templateId(sbe::MessageSBE::sbeTemplateId())
//                .schemaId(sbe::MessageSBE::sbeSchemaId())
//                .version(sbe::MessageSBE::sbeSchemaVersion());
//        msgSBE.wrapForEncode(buffer, hdr.encodedLength(), bufferLength);
//
//        msgSBE.id('u')
//                .status(123156)
//                .statesCount(2)
//                .next().state(9070150)
//                .next().state(5432);
//        msgSBE.putMessageInfo(message.c_str(), strlen(message.c_str()));
//
//        zmq_msg_t msg;
//        zmq_msg_init_data(&msg, msgSBE.buffer(), msgSBE.bufferLength(), nullptr, NULL);

        auto startDecode = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < runs; i++) {
            /*
         * setup message to decode
         */
            int size = 21 +
                       + sbe::MessageSBE::States::stateEncodingLength() * 2
                       + message.size()
                       + sbe::MessageSBE::messageInfoHeaderLength();
            sbe::MessageSBE msgSBE;
            sbe::MessageHeader hdrEncode;
            char buffer[size];
            int bufferLength = sizeof(buffer);

            hdrEncode.wrap(buffer, 0, 0, bufferLength)
                    .blockLength(sbe::MessageSBE::sbeBlockLength())
                    .templateId(sbe::MessageSBE::sbeTemplateId())
                    .schemaId(sbe::MessageSBE::sbeSchemaId())
                    .version(sbe::MessageSBE::sbeSchemaVersion());
            msgSBE.wrapForEncode(buffer, hdrEncode.encodedLength(), bufferLength);

            msgSBE.id('u')
                    .status(123156)
                    .statesCount(2)
                    .next().state(9070150)
                    .next().state(5432);
            msgSBE.putMessageInfo(message.c_str(), strlen(message.c_str()));

            zmq_msg_t msg;
            zmq_msg_init_data(&msg, msgSBE.buffer(), msgSBE.bufferLength(), nullptr, NULL);

            /*
             * Decode message
             */
            char* data = (char*) zmq_msg_data(&msg);
            sbe::MessageHeader hdr;
            sbe::MessageSBE message;

            message.wrapAndApplyHeader(data, 0, zmq_msg_size(&msg));
            char id = msgSBE.id();
            long int status = message.status();
            sbe::MessageSBE::States &states = message.states();
            while (states.hasNext()) {
                states.next();
            }
            std::string messageInfo = message.getMessageInfoAsString();
        }

        auto endDecode = std::chrono::high_resolution_clock::now();
        auto timeDecoding = std::chrono::duration_cast<std::chrono::milliseconds>(endDecode - start).count();

        std::cout << "decode " << runs << " messages: " << timeDecoding - timeEncoding << "ms\n" << std::endl;

        json result;
        result["size"] = message.size();
        result["runs"] = runs;
        result["time_encoding_ms"] = timeEncoding;
        result["time_decoding_ms"] = timeDecoding;
        return result;
    }
}
