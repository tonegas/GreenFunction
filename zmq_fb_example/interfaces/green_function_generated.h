// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_GREENFUNCTION_GREENFUNCTION_H_
#define FLATBUFFERS_GENERATED_GREENFUNCTION_GREENFUNCTION_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 23 &&
              FLATBUFFERS_VERSION_MINOR == 5 &&
              FLATBUFFERS_VERSION_REVISION == 26,
             "Non-compatible flatbuffers version included");

namespace GreenFunction {

struct CoDriverStruct;

struct GreenStruct;

struct ICoDriver;
struct ICoDriverBuilder;
struct ICoDriverT;

struct IGreen;
struct IGreenBuilder;
struct IGreenT;

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(8) CoDriverStruct FLATBUFFERS_FINAL_CLASS {
 private:
  double double_array_[3];

 public:
  CoDriverStruct()
      : double_array_() {
  }
  CoDriverStruct(::flatbuffers::span<const double, 3> _double_array) {
    ::flatbuffers::CastToArray(double_array_).CopyFromSpan(_double_array);
  }
  const ::flatbuffers::Array<double, 3> *double_array() const {
    return &::flatbuffers::CastToArray(double_array_);
  }
};
FLATBUFFERS_STRUCT_END(CoDriverStruct, 24);

/// I am a comment that will be included in the generated code
FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(8) GreenStruct FLATBUFFERS_FINAL_CLASS {
 private:
  double double_array_[3];

 public:
  GreenStruct()
      : double_array_() {
  }
  GreenStruct(::flatbuffers::span<const double, 3> _double_array) {
    ::flatbuffers::CastToArray(double_array_).CopyFromSpan(_double_array);
  }
  const ::flatbuffers::Array<double, 3> *double_array() const {
    return &::flatbuffers::CastToArray(double_array_);
  }
};
FLATBUFFERS_STRUCT_END(GreenStruct, 24);

struct ICoDriverT : public ::flatbuffers::NativeTable {
  typedef ICoDriver TableType;
  uint64_t id = 0;
  std::unique_ptr<GreenFunction::CoDriverStruct> data{};
  ICoDriverT() = default;
  ICoDriverT(const ICoDriverT &o);
  ICoDriverT(ICoDriverT&&) FLATBUFFERS_NOEXCEPT = default;
  ICoDriverT &operator=(ICoDriverT o) FLATBUFFERS_NOEXCEPT;
};

struct ICoDriver FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef ICoDriverT NativeTableType;
  typedef ICoDriverBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_DATA = 6
  };
  uint64_t id() const {
    return GetField<uint64_t>(VT_ID, 0);
  }
  const GreenFunction::CoDriverStruct *data() const {
    return GetStruct<const GreenFunction::CoDriverStruct *>(VT_DATA);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint64_t>(verifier, VT_ID, 8) &&
           VerifyField<GreenFunction::CoDriverStruct>(verifier, VT_DATA, 8) &&
           verifier.EndTable();
  }
  ICoDriverT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(ICoDriverT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<ICoDriver> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const ICoDriverT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct ICoDriverBuilder {
  typedef ICoDriver Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_id(uint64_t id) {
    fbb_.AddElement<uint64_t>(ICoDriver::VT_ID, id, 0);
  }
  void add_data(const GreenFunction::CoDriverStruct *data) {
    fbb_.AddStruct(ICoDriver::VT_DATA, data);
  }
  explicit ICoDriverBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<ICoDriver> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<ICoDriver>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<ICoDriver> CreateICoDriver(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    uint64_t id = 0,
    const GreenFunction::CoDriverStruct *data = nullptr) {
  ICoDriverBuilder builder_(_fbb);
  builder_.add_id(id);
  builder_.add_data(data);
  return builder_.Finish();
}

::flatbuffers::Offset<ICoDriver> CreateICoDriver(::flatbuffers::FlatBufferBuilder &_fbb, const ICoDriverT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct IGreenT : public ::flatbuffers::NativeTable {
  typedef IGreen TableType;
  uint64_t id = 0;
  std::unique_ptr<GreenFunction::GreenStruct> data{};
  IGreenT() = default;
  IGreenT(const IGreenT &o);
  IGreenT(IGreenT&&) FLATBUFFERS_NOEXCEPT = default;
  IGreenT &operator=(IGreenT o) FLATBUFFERS_NOEXCEPT;
};

struct IGreen FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef IGreenT NativeTableType;
  typedef IGreenBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_DATA = 6
  };
  uint64_t id() const {
    return GetField<uint64_t>(VT_ID, 0);
  }
  const GreenFunction::GreenStruct *data() const {
    return GetStruct<const GreenFunction::GreenStruct *>(VT_DATA);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint64_t>(verifier, VT_ID, 8) &&
           VerifyField<GreenFunction::GreenStruct>(verifier, VT_DATA, 8) &&
           verifier.EndTable();
  }
  IGreenT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(IGreenT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<IGreen> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const IGreenT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct IGreenBuilder {
  typedef IGreen Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_id(uint64_t id) {
    fbb_.AddElement<uint64_t>(IGreen::VT_ID, id, 0);
  }
  void add_data(const GreenFunction::GreenStruct *data) {
    fbb_.AddStruct(IGreen::VT_DATA, data);
  }
  explicit IGreenBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<IGreen> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<IGreen>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<IGreen> CreateIGreen(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    uint64_t id = 0,
    const GreenFunction::GreenStruct *data = nullptr) {
  IGreenBuilder builder_(_fbb);
  builder_.add_id(id);
  builder_.add_data(data);
  return builder_.Finish();
}

::flatbuffers::Offset<IGreen> CreateIGreen(::flatbuffers::FlatBufferBuilder &_fbb, const IGreenT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

inline ICoDriverT::ICoDriverT(const ICoDriverT &o)
      : id(o.id),
        data((o.data) ? new GreenFunction::CoDriverStruct(*o.data) : nullptr) {
}

inline ICoDriverT &ICoDriverT::operator=(ICoDriverT o) FLATBUFFERS_NOEXCEPT {
  std::swap(id, o.id);
  std::swap(data, o.data);
  return *this;
}

inline ICoDriverT *ICoDriver::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<ICoDriverT>(new ICoDriverT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void ICoDriver::UnPackTo(ICoDriverT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = id(); _o->id = _e; }
  { auto _e = data(); if (_e) _o->data = std::unique_ptr<GreenFunction::CoDriverStruct>(new GreenFunction::CoDriverStruct(*_e)); }
}

inline ::flatbuffers::Offset<ICoDriver> ICoDriver::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const ICoDriverT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return CreateICoDriver(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<ICoDriver> CreateICoDriver(::flatbuffers::FlatBufferBuilder &_fbb, const ICoDriverT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const ICoDriverT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _id = _o->id;
  auto _data = _o->data ? _o->data.get() : nullptr;
  return GreenFunction::CreateICoDriver(
      _fbb,
      _id,
      _data);
}

inline IGreenT::IGreenT(const IGreenT &o)
      : id(o.id),
        data((o.data) ? new GreenFunction::GreenStruct(*o.data) : nullptr) {
}

inline IGreenT &IGreenT::operator=(IGreenT o) FLATBUFFERS_NOEXCEPT {
  std::swap(id, o.id);
  std::swap(data, o.data);
  return *this;
}

inline IGreenT *IGreen::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<IGreenT>(new IGreenT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void IGreen::UnPackTo(IGreenT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = id(); _o->id = _e; }
  { auto _e = data(); if (_e) _o->data = std::unique_ptr<GreenFunction::GreenStruct>(new GreenFunction::GreenStruct(*_e)); }
}

inline ::flatbuffers::Offset<IGreen> IGreen::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const IGreenT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return CreateIGreen(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<IGreen> CreateIGreen(::flatbuffers::FlatBufferBuilder &_fbb, const IGreenT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const IGreenT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _id = _o->id;
  auto _data = _o->data ? _o->data.get() : nullptr;
  return GreenFunction::CreateIGreen(
      _fbb,
      _id,
      _data);
}

inline const GreenFunction::IGreen *GetIGreen(const void *buf) {
  return ::flatbuffers::GetRoot<GreenFunction::IGreen>(buf);
}

inline const GreenFunction::IGreen *GetSizePrefixedIGreen(const void *buf) {
  return ::flatbuffers::GetSizePrefixedRoot<GreenFunction::IGreen>(buf);
}

inline bool VerifyIGreenBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<GreenFunction::IGreen>(nullptr);
}

inline bool VerifySizePrefixedIGreenBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<GreenFunction::IGreen>(nullptr);
}

inline void FinishIGreenBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<GreenFunction::IGreen> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedIGreenBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<GreenFunction::IGreen> root) {
  fbb.FinishSizePrefixed(root);
}

inline std::unique_ptr<GreenFunction::IGreenT> UnPackIGreen(
    const void *buf,
    const ::flatbuffers::resolver_function_t *res = nullptr) {
  return std::unique_ptr<GreenFunction::IGreenT>(GetIGreen(buf)->UnPack(res));
}

inline std::unique_ptr<GreenFunction::IGreenT> UnPackSizePrefixedIGreen(
    const void *buf,
    const ::flatbuffers::resolver_function_t *res = nullptr) {
  return std::unique_ptr<GreenFunction::IGreenT>(GetSizePrefixedIGreen(buf)->UnPack(res));
}

}  // namespace GreenFunction

#endif  // FLATBUFFERS_GENERATED_GREENFUNCTION_GREENFUNCTION_H_
