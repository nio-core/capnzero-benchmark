#include "capnzero/PublisherProtobuf.h"

#include <assert.h>

namespace capnzero
{

/**
 * Static method that cleans up sent messages. The method is called by ZeroMQ!
 * @param data Pointer to the data that was sent.
 * @param hint
 */
static void cleanUpMsgData(void* data, void* hint);

PublisherProtobuf::PublisherProtobuf(void* context, Protocol protocol)
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

PublisherProtobuf::~PublisherProtobuf()
{
    check(zmq_close(this->socket), "zmq_close");
}

void PublisherProtobuf::setDefaultTopic(std::string defaultTopic)
{
    assert(defaultTopic.length() < MAX_TOPIC_LENGTH && "Publisher::setTopic: The given default topic is too long!");
    this->defaultTopic = defaultTopic;
}

void PublisherProtobuf::addAddress(std::string address)
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

void PublisherProtobuf::setSendQueueSize(int queueSize)
{
    check( zmq_setsockopt(this->socket, ZMQ_SNDHWM, &queueSize, sizeof(queueSize)), "zmq_setsockopt");
}

int PublisherProtobuf::send(String& string, std::string topic)
{
    assert(topic.length() < MAX_TOPIC_LENGTH && "Publisher::send: The given topic is too long!");

    // setup zmq msg
    zmq_msg_t msg;
    int sumBytesSend = 0;
    void* msg_ser = malloc(string.ByteSizeLong());
    string.SerializeToArray(msg_ser, string.ByteSizeLong());

    check(zmq_msg_init_data(&msg, msg_ser, string.ByteSizeLong(), &cleanUpMsgData, NULL), "zmq_msg_init_data");


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

int PublisherProtobuf::send(String& string)
{
    return this->send(string, this->defaultTopic);
}

capnzero::String PublisherProtobuf::createMessage(std::string message) {
    String string;
    string.set_string(message);
    return string;
}

static void cleanUpMsgData(void* data, void* hint)
{
    free(data);
}
} // namespace capnzero
