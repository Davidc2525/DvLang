// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: objFile.proto

#ifndef PROTOBUF_objFile_2eproto__INCLUDED
#define PROTOBUF_objFile_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3003000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3003000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
namespace DvObjecFile {
class Code;
class CodeDefaultTypeInternal;
extern CodeDefaultTypeInternal _Code_default_instance_;
class Program;
class ProgramDefaultTypeInternal;
extern ProgramDefaultTypeInternal _Program_default_instance_;
class Strings;
class StringsDefaultTypeInternal;
extern StringsDefaultTypeInternal _Strings_default_instance_;
}  // namespace DvObjecFile

namespace DvObjecFile {

namespace protobuf_objFile_2eproto {
// Internal implementation detail -- do not call these.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[];
  static const ::google::protobuf::uint32 offsets[];
  static void InitDefaultsImpl();
  static void Shutdown();
};
void AddDescriptors();
void InitDefaults();
}  // namespace protobuf_objFile_2eproto

// ===================================================================

class Program : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:DvObjecFile.Program) */ {
 public:
  Program();
  virtual ~Program();

  Program(const Program& from);

  inline Program& operator=(const Program& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Program& default_instance();

  static inline const Program* internal_default_instance() {
    return reinterpret_cast<const Program*>(
               &_Program_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(Program* other);

  // implements Message ----------------------------------------------

  inline Program* New() const PROTOBUF_FINAL { return New(NULL); }

  Program* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const Program& from);
  void MergeFrom(const Program& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(Program* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string signature = 1;
  void clear_signature();
  static const int kSignatureFieldNumber = 1;
  const ::std::string& signature() const;
  void set_signature(const ::std::string& value);
  #if LANG_CXX11
  void set_signature(::std::string&& value);
  #endif
  void set_signature(const char* value);
  void set_signature(const char* value, size_t size);
  ::std::string* mutable_signature();
  ::std::string* release_signature();
  void set_allocated_signature(::std::string* signature);

  // .DvObjecFile.Strings strings = 4;
  bool has_strings() const;
  void clear_strings();
  static const int kStringsFieldNumber = 4;
  const ::DvObjecFile::Strings& strings() const;
  ::DvObjecFile::Strings* mutable_strings();
  ::DvObjecFile::Strings* release_strings();
  void set_allocated_strings(::DvObjecFile::Strings* strings);

  // .DvObjecFile.Code code = 5;
  bool has_code() const;
  void clear_code();
  static const int kCodeFieldNumber = 5;
  const ::DvObjecFile::Code& code() const;
  ::DvObjecFile::Code* mutable_code();
  ::DvObjecFile::Code* release_code();
  void set_allocated_code(::DvObjecFile::Code* code);

  // int32 size = 2;
  void clear_size();
  static const int kSizeFieldNumber = 2;
  ::google::protobuf::int32 size() const;
  void set_size(::google::protobuf::int32 value);

  // int32 mainp = 3;
  void clear_mainp();
  static const int kMainpFieldNumber = 3;
  ::google::protobuf::int32 mainp() const;
  void set_mainp(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:DvObjecFile.Program)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr signature_;
  ::DvObjecFile::Strings* strings_;
  ::DvObjecFile::Code* code_;
  ::google::protobuf::int32 size_;
  ::google::protobuf::int32 mainp_;
  mutable int _cached_size_;
  friend struct protobuf_objFile_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class Strings : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:DvObjecFile.Strings) */ {
 public:
  Strings();
  virtual ~Strings();

  Strings(const Strings& from);

  inline Strings& operator=(const Strings& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Strings& default_instance();

  static inline const Strings* internal_default_instance() {
    return reinterpret_cast<const Strings*>(
               &_Strings_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    1;

  void Swap(Strings* other);

  // implements Message ----------------------------------------------

  inline Strings* New() const PROTOBUF_FINAL { return New(NULL); }

  Strings* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const Strings& from);
  void MergeFrom(const Strings& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(Strings* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated string inst = 1;
  int inst_size() const;
  void clear_inst();
  static const int kInstFieldNumber = 1;
  const ::std::string& inst(int index) const;
  ::std::string* mutable_inst(int index);
  void set_inst(int index, const ::std::string& value);
  #if LANG_CXX11
  void set_inst(int index, ::std::string&& value);
  #endif
  void set_inst(int index, const char* value);
  void set_inst(int index, const char* value, size_t size);
  ::std::string* add_inst();
  void add_inst(const ::std::string& value);
  #if LANG_CXX11
  void add_inst(::std::string&& value);
  #endif
  void add_inst(const char* value);
  void add_inst(const char* value, size_t size);
  const ::google::protobuf::RepeatedPtrField< ::std::string>& inst() const;
  ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_inst();

  // @@protoc_insertion_point(class_scope:DvObjecFile.Strings)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedPtrField< ::std::string> inst_;
  mutable int _cached_size_;
  friend struct protobuf_objFile_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class Code : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:DvObjecFile.Code) */ {
 public:
  Code();
  virtual ~Code();

  Code(const Code& from);

  inline Code& operator=(const Code& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Code& default_instance();

  static inline const Code* internal_default_instance() {
    return reinterpret_cast<const Code*>(
               &_Code_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    2;

  void Swap(Code* other);

  // implements Message ----------------------------------------------

  inline Code* New() const PROTOBUF_FINAL { return New(NULL); }

  Code* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const Code& from);
  void MergeFrom(const Code& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(Code* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated int64 inst = 1;
  int inst_size() const;
  void clear_inst();
  static const int kInstFieldNumber = 1;
  ::google::protobuf::int64 inst(int index) const;
  void set_inst(int index, ::google::protobuf::int64 value);
  void add_inst(::google::protobuf::int64 value);
  const ::google::protobuf::RepeatedField< ::google::protobuf::int64 >&
      inst() const;
  ::google::protobuf::RepeatedField< ::google::protobuf::int64 >*
      mutable_inst();

  // @@protoc_insertion_point(class_scope:DvObjecFile.Code)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int64 > inst_;
  mutable int _inst_cached_byte_size_;
  mutable int _cached_size_;
  friend struct protobuf_objFile_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// Program

// string signature = 1;
inline void Program::clear_signature() {
  signature_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& Program::signature() const {
  // @@protoc_insertion_point(field_get:DvObjecFile.Program.signature)
  return signature_.GetNoArena();
}
inline void Program::set_signature(const ::std::string& value) {
  
  signature_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:DvObjecFile.Program.signature)
}
#if LANG_CXX11
inline void Program::set_signature(::std::string&& value) {
  
  signature_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:DvObjecFile.Program.signature)
}
#endif
inline void Program::set_signature(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  signature_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:DvObjecFile.Program.signature)
}
inline void Program::set_signature(const char* value, size_t size) {
  
  signature_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:DvObjecFile.Program.signature)
}
inline ::std::string* Program::mutable_signature() {
  
  // @@protoc_insertion_point(field_mutable:DvObjecFile.Program.signature)
  return signature_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* Program::release_signature() {
  // @@protoc_insertion_point(field_release:DvObjecFile.Program.signature)
  
  return signature_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Program::set_allocated_signature(::std::string* signature) {
  if (signature != NULL) {
    
  } else {
    
  }
  signature_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), signature);
  // @@protoc_insertion_point(field_set_allocated:DvObjecFile.Program.signature)
}

// int32 size = 2;
inline void Program::clear_size() {
  size_ = 0;
}
inline ::google::protobuf::int32 Program::size() const {
  // @@protoc_insertion_point(field_get:DvObjecFile.Program.size)
  return size_;
}
inline void Program::set_size(::google::protobuf::int32 value) {
  
  size_ = value;
  // @@protoc_insertion_point(field_set:DvObjecFile.Program.size)
}

// int32 mainp = 3;
inline void Program::clear_mainp() {
  mainp_ = 0;
}
inline ::google::protobuf::int32 Program::mainp() const {
  // @@protoc_insertion_point(field_get:DvObjecFile.Program.mainp)
  return mainp_;
}
inline void Program::set_mainp(::google::protobuf::int32 value) {
  
  mainp_ = value;
  // @@protoc_insertion_point(field_set:DvObjecFile.Program.mainp)
}

// .DvObjecFile.Strings strings = 4;
inline bool Program::has_strings() const {
  return this != internal_default_instance() && strings_ != NULL;
}
inline void Program::clear_strings() {
  if (GetArenaNoVirtual() == NULL && strings_ != NULL) delete strings_;
  strings_ = NULL;
}
inline const ::DvObjecFile::Strings& Program::strings() const {
  // @@protoc_insertion_point(field_get:DvObjecFile.Program.strings)
  return strings_ != NULL ? *strings_
                         : *::DvObjecFile::Strings::internal_default_instance();
}
inline ::DvObjecFile::Strings* Program::mutable_strings() {
  
  if (strings_ == NULL) {
    strings_ = new ::DvObjecFile::Strings;
  }
  // @@protoc_insertion_point(field_mutable:DvObjecFile.Program.strings)
  return strings_;
}
inline ::DvObjecFile::Strings* Program::release_strings() {
  // @@protoc_insertion_point(field_release:DvObjecFile.Program.strings)
  
  ::DvObjecFile::Strings* temp = strings_;
  strings_ = NULL;
  return temp;
}
inline void Program::set_allocated_strings(::DvObjecFile::Strings* strings) {
  delete strings_;
  strings_ = strings;
  if (strings) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:DvObjecFile.Program.strings)
}

// .DvObjecFile.Code code = 5;
inline bool Program::has_code() const {
  return this != internal_default_instance() && code_ != NULL;
}
inline void Program::clear_code() {
  if (GetArenaNoVirtual() == NULL && code_ != NULL) delete code_;
  code_ = NULL;
}
inline const ::DvObjecFile::Code& Program::code() const {
  // @@protoc_insertion_point(field_get:DvObjecFile.Program.code)
  return code_ != NULL ? *code_
                         : *::DvObjecFile::Code::internal_default_instance();
}
inline ::DvObjecFile::Code* Program::mutable_code() {
  
  if (code_ == NULL) {
    code_ = new ::DvObjecFile::Code;
  }
  // @@protoc_insertion_point(field_mutable:DvObjecFile.Program.code)
  return code_;
}
inline ::DvObjecFile::Code* Program::release_code() {
  // @@protoc_insertion_point(field_release:DvObjecFile.Program.code)
  
  ::DvObjecFile::Code* temp = code_;
  code_ = NULL;
  return temp;
}
inline void Program::set_allocated_code(::DvObjecFile::Code* code) {
  delete code_;
  code_ = code;
  if (code) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:DvObjecFile.Program.code)
}

// -------------------------------------------------------------------

// Strings

// repeated string inst = 1;
inline int Strings::inst_size() const {
  return inst_.size();
}
inline void Strings::clear_inst() {
  inst_.Clear();
}
inline const ::std::string& Strings::inst(int index) const {
  // @@protoc_insertion_point(field_get:DvObjecFile.Strings.inst)
  return inst_.Get(index);
}
inline ::std::string* Strings::mutable_inst(int index) {
  // @@protoc_insertion_point(field_mutable:DvObjecFile.Strings.inst)
  return inst_.Mutable(index);
}
inline void Strings::set_inst(int index, const ::std::string& value) {
  // @@protoc_insertion_point(field_set:DvObjecFile.Strings.inst)
  inst_.Mutable(index)->assign(value);
}
#if LANG_CXX11
inline void Strings::set_inst(int index, ::std::string&& value) {
  // @@protoc_insertion_point(field_set:DvObjecFile.Strings.inst)
  inst_.Mutable(index)->assign(std::move(value));
}
#endif
inline void Strings::set_inst(int index, const char* value) {
  GOOGLE_DCHECK(value != NULL);
  inst_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set_char:DvObjecFile.Strings.inst)
}
inline void Strings::set_inst(int index, const char* value, size_t size) {
  inst_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:DvObjecFile.Strings.inst)
}
inline ::std::string* Strings::add_inst() {
  // @@protoc_insertion_point(field_add_mutable:DvObjecFile.Strings.inst)
  return inst_.Add();
}
inline void Strings::add_inst(const ::std::string& value) {
  inst_.Add()->assign(value);
  // @@protoc_insertion_point(field_add:DvObjecFile.Strings.inst)
}
#if LANG_CXX11
inline void Strings::add_inst(::std::string&& value) {
  inst_.Add(std::move(value));
  // @@protoc_insertion_point(field_add:DvObjecFile.Strings.inst)
}
#endif
inline void Strings::add_inst(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  inst_.Add()->assign(value);
  // @@protoc_insertion_point(field_add_char:DvObjecFile.Strings.inst)
}
inline void Strings::add_inst(const char* value, size_t size) {
  inst_.Add()->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_add_pointer:DvObjecFile.Strings.inst)
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
Strings::inst() const {
  // @@protoc_insertion_point(field_list:DvObjecFile.Strings.inst)
  return inst_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
Strings::mutable_inst() {
  // @@protoc_insertion_point(field_mutable_list:DvObjecFile.Strings.inst)
  return &inst_;
}

// -------------------------------------------------------------------

// Code

// repeated int64 inst = 1;
inline int Code::inst_size() const {
  return inst_.size();
}
inline void Code::clear_inst() {
  inst_.Clear();
}
inline ::google::protobuf::int64 Code::inst(int index) const {
  // @@protoc_insertion_point(field_get:DvObjecFile.Code.inst)
  return inst_.Get(index);
}
inline void Code::set_inst(int index, ::google::protobuf::int64 value) {
  inst_.Set(index, value);
  // @@protoc_insertion_point(field_set:DvObjecFile.Code.inst)
}
inline void Code::add_inst(::google::protobuf::int64 value) {
  inst_.Add(value);
  // @@protoc_insertion_point(field_add:DvObjecFile.Code.inst)
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int64 >&
Code::inst() const {
  // @@protoc_insertion_point(field_list:DvObjecFile.Code.inst)
  return inst_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int64 >*
Code::mutable_inst() {
  // @@protoc_insertion_point(field_mutable_list:DvObjecFile.Code.inst)
  return &inst_;
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


}  // namespace DvObjecFile

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_objFile_2eproto__INCLUDED
