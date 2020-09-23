#pragma once

#include "Common.h"
#include <capnp/serialize-packed.h>
#include <zmq.h>

#include "capnzero-base-msgs/string.capnp.h"

#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace capnzero
{

/**
 * Each Publisher can only send via one protocol (see Common.h -> capnzero::Protocol), but it is possible to send
 * to multiple addresses via this protocol at once. Furthermore each message is send on a certain topic. For IPC and TCP
 * the topic is set via multi-part messages (see http://wiki.zeromq.org/blog:zero-copy), in case of UDP via multicast
 * groups (see http://api.zeromq.org/master:zmq-socket#toc6 ZMQ_RADIO/ZMQ_DISH socket type).
 */
class PublisherCapnProto
{
public:
    PublisherCapnProto(void* context, Protocol protocol);
    virtual ~PublisherCapnProto();

    void setDefaultTopic(std::string defaultTopic);

    /**
     * Connects or binds the socket of the publisher to the given address.
     * @param address The address.
     * @param bind Flag for using zmq_bind or zmq_connect, default to false.
     */
    void addAddress(std::string address, bool bind = false);

    /**
     * Sends the message to the default topic.
     * @param msgBuilder Builder containing the message.
     * @return Number of bytes sent.
     */
    int send(zmq_msg_t &msg);

    /**
     * Sends the message to the given topic.
     * @param msgBuilder Builder containing the message.
     * @param topic The topic to send the message to.
     * @return Number of bytes sent.
     */
    int send(zmq_msg_t &msg, std::string topic);

    /**
     * Sets the sender high water mark level of the underlying socket of
     * this publisher.
     * @param queueSize
     */
    void setSendQueueSize(int queueSize);

protected:
    void* context;
    void* socket;
    std::string defaultTopic;
    Protocol protocol;
};


} // namespace capnzero
