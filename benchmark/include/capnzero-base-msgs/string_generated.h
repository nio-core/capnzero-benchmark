// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_STRING_CAPNZERO_H_
#define FLATBUFFERS_GENERATED_STRING_CAPNZERO_H_

#include "flatbuffers/flatbuffers.h"

namespace capnzero {

struct MessageFlatbuffers;
struct MessageFlatbuffersBuilder;

inline const flatbuffers::TypeTable *MessageFlatbuffersTypeTable();

struct MessageFlatbuffers FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef MessageFlatbuffersBuilder Builder;
  static const flatbuffers::TypeTable *MiniReflectTypeTable() {
    return MessageFlatbuffersTypeTable();
  }
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_STATUS = 6,
    VT_STATES = 8,
    VT_MESSAGEINFO = 10
  };
  const flatbuffers::String *id() const {
    return GetPointer<const flatbuffers::String *>(VT_ID);
  }
  int64_t status() const {
    return GetField<int64_t>(VT_STATUS, 0);
  }
  const flatbuffers::Vector<int64_t> *states() const {
    return GetPointer<const flatbuffers::Vector<int64_t> *>(VT_STATES);
  }
  const flatbuffers::String *messageInfo() const {
    return GetPointer<const flatbuffers::String *>(VT_MESSAGEINFO);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_ID) &&
           verifier.VerifyString(id()) &&
           VerifyField<int64_t>(verifier, VT_STATUS) &&
           VerifyOffset(verifier, VT_STATES) &&
           verifier.VerifyVector(states()) &&
           VerifyOffset(verifier, VT_MESSAGEINFO) &&
           verifier.VerifyString(messageInfo()) &&
           verifier.EndTable();
  }
};

struct MessageFlatbuffersBuilder {
  typedef MessageFlatbuffers Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(flatbuffers::Offset<flatbuffers::String> id) {
    fbb_.AddOffset(MessageFlatbuffers::VT_ID, id);
  }
  void add_status(int64_t status) {
    fbb_.AddElement<int64_t>(MessageFlatbuffers::VT_STATUS, status, 0);
  }
  void add_states(flatbuffers::Offset<flatbuffers::Vector<int64_t>> states) {
    fbb_.AddOffset(MessageFlatbuffers::VT_STATES, states);
  }
  void add_messageInfo(flatbuffers::Offset<flatbuffers::String> messageInfo) {
    fbb_.AddOffset(MessageFlatbuffers::VT_MESSAGEINFO, messageInfo);
  }
  explicit MessageFlatbuffersBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  MessageFlatbuffersBuilder &operator=(const MessageFlatbuffersBuilder &);
  flatbuffers::Offset<MessageFlatbuffers> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<MessageFlatbuffers>(end);
    return o;
  }
};

inline flatbuffers::Offset<MessageFlatbuffers> CreateMessageFlatbuffers(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> id = 0,
    int64_t status = 0,
    flatbuffers::Offset<flatbuffers::Vector<int64_t>> states = 0,
    flatbuffers::Offset<flatbuffers::String> messageInfo = 0) {
  MessageFlatbuffersBuilder builder_(_fbb);
  builder_.add_status(status);
  builder_.add_messageInfo(messageInfo);
  builder_.add_states(states);
  builder_.add_id(id);
  return builder_.Finish();
}

inline flatbuffers::Offset<MessageFlatbuffers> CreateMessageFlatbuffersDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *id = nullptr,
    int64_t status = 0,
    const std::vector<int64_t> *states = nullptr,
    const char *messageInfo = nullptr) {
  auto id__ = id ? _fbb.CreateString(id) : 0;
  auto states__ = states ? _fbb.CreateVector<int64_t>(*states) : 0;
  auto messageInfo__ = messageInfo ? _fbb.CreateString(messageInfo) : 0;
  return capnzero::CreateMessageFlatbuffers(
      _fbb,
      id__,
      status,
      states__,
      messageInfo__);
}

inline const flatbuffers::TypeTable *MessageFlatbuffersTypeTable() {
  static const flatbuffers::TypeCode type_codes[] = {
    { flatbuffers::ET_STRING, 0, -1 },
    { flatbuffers::ET_LONG, 0, -1 },
    { flatbuffers::ET_LONG, 1, -1 },
    { flatbuffers::ET_STRING, 0, -1 }
  };
  static const char * const names[] = {
    "id",
    "status",
    "states",
    "messageInfo"
  };
  static const flatbuffers::TypeTable tt = {
    flatbuffers::ST_TABLE, 4, type_codes, nullptr, nullptr, names
  };
  return &tt;
}

inline const capnzero::MessageFlatbuffers *GetMessageFlatbuffers(const void *buf) {
  return flatbuffers::GetRoot<capnzero::MessageFlatbuffers>(buf);
}

inline const capnzero::MessageFlatbuffers *GetSizePrefixedMessageFlatbuffers(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<capnzero::MessageFlatbuffers>(buf);
}

inline bool VerifyMessageFlatbuffersBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<capnzero::MessageFlatbuffers>(nullptr);
}

inline bool VerifySizePrefixedMessageFlatbuffersBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<capnzero::MessageFlatbuffers>(nullptr);
}

inline void FinishMessageFlatbuffersBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<capnzero::MessageFlatbuffers> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedMessageFlatbuffersBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<capnzero::MessageFlatbuffers> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace capnzero

#endif  // FLATBUFFERS_GENERATED_STRING_CAPNZERO_H_
