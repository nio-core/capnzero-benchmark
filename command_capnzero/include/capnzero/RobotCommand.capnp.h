// Generated by Cap'n Proto compiler, DO NOT EDIT
// source: RobotCommand.capnp

#ifndef CAPNP_INCLUDED_c0f7398d39bb2416_
#define CAPNP_INCLUDED_c0f7398d39bb2416_

#include <capnp/generated-header-support.h>

#if CAPNP_VERSION != 6001
#error "Version mismatch between generated code and library headers.  You must use the same version of the Cap'n Proto compiler and library."
#endif

#include <capnzero/ID.capnp.h>

namespace capnp {
namespace schemas {

CAPNP_DECLARE_SCHEMA(f8452cdd571622ef);
CAPNP_DECLARE_SCHEMA(8d7a3458f3308359);
enum class Cmd_8d7a3458f3308359: uint16_t {
  START,
  STOP,
};
CAPNP_DECLARE_ENUM(Cmd, 8d7a3458f3308359);

}  // namespace schemas
}  // namespace capnp

namespace capnzero {

struct RobotCommand {
  RobotCommand() = delete;

  class Reader;
  class Builder;
  class Pipeline;
  typedef ::capnp::schemas::Cmd_8d7a3458f3308359 Cmd;


  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(f8452cdd571622ef, 1, 1)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

// =======================================================================================

class RobotCommand::Reader {
public:
  typedef RobotCommand Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

  inline bool hasReceiverID() const;
  inline  ::capnzero::ID::Reader getReceiverID() const;

  inline  ::capnzero::RobotCommand::Cmd getCommand() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class RobotCommand::Builder {
public:
  typedef RobotCommand Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline bool hasReceiverID();
  inline  ::capnzero::ID::Builder getReceiverID();
  inline void setReceiverID( ::capnzero::ID::Reader value);
  inline  ::capnzero::ID::Builder initReceiverID();
  inline void adoptReceiverID(::capnp::Orphan< ::capnzero::ID>&& value);
  inline ::capnp::Orphan< ::capnzero::ID> disownReceiverID();

  inline  ::capnzero::RobotCommand::Cmd getCommand();
  inline void setCommand( ::capnzero::RobotCommand::Cmd value);

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class RobotCommand::Pipeline {
public:
  typedef RobotCommand Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

  inline  ::capnzero::ID::Pipeline getReceiverID();
private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

// =======================================================================================

inline bool RobotCommand::Reader::hasReceiverID() const {
  return !_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline bool RobotCommand::Builder::hasReceiverID() {
  return !_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline  ::capnzero::ID::Reader RobotCommand::Reader::getReceiverID() const {
  return ::capnp::_::PointerHelpers< ::capnzero::ID>::get(_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline  ::capnzero::ID::Builder RobotCommand::Builder::getReceiverID() {
  return ::capnp::_::PointerHelpers< ::capnzero::ID>::get(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
#if !CAPNP_LITE
inline  ::capnzero::ID::Pipeline RobotCommand::Pipeline::getReceiverID() {
  return  ::capnzero::ID::Pipeline(_typeless.getPointerField(0));
}
#endif  // !CAPNP_LITE
inline void RobotCommand::Builder::setReceiverID( ::capnzero::ID::Reader value) {
  ::capnp::_::PointerHelpers< ::capnzero::ID>::set(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), value);
}
inline  ::capnzero::ID::Builder RobotCommand::Builder::initReceiverID() {
  return ::capnp::_::PointerHelpers< ::capnzero::ID>::init(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline void RobotCommand::Builder::adoptReceiverID(
    ::capnp::Orphan< ::capnzero::ID>&& value) {
  ::capnp::_::PointerHelpers< ::capnzero::ID>::adopt(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnzero::ID> RobotCommand::Builder::disownReceiverID() {
  return ::capnp::_::PointerHelpers< ::capnzero::ID>::disown(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}

inline  ::capnzero::RobotCommand::Cmd RobotCommand::Reader::getCommand() const {
  return _reader.getDataField< ::capnzero::RobotCommand::Cmd>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}

inline  ::capnzero::RobotCommand::Cmd RobotCommand::Builder::getCommand() {
  return _builder.getDataField< ::capnzero::RobotCommand::Cmd>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}
inline void RobotCommand::Builder::setCommand( ::capnzero::RobotCommand::Cmd value) {
  _builder.setDataField< ::capnzero::RobotCommand::Cmd>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, value);
}

}  // namespace

#endif  // CAPNP_INCLUDED_c0f7398d39bb2416_