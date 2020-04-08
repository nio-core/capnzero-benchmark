using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("capnzero");

@0xd9693c54d5510668;

# UUID (64 bit)
# Message Info Message (var)
# Status int(64 bit)
# Liste von Zustandsänderung (int) (var)

struct MessageCapnp {
    id @0 :Text;
    status @2 :Int64;
    messageInfo @1 :Text;
    states @3 :List(Int64);
}
