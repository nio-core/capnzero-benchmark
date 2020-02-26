#include "capnzero/PublisherSBE.h"

#include <assert.h>

namespace capnzero
{

/**
 * Static method that cleans up sent messages. The method is called by ZeroMQ!
 * @param data Pointer to the data that was sent.
 * @param hint
 */
static void cleanUpMsgData(void* data, void* hint);

    PublisherSBE::PublisherSBE(void* context, Protocol protocol)
        : socket(nullptr)
        , protocol(protocol)
        , context(context)
        , defaultTopic("")
{
    // set socket type with respect to given protocol
    switch (this->protocol) {
    case Protocol::UDP:
        this->socket = zmq_socket(this->context, ZMQ_RADIO);
        break;
    case Protocol::TCP:
    case Protocol::IPC:
        this->socket = zmq_socket(this->context, ZMQ_PUB);
        break;
    default:
        // Unknown protocol!
        assert(false && "Publisher::Publisher: The given protocol is unknown!");
    }
}

    PublisherSBE::~PublisherSBE()
{
    check(zmq_close(this->socket), "zmq_close");
}

void PublisherSBE::setDefaultTopic(std::string defaultTopic)
{
    assert(defaultTopic.length() < MAX_TOPIC_LENGTH && "Publisher::setTopic: The given default topic is too long!");
    this->defaultTopic = defaultTopic;
}

void PublisherSBE::addAddress(std::string address)
{
    switch (this->protocol) {
    case Protocol::UDP:
        check(zmq_connect(this->socket, ("udp://" + address).c_str()), "zmq_connect");
        break;
    case Protocol::TCP:
        check(zmq_connect(this->socket, ("tcp://" + address).c_str()), "zmq_connect");
        break;
    case Protocol::IPC:
        check(zmq_connect(this->socket, ("ipc://" + address).c_str()), "zmq_connect");
        break;
    default:
        // Unknown protocol!
        assert(false && "Publisher::Publisher: The given protocol is unknown!");
    }
}

void PublisherSBE::setSendQueueSize(int queueSize)
{
    check( zmq_setsockopt(this->socket, ZMQ_SNDHWM, &queueSize, sizeof(queueSize)), "zmq_setsockopt");
}

int PublisherSBE::send(Message& message, std::string topic)
{
    assert(topic.length() < MAX_TOPIC_LENGTH && "Publisher::send: The given topic is too long!");

    // setup zmq msg
    zmq_msg_t msg;
    int sumBytesSend = 0;

    check(zmq_msg_init_data(&msg, message.buffer(), message.bufferLength(), &cleanUpMsgData, NULL), "zmq_msg_init_data");

    // Topic handling
    if (this->protocol == Protocol::UDP) {
        // for UDP via multicast group
        check(zmq_msg_set_group(&msg, topic.c_str()), "zmq_msg_set_group");
    } else {
        // for NON-UDP via multi part messages
        zmq_msg_t topicMsg;
        check(zmq_msg_init_data(&topicMsg, &topic, topic.size() * sizeof(topic), NULL, NULL), "zmq_msg_init_data for topic");
        sumBytesSend = checkSend(zmq_msg_send(&topicMsg, this->socket, ZMQ_SNDMORE), topicMsg, "Publisher-topic");
        if (sumBytesSend == 0) {
            // sending topic did not work, so stop here
            return sumBytesSend;
        }
    }

    sumBytesSend += checkSend(zmq_msg_send(&msg, this->socket, 0), msg, "Publisher-content");
    return sumBytesSend;
}

int PublisherSBE::send(Message& msg)
{
    return this->send(msg, this->defaultTopic);
}

Message PublisherSBE::createMessage(std::string message) {
    Message msg;
    MessageHeader hdr;
    char buffer[2048];
    int bufferLength = sizeof(buffer);

    hdr.wrap(buffer, 0, 0, bufferLength)
            .blockLength(Message::sbeBlockLength())
            .templateId(Message::sbeTemplateId())
            .schemaId(Message::sbeSchemaId())
            .version(Message::sbeSchemaVersion());
    msg.wrapForEncode(buffer, hdr.encodedLength(), bufferLength);
    msg.putMessageString(message.c_str(), strlen(message.c_str()));

    return msg;
}

static void cleanUpMsgData(void* data, void* hint)
{
//    free(data);
}
} // namespace capnzero
