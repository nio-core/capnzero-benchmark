using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("capnzero");

@0xd9693c54d5510668;

struct MessageCapnp {
    id @0 :Text;
    status @2 :Int64;
    messageInfo @1 :Text;
    states @3 :List(Int64);
}

struct String {
  string @0 :Text;
}

using IDMsg = import "/capnzero/ID.capnp";

struct AlicaEngineInfo {
    senderId @0 :IDMsg.ID;
    masterPlan @1 :Text;
    currentPlan @2 :Text;
    currentState @3 :Text;
    currentRole @4 :Text;
    currentTask @5 :Text;
    agentIdsWithMe @6 :List(IDMsg.ID);
}
