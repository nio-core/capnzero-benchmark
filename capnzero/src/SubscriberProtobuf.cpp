#include "capnzero/SubscriberProtobuf.h"
//#define DEBUG_SUBSCRIBER

namespace capnzero
{

SubscriberProtobuf::SubscriberProtobuf(void* context, Protocol protocol)
        : socket(nullptr)
        , rcvTimeout(500)
        , topic("???") // this filter topic will hopefully never be used
        , context(context)
        , protocol(protocol)
        , running(false)
        , runThread(nullptr)
        , callbackFunction_(nullptr)
{
    // set socket type with respect to given protocol
    switch (this->protocol) {
        case Protocol::UDP:
            this->socket = zmq_socket(this->context, ZMQ_DISH);
            break;
        case Protocol::TCP:
        case Protocol::IPC:
            this->socket = zmq_socket(this->context, ZMQ_SUB);
            break;
        default:
            // Unknown protocol!
            assert(false && "Subscriber::Subscriber: The given protocol is unknown!");
    }

    check(zmq_setsockopt(this->socket, ZMQ_RCVTIMEO, &rcvTimeout, sizeof(rcvTimeout)), "zmq_setsockopt");
}

SubscriberProtobuf::~SubscriberProtobuf()
{
    this->running = false;
    this->runThread->join();
    delete this->runThread;
    check(zmq_close(this->socket), "zmq_close");
}

void SubscriberProtobuf::setTopic(std::string topic)
{
    assert(topic.length() < MAX_TOPIC_LENGTH && "Subscriber::setTopic: The given topic is too long!");

    if (this->topic.compare(topic) == 0) {
        return;
    }

    switch (this->protocol) {
        case Protocol::UDP:
            this->topic = topic;
            check(zmq_join(this->socket, this->topic.c_str()), "zmq_join");
            break;
        case Protocol::TCP:
        case Protocol::IPC:
            if (this->topic.compare("???") != 0) {
                check(zmq_setsockopt(this->socket, ZMQ_UNSUBSCRIBE, this->topic.c_str(), 0), "zmq_setsockopt");
            }
            this->topic = topic;
            check(zmq_setsockopt(this->socket, ZMQ_SUBSCRIBE, this->topic.c_str(), 0), "zmq_setsockopt");
            break;
        default:
            // Unknown protocol!
            assert(false && "Subscriber::setTopic: The given protocol is unknown!");
    }
}

void SubscriberProtobuf::addAddress(std::string address)
{
    switch (protocol) {
    case Protocol::UDP:
        check(zmq_bind(this->socket, ("udp://" + address).c_str()), "zmq_bind");
        break;
    case Protocol::TCP:
        check(zmq_bind(this->socket, ("tcp://" + address).c_str()), "zmq_bind");
        break;
    case Protocol::IPC:
        check(zmq_bind(this->socket, ("ipc://" + address).c_str()), "zmq_bind");
        break;
    default:
        // Unknown protocol!
        assert(false && "Subscriber::addAddress: Given protocol is unknown!");
    }
}

void SubscriberProtobuf::subscribe(void (*callbackFunction)(MessageProtobuf* msg)) {
    this->callbackFunction_ = callbackFunction;
    if (!running) {
        this->running = true;
        this->runThread = new std::thread(&SubscriberProtobuf::receive, this);
    }
}

void SubscriberProtobuf::setReceiveQueueSize(int queueSize)
{
    check( zmq_setsockopt(this->socket, ZMQ_RCVHWM, &queueSize, sizeof(queueSize)), "zmq_setsockopt");
}

void SubscriberProtobuf::receive()
{
    while (this->running) {

#ifdef DEBUG_SUBSCRIBER
        std::cout << "Subscriber::received() waiting ..." << std::endl;
#endif

        if (this->protocol != Protocol::UDP) {
            // Topic handling for non-udp protocols via multipart message
            zmq_msg_t topicMsg;
            check(zmq_msg_init(&topicMsg), "zmq_msg_init");
            if (0 == checkReceive(zmq_msg_recv(&topicMsg, this->socket, ZMQ_SNDMORE), topicMsg, "Subscriber::receive-Topic")) {
                // error or timeout on recv
//                std::cout << "Subscriber: No Message here!" << std::endl;
                continue;
            }
        }

        zmq_msg_t msg;
        check(zmq_msg_init(&msg), "zmq_msg_init");
        if (0 == checkReceive(zmq_msg_recv(&msg, this->socket, 0), msg, "Subscriber::receive")) {
            // error or timeout on recv
//            std::cout << "Subscriber: No Message here!" << std::endl;
            continue;
        }

#ifdef DEBUG_SUBSCRIBER
        std::cout << std::endl;
#endif

        void* data = zmq_msg_data(&msg);
        size_t len = zmq_msg_size(&msg);
        MessageProtobuf messageProtobuf;
        messageProtobuf.ParseFromArray(data, len);
        (this->callbackFunction_)(&messageProtobuf);

        check(zmq_msg_close(&msg), "zmq_msg_close");
    }
}

} // namespace capnzero