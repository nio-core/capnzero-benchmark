#pragma once

#include "capnzero/Common.h"

#include <capnp/serialize-packed.h>
#include "capnzero-base-msgs/string.capnp.h"

#include <zmq.h>

#include <string>

#include <assert.h>
#include <functional>
#include <thread>
#include <unistd.h>
#include <vector>

//#define DEBUG_SUBSCRIBER

namespace capnzero
{
class SubscriberCapnProto
{
public:
    typedef std::function<void(::capnp::FlatArrayMessageReader&)> callbackFunction;
    static const int WORD_SIZE;

    SubscriberCapnProto(void* context, Protocol protocol);
    virtual ~SubscriberCapnProto();

    /**
     * Starts the receiving thread, if called for the first time. Changes the callback to the given function and object.
     * @tparam CallbackObjType
     * @param callbackFunction
     * @param callbackObject
     */
    template <class CallbackObjType>
    void subscribe(void (CallbackObjType::*callbackFunction)(::capnp::FlatArrayMessageReader&), CallbackObjType* callbackObject) {
        using std::placeholders::_1;
        this->callbackFunction_ = std::bind(callbackFunction, callbackObject, _1);
        if (!running) {
            this->running = true;
            this->runThread = new std::thread(&SubscriberCapnProto::receive, this);
        }
    }

    /**
     * Starts the receiving thread, if called for the first time. Changes the callback to the given function.
     * @param callbackFunction
     */
    void subscribe(void (*callbackFunction)(capnp::FlatArrayMessageReader&));

    /**
     * Sets the topic to receive from.
     * @param defaultTopic
     */
    void setTopic(std::string topic);

    /**
     * Connects or binds the socket of the subscriber to the given address.
     * @param address The address.
     * * @param bind Flag for using zmq_bind or zmq_connect, default to true.
     */
    void addAddress(std::string address, bool bind = true);

    /**
     * Sets the receiver high water mark level of the underlying socket of
     * this subscriber.
     * @param queueSize
     */
    void setReceiveQueueSize(int queueSize);

protected:
    void* context;
    void* socket;
    std::string topic;
    Protocol protocol;
    int rcvTimeout; /** < only initialized if needed */

    callbackFunction callbackFunction_;

    std::thread* runThread;
    bool running;

    void receive();
};

} // namespace capnzero
