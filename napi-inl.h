﻿#ifndef SRC_NAPI_INL_H_
#define SRC_NAPI_INL_H_

////////////////////////////////////////////////////////////////////////////////
// NAPI C++ Wrapper Classes
//
// Inline header-only implementations for "NAPI" ABI-stable C APIs for Node.js.
////////////////////////////////////////////////////////////////////////////////

#include <cassert>

namespace Napi {

////////////////////////////////////////////////////////////////////////////////
// Module registration
////////////////////////////////////////////////////////////////////////////////

#define NODE_API_MODULE(modname, regfunc)                 \
  void __napi_ ## regfunc(napi_env env,                   \
                          napi_value exports,             \
                          napi_value module,              \
                          void* priv) {                   \
    Napi::RegisterModule(env, exports, module, regfunc);  \
  }                                                       \
  NAPI_MODULE(modname, __napi_ ## regfunc);

// Adapt the NAPI_MODULE registration function:
//  - Wrap the arguments in NAPI wrappers.
//  - Catch any NAPI errors and rethrow as JS exceptions.
inline void RegisterModule(napi_env env,
                           napi_value exports,
                           napi_value module,
                           ModuleRegisterCallback registerCallback) {
  try {
      registerCallback(Napi::Env(env),
                       Napi::Object(env, exports),
                       Napi::Object(env, module));
  }
  catch (const Error& e) {
    if (!Napi::Env(env).IsExceptionPending()) {
      e.ThrowAsJavaScriptException();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Env class
////////////////////////////////////////////////////////////////////////////////

inline Env::Env(napi_env env) : _env(env) {
}

inline Env::operator napi_env() const {
  return _env;
}

inline Object Env::Global() const {
  napi_value value;
  napi_status status = napi_get_global(*this, &value);
  if (status != napi_ok) throw Error::New(*this);
  return Object(*this, value);
}

inline Value Env::Undefined() const {
  napi_value value;
  napi_status status = napi_get_undefined(*this, &value);
  if (status != napi_ok) throw Error::New(*this);
  return Value(*this, value);
}

inline Value Env::Null() const {
  napi_value value;
  napi_status status = napi_get_null(*this, &value);
  if (status != napi_ok) throw Error::New(*this);
  return Value(*this, value);
}

inline bool Env::IsExceptionPending() const {
  bool result;
  napi_status status = napi_is_exception_pending(_env, &result);
  if (status != napi_ok) result = false; // Checking for a pending exception shouldn't throw.
  return result;
}

////////////////////////////////////////////////////////////////////////////////
// Value class
////////////////////////////////////////////////////////////////////////////////

inline Value::Value() : _env(nullptr), _value(nullptr) {
}

inline Value::Value(napi_env env, napi_value value) : _env(env), _value(value) {
}

inline Value::operator napi_value() const {
  return _value;
}

inline bool Value::operator ==(const Value& other) const {
  return StrictEquals(other);
}

inline bool Value::operator !=(const Value& other) const {
  return !this->operator ==(other);
}

inline bool Value::StrictEquals(const Value& other) const {
  bool result;
  napi_status status = napi_strict_equals(_env, *this, other, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

inline Env Value::Env() const {
  return Napi::Env(_env);
}

inline napi_valuetype Value::Type() const {
  if (_value == nullptr) {
    return napi_undefined;
  }

  napi_valuetype type;
  napi_status status = napi_typeof(_env, _value, &type);
  if (status != napi_ok) throw Error::New(_env);
  return type;
}

inline bool Value::IsUndefined() const {
  return Type() == napi_undefined;
}

inline bool Value::IsNull() const {
  return Type() == napi_null;
}

inline bool Value::IsBoolean() const {
  return Type() == napi_boolean;
}

inline bool Value::IsNumber() const {
  return Type() == napi_number;
}

inline bool Value::IsString() const {
  return Type() == napi_string;
}

inline bool Value::IsSymbol() const {
  return Type() == napi_symbol;
}

inline bool Value::IsArray() const {
  if (_value == nullptr) {
    return false;
  }

  bool result;
  napi_status status = napi_is_array(_env, _value, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

inline bool Value::IsArrayBuffer() const {
  if (_value == nullptr) {
    return false;
  }

  bool result;
  napi_status status = napi_is_arraybuffer(_env, _value, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

inline bool Value::IsTypedArray() const {
  if (_value == nullptr) {
    return false;
  }

  bool result;
  napi_status status = napi_is_typedarray(_env, _value, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

inline bool Value::IsObject() const {
  return Type() == napi_object;
}

inline bool Value::IsFunction() const {
  return Type() == napi_function;
}

inline bool Value::IsBuffer() const {
  if (_value == nullptr) {
    return false;
  }

  bool result;
  napi_status status = napi_is_buffer(_env, _value, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

template <typename T>
inline T Value::As() const {
  return T(_env, _value);
}

inline Boolean Value::ToBoolean() const {
  napi_value result;
  napi_status status = napi_coerce_to_bool(_env, _value, &result);
  if (status != napi_ok) throw Error::New(_env);
  return Boolean(_env, result);
}

inline Number Value::ToNumber() const {
  napi_value result;
  napi_status status = napi_coerce_to_number(_env, _value, &result);
  if (status != napi_ok) throw Error::New(_env);
  return Number(_env, result);
}

inline String Value::ToString() const {
  napi_value result;
  napi_status status = napi_coerce_to_string(_env, _value, &result);
  if (status != napi_ok) throw Error::New(_env);
  return String(_env, result);
}

inline Object Value::ToObject() const {
  napi_value result;
  napi_status status = napi_coerce_to_object(_env, _value, &result);
  if (status != napi_ok) throw Error::New(_env);
  return Object(_env, result);
}

////////////////////////////////////////////////////////////////////////////////
// Boolean class
////////////////////////////////////////////////////////////////////////////////

inline Boolean Boolean::New(napi_env env, bool val) {
  napi_value value;
  napi_status status = napi_get_boolean(env, val, &value);
  if (status != napi_ok) throw Error::New(env);
  return Boolean(env, value);
}

inline Boolean::Boolean() : Napi::Value() {
}

inline Boolean::Boolean(napi_env env, napi_value value) : Napi::Value(env, value) {
}

inline Boolean::operator bool() const {
  return Value();
}

inline bool Boolean::Value() const {
  bool result;
  napi_status status = napi_get_value_bool(_env, _value, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

////////////////////////////////////////////////////////////////////////////////
// Number class
////////////////////////////////////////////////////////////////////////////////

inline Number Number::New(napi_env env, double val) {
  napi_value value;
  napi_status status = napi_create_number(env, val, &value);
  if (status != napi_ok) throw Error::New(env);
  return Number(env, value);
}

inline Number::Number() : Value() {
}

inline Number::Number(napi_env env, napi_value value) : Value(env, value) {
}

inline Number::operator int32_t() const {
  return Int32Value();
}

inline Number::operator uint32_t() const {
  return Uint32Value();
}

inline Number::operator int64_t() const {
  return Int64Value();
}

inline Number::operator float() const {
  return FloatValue();
}

inline Number::operator double() const {
  return DoubleValue();
}

inline int32_t Number::Int32Value() const {
  int32_t result;
  napi_status status = napi_get_value_int32(_env, _value, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

inline uint32_t Number::Uint32Value() const {
  uint32_t result;
  napi_status status = napi_get_value_uint32(_env, _value, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

inline int64_t Number::Int64Value() const {
  int64_t result;
  napi_status status = napi_get_value_int64(_env, _value, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

inline float Number::FloatValue() const {
  return static_cast<float>(DoubleValue());
}

inline double Number::DoubleValue() const {
  double result;
  napi_status status = napi_get_value_double(_env, _value, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

////////////////////////////////////////////////////////////////////////////////
// String class
////////////////////////////////////////////////////////////////////////////////

inline String String::New(napi_env env, const char* val, int length) {
  napi_value value;
  napi_status status = napi_create_string_utf8(env, val, length, &value);
  if (status != napi_ok) throw Error::New(env);
  return String(env, value);
}

inline String String::New(napi_env env, const char16_t* val, int length) {
  napi_value value;
  napi_status status = napi_create_string_utf16(env, val, length, &value);
  if (status != napi_ok) throw Error::New(env);
  return String(env, value);
}

inline String::String() : Value() {
}

inline String::String(napi_env env, napi_value value) : Value(env, value) {
}

inline String::operator std::string() const {
  return Utf8Value();
}

inline String::operator std::u16string() const {
  return Utf16Value();
}

inline std::string String::Utf8Value() const {
  size_t length;
  napi_status status = napi_get_value_string_utf8(_env, _value, nullptr, 0, &length);
  if (status != napi_ok) throw Error::New(_env);

  std::string value;
  value.resize(length);
  status = napi_get_value_string_utf8(_env, _value, &value[0], value.capacity(), nullptr);
  if (status != napi_ok) throw Error::New(_env);
  return value;
}

inline std::u16string String::Utf16Value() const {
  size_t length;
  napi_status status = napi_get_value_string_utf16(_env, _value, nullptr, 0, &length);
  if (status != napi_ok) throw Error::New(_env);

  std::u16string value;
  value.resize(length);
  status = napi_get_value_string_utf16(_env, _value, &value[0], value.capacity(), nullptr);
  if (status != napi_ok) throw Error::New(_env);
  return value;
}

////////////////////////////////////////////////////////////////////////////////
// Object class
////////////////////////////////////////////////////////////////////////////////

inline Object Object::New(napi_env env) {
  napi_value value;
  napi_status status = napi_create_object(env, &value);
  if (status != napi_ok) throw Error::New(env);
  return Object(env, value);
}

inline Object::Object() : Value() {
}

inline Object::Object(napi_env env, napi_value value) : Value(env, value) {
}

inline Value Object::operator [](const char* name) const {
  return Get(name);
}

inline Value Object::operator [](const std::string& name) const {
  return Get(name);
}

inline Value Object::operator [](uint32_t index) const {
  return Get(index);
}

inline bool Object::Has(napi_value name) const {
  bool result;
  napi_status status = napi_has_property(_env, _value, name, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

inline bool Object::Has(Value name) const {
  bool result;
  napi_status status = napi_has_property(_env, _value, name, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

inline bool Object::Has(const char* utf8name) const {
  bool result;
  napi_status status = napi_has_named_property(_env, _value, utf8name, &result);
  if (status != napi_ok) throw Error::New(Env());
  return result;
}

inline bool Object::Has(const std::string& utf8name) const {
  return Has(utf8name.c_str());
}

inline Value Object::Get(napi_value name) const {
  napi_value result;
  napi_status status = napi_get_property(_env, _value, name, &result);
  if (status != napi_ok) throw Error::New(_env);
  return Value(_env, result);
}

inline Value Object::Get(Value name) const {
  napi_value result;
  napi_status status = napi_get_property(_env, _value, name, &result);
  if (status != napi_ok) throw Error::New(_env);
  return Value(_env, result);
}

inline Value Object::Get(const char* utf8name) const {
  napi_value result;
  napi_status status = napi_get_named_property(_env, _value, utf8name, &result);
  if (status != napi_ok) throw Error::New(Env());
  return Value(_env, result);
}

inline Value Object::Get(const std::string& utf8name) const {
  return Get(utf8name.c_str());
}

inline void Object::Set(napi_value name, napi_value value) {
  napi_status status = napi_set_property(_env, _value, name, value);
  if (status != napi_ok) throw Error::New(_env);
}

inline void Object::Set(const char* utf8name, Value value) {
  napi_status status = napi_set_named_property(_env, _value, utf8name, value);
  if (status != napi_ok) throw Error::New(Env());
}

inline void Object::Set(const char* utf8name, napi_value value) {
  napi_status status = napi_set_named_property(_env, _value, utf8name, value);
  if (status != napi_ok) throw Error::New(Env());
}

inline void Object::Set(const char* utf8name, const char* utf8value) {
  Set(utf8name, String::New(Env(), utf8value));
}

inline void Object::Set(const char* utf8name, bool boolValue) {
  Set(utf8name, Boolean::New(Env(), boolValue));
}

inline void Object::Set(const char* utf8name, double numberValue) {
  Set(utf8name, Number::New(Env(), numberValue));
}

inline void Object::Set(const std::string& utf8name, napi_value value) {
  Set(utf8name.c_str(), value);
}

inline void Object::Set(const std::string& utf8name, Value value) {
  Set(utf8name.c_str(), value);
}

inline void Object::Set(const std::string& utf8name, std::string& utf8value) {
  Set(utf8name.c_str(), String::New(Env(), utf8value));
}

inline void Object::Set(const std::string& utf8name, bool boolValue) {
  Set(utf8name.c_str(), Boolean::New(Env(), boolValue));
}

inline void Object::Set(const std::string& utf8name, double numberValue) {
  Set(utf8name.c_str(), Number::New(Env(), numberValue));
}

inline bool Object::Has(uint32_t index) const {
  bool result;
  napi_status status = napi_has_element(_env, _value, index, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

inline Value Object::Get(uint32_t index) const {
  napi_value value;
  napi_status status = napi_get_element(_env, _value, index, &value);
  if (status != napi_ok) throw Error::New(_env);
  return Value(_env, value);
}

inline void Object::Set(uint32_t index, napi_value value) {
  napi_status status = napi_set_element(_env, _value, index, value);
  if (status != napi_ok) throw Error::New(_env);
}

inline void Object::Set(uint32_t index, Value value) {
  napi_status status = napi_set_element(_env, _value, index, value);
  if (status != napi_ok) throw Error::New(_env);
}

inline void Object::Set(uint32_t index, const char* utf8value) {
  Set(index, static_cast<napi_value>(String::New(Env(), utf8value)));
}

inline void Object::Set(uint32_t index, const std::string& utf8value) {
  Set(index, static_cast<napi_value>(String::New(Env(), utf8value)));
}

inline void Object::Set(uint32_t index, bool boolValue) {
  Set(index, static_cast<napi_value>(Boolean::New(Env(), boolValue)));
}

inline void Object::Set(uint32_t index, double numberValue) {
  Set(index, static_cast<napi_value>(Number::New(Env(), numberValue)));
}

inline void Object::DefineProperty(const PropertyDescriptor& property) {
  napi_status status = napi_define_properties(_env, _value, 1,
    reinterpret_cast<const napi_property_descriptor*>(&property));
  if (status != napi_ok) throw Error::New(_env);
}

inline void Object::DefineProperties(const std::initializer_list<PropertyDescriptor>& properties) {
  napi_status status = napi_define_properties(_env, _value, properties.size(),
    reinterpret_cast<const napi_property_descriptor*>(properties.begin()));
  if (status != napi_ok) throw Error::New(_env);
}

inline void Object::DefineProperties(const std::vector<PropertyDescriptor>& properties) {
  napi_status status = napi_define_properties(_env, _value, properties.size(),
    reinterpret_cast<const napi_property_descriptor*>(properties.data()));
  if (status != napi_ok) throw Error::New(_env);
}

inline bool Object::InstanceOf(const Function& constructor) const {
  bool result;
  napi_status status = napi_instanceof(_env, _value, constructor, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

////////////////////////////////////////////////////////////////////////////////
// External class
////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline External<T> External<T>::New(
    napi_env env, T* data, napi_finalize finalizeCallback, void* finalizeHint) {
  napi_value value;
  napi_status status = napi_create_external(env, data, finalizeCallback, finalizeHint, &value);
  if (status != napi_ok) throw Error::New(env);
  return External(env, value);
}

template <typename T>
inline External<T>::External() : Value() {
}

template <typename T>
inline External<T>::External(napi_env env, napi_value value) : Value(env, value) {
}

template <typename T>
inline T* External<T>::Data() const {
  void* data;
  napi_status status = napi_get_value_external(_env, _value, &data);
  if (status != napi_ok) throw Error::New(_env);
  return reinterpret_cast<T*>(data);
}

////////////////////////////////////////////////////////////////////////////////
// Array class
////////////////////////////////////////////////////////////////////////////////

inline Array Array::New(napi_env env) {
  napi_value value;
  napi_status status = napi_create_array(env, &value);
  if (status != napi_ok) throw Error::New(env);
  return Array(env, value);
}

inline Array Array::New(napi_env env, int length) {
  napi_value value;
  napi_status status = napi_create_array_with_length(env, length, &value);
  if (status != napi_ok) throw Error::New(env);
  return Array(env, value);
}

inline Array::Array() : Object() {
}

inline Array::Array(napi_env env, napi_value value) : Object(env, value) {
}

inline uint32_t Array::Length() const {
  uint32_t result;
  napi_status status = napi_get_array_length(_env, _value, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

////////////////////////////////////////////////////////////////////////////////
// ArrayBuffer class
////////////////////////////////////////////////////////////////////////////////

inline ArrayBuffer ArrayBuffer::New(napi_env env, size_t byteLength) {
  napi_value value;
  void* data;
  napi_status status = napi_create_arraybuffer(env, byteLength, &data, &value);
  if (status != napi_ok) throw Error::New(env);

  ArrayBuffer arrayBuffer(env, value);
  arrayBuffer._data = data;
  arrayBuffer._length = byteLength;
  return arrayBuffer;
}

inline ArrayBuffer ArrayBuffer::New(napi_env env,
                                    void* externalData,
                                    size_t byteLength,
                                    napi_finalize finalizeCallback,
                                    void* finalizeHint) {
  napi_value value;
  napi_status status = napi_create_external_arraybuffer(
    env, externalData, byteLength, finalizeCallback, finalizeHint, &value);
  if (status != napi_ok) throw Error::New(env);

  ArrayBuffer arrayBuffer(env, value);
  arrayBuffer._data = externalData;
  arrayBuffer._length = byteLength;
  return arrayBuffer;
}

inline ArrayBuffer::ArrayBuffer() : Object(), _data(nullptr), _length(0) {
}

inline ArrayBuffer::ArrayBuffer(napi_env env, napi_value value)
  : Object(env, value), _data(nullptr), _length(0) {
}

inline void* ArrayBuffer::Data() {
  return _data;
}

inline size_t ArrayBuffer::ByteLength() {
  return _length;
}

////////////////////////////////////////////////////////////////////////////////
// TypedArray class
////////////////////////////////////////////////////////////////////////////////

inline TypedArray::TypedArray() : Object(), _type(TypedArray::unknown_type), _length(0) {
}

inline TypedArray::TypedArray(napi_env env, napi_value value)
  : Object(env, value), _type(TypedArray::unknown_type), _length(0) {
}

inline TypedArray::TypedArray(napi_env env,
                              napi_value value,
                              napi_typedarray_type type,
                              size_t length) : Object(env, value), _type(type), _length(length) {
}

inline napi_typedarray_type TypedArray::TypedArrayType() const {
  if (_type == TypedArray::unknown_type) {
    napi_status status = napi_get_typedarray_info(_env, _value,
      &const_cast<TypedArray*>(this)->_type, &const_cast<TypedArray*>(this)->_length,
      nullptr, nullptr, nullptr);
    if (status != napi_ok) throw Error::New(_env);
  }

  return _type;
}

inline uint8_t TypedArray::ElementSize() const {
  switch (TypedArrayType()) {
    case napi_int8:
    case napi_uint8:
    case napi_uint8_clamped:
      return 1;
    case napi_int16:
    case napi_uint16:
      return 2;
    case napi_int32:
    case napi_uint32:
    case napi_float32:
      return 4;
    case napi_float64:
      return 8;
    default:
      return 0;
  }
}

inline size_t TypedArray::ElementLength() const {
  if (_type == TypedArray::unknown_type) {
    napi_status status = napi_get_typedarray_info(_env, _value,
      &const_cast<TypedArray*>(this)->_type, &const_cast<TypedArray*>(this)->_length,
      nullptr, nullptr, nullptr);
    if (status != napi_ok) throw Error::New(_env);
  }

  return _length;
}

inline size_t TypedArray::ByteOffset() const {
  size_t byteOffset;
  napi_status status = napi_get_typedarray_info(
    _env, _value, nullptr, nullptr, nullptr, nullptr, &byteOffset);
  if (status != napi_ok) throw Error::New(_env);
  return byteOffset;
}

inline size_t TypedArray::ByteLength() const {
  return ElementSize() * ElementLength();
}

inline ArrayBuffer TypedArray::ArrayBuffer() const {
  napi_value arrayBuffer;
  napi_status status = napi_get_typedarray_info(
    _env, _value, nullptr, nullptr, nullptr, &arrayBuffer, nullptr);
  if (status != napi_ok) throw Error::New(_env);
  return Napi::ArrayBuffer(_env, arrayBuffer);
}

inline Int8Array TypedArray::AsInt8Array() const {
  return Int8Array(_env, _value);
}

inline Uint8Array TypedArray::AsUint8Array() const {
  return Uint8Array(_env, _value);
}

inline Uint8ClampedArray TypedArray::AsUint8ClampedArray() const {
  return Uint8ClampedArray(_env, _value);
}

inline Int16Array TypedArray::AsInt16Array() const {
  return Int16Array(_env, _value);
}

inline Uint16Array TypedArray::AsUint16Array() const {
  return Uint16Array(_env, _value);
}

inline Int32Array TypedArray::AsInt32Array() const {
  return Int32Array(_env, _value);
}

inline Uint32Array TypedArray::AsUint32Array() const {
  return Uint32Array(_env, _value);
}

inline Float32Array TypedArray::AsFloat32Array() const {
  return Float32Array(_env, _value);
}

inline Float64Array TypedArray::AsFloat64Array() const {
  return Float64Array(_env, _value);
}

////////////////////////////////////////////////////////////////////////////////
// TypedArray_<T,A> class
////////////////////////////////////////////////////////////////////////////////

template <typename T, napi_typedarray_type A>
inline TypedArray_<T,A> TypedArray_<T,A>::New(napi_env env, size_t elementLength) {
  Napi::ArrayBuffer arrayBuffer = Napi::ArrayBuffer::New(env, elementLength * sizeof (T));
  return New(env, elementLength, arrayBuffer, 0);
}

template <typename T, napi_typedarray_type A>
inline TypedArray_<T,A> TypedArray_<T,A>::New(napi_env env,
                                              size_t elementLength,
                                              Napi::ArrayBuffer arrayBuffer,
                                              size_t bufferOffset) {
  napi_value value;
  napi_status status = napi_create_typedarray(
    env, A, elementLength, arrayBuffer, bufferOffset, &value);
  if (status != napi_ok) throw Error::New(env);

  return TypedArray_<T,A>(env, value, elementLength, reinterpret_cast<T*>(arrayBuffer.Data()));
}

template <typename T, napi_typedarray_type A>
inline TypedArray_<T,A>::TypedArray_() : TypedArray(), _data(nullptr) {
}

template <typename T, napi_typedarray_type A>
inline TypedArray_<T,A>::TypedArray_(napi_env env, napi_value value)
  : TypedArray(env, value, A, 0), _data(nullptr) {
  napi_status status = napi_get_typedarray_info(
    _env, _value, nullptr, &_length, reinterpret_cast<void**>(&_data), nullptr, nullptr);
  if (status != napi_ok) throw Error::New(_env);
}

template <typename T, napi_typedarray_type A>
inline TypedArray_<T,A>::TypedArray_(napi_env env, napi_value value, size_t length, T* data)
  : TypedArray(env, value, A, length), _data(data) {
}

template <typename T, napi_typedarray_type A>
inline T& TypedArray_<T,A>::operator [](size_t index) {
  return _data[index];
}

template <typename T, napi_typedarray_type A>
inline const T& TypedArray_<T,A>::operator [](size_t index) const {
  return _data[index];
}

template <typename T, napi_typedarray_type A>
inline T* TypedArray_<T,A>::Data() {
  return _data;
}

template <typename T, napi_typedarray_type A>
inline const T* TypedArray_<T,A>::Data() const {
  return _data;
}

////////////////////////////////////////////////////////////////////////////////
// Function class
////////////////////////////////////////////////////////////////////////////////

inline Function Function::New(napi_env env,
                              VoidFunctionCallback cb,
                              const char* utf8name,
                              void* data) {
  // TODO: Delete when the function is destroyed
  CallbackData* callbackData = new CallbackData(cb, data);

  // TODO: set the function name
  napi_value value;
  napi_status status = napi_create_function(
    env, utf8name, VoidFunctionCallbackWrapper, callbackData, &value);
  if (status != napi_ok) throw Error::New(env);
  return Function(env, value);
}

inline Function Function::New(napi_env env,
                              FunctionCallback cb,
                              const char* utf8name,
                              void* data) {
  // TODO: Delete when the function is destroyed
  CallbackData* callbackData = new CallbackData(cb, data);

  napi_value value;
  napi_status status = napi_create_function(
    env, utf8name, FunctionCallbackWrapper, callbackData, &value);
  if (status != napi_ok) throw Error::New(env);
  return Function(env, value);
}

inline Function Function::New(napi_env env,
                              VoidFunctionCallback cb,
                              const std::string& utf8name,
                              void* data) {
  return New(env, cb, utf8name.c_str(), data);
}

inline Function Function::New(napi_env env,
                              FunctionCallback cb,
                              const std::string& utf8name,
                              void* data) {
  return New(env, cb, utf8name.c_str(), data);
}

inline Function::Function() : Object() {
}

inline Function::Function(napi_env env, napi_value value) : Object(env, value) {
}

inline Value Function::operator ()(const std::initializer_list<napi_value>& args) const {
  return Call(Env().Global(), args);
}

inline Value Function::Call(const std::initializer_list<napi_value>& args) const {
  return Call(Env().Global(), args);
}

inline Value Function::Call(const std::vector<napi_value>& args) const {
  return Call(Env().Global(), args);
}

inline Value Function::Call(napi_value recv, const std::initializer_list<napi_value>& args) const {
  napi_value result;
  napi_status status = napi_call_function(
    _env, recv, _value, args.size(), args.begin(), &result);
  if (status != napi_ok) throw Error::New(_env);
  return Value(_env, result);
}

inline Value Function::Call(napi_value recv, const std::vector<napi_value>& args) const {
  napi_value result;
  napi_status status = napi_call_function(
    _env, recv, _value, args.size(), args.data(), &result);
  if (status != napi_ok) throw Error::New(_env);
  return Value(_env, result);
}

inline Value Function::MakeCallback(const std::initializer_list<napi_value>& args) const {
  return MakeCallback(Env().Global(), args);
}

inline Value Function::MakeCallback(const std::vector<napi_value>& args) const {
  return MakeCallback(Env().Global(), args);
}

inline Value Function::MakeCallback(
    napi_value recv, const std::initializer_list<napi_value>& args) const {
  napi_value result;
  napi_status status = napi_make_callback(
    _env, recv, _value, args.size(), args.begin(), &result);
  if (status != napi_ok) throw Error::New(_env);
  return Value(_env, result);
}

inline Value Function::MakeCallback(
    napi_value recv, const std::vector<napi_value>& args) const {
  napi_value result;
  napi_status status = napi_make_callback(
    _env, recv, _value, args.size(), args.data(), &result);
  if (status != napi_ok) throw Error::New(_env);
  return Value(_env, result);
}

inline Object Function::New(const std::initializer_list<napi_value>& args) const {
  napi_value result;
  napi_status status = napi_new_instance(
    _env, _value, args.size(), args.begin(), &result);
  if (status != napi_ok) throw Error::New(_env);
  return Object(_env, result);
}

inline Object Function::New(const std::vector<napi_value>& args) const {
  napi_value result;
  napi_status status = napi_new_instance(
    _env, _value, args.size(), args.data(), &result);
  if (status != napi_ok) throw Error::New(_env);
  return Object(_env, result);
}

inline void Function::VoidFunctionCallbackWrapper(napi_env env, napi_callback_info info) {
  try {
    CallbackInfo callbackInfo(env, info);
    CallbackData* callbackData = reinterpret_cast<CallbackData*>(callbackInfo.Data());
    callbackData->functionCallback(callbackInfo);
  }
  catch (const Error& e) {
    if (!Napi::Env(env).IsExceptionPending()) {
      e.ThrowAsJavaScriptException();
    }
    return;
  }
}

inline void Function::FunctionCallbackWrapper(napi_env env, napi_callback_info info) {
  napi_value result;
  try {
    CallbackInfo callbackInfo(env, info);
    CallbackData* callbackData = reinterpret_cast<CallbackData*>(callbackInfo.Data());
    result = callbackData->functionCallback(callbackInfo);
  }
  catch (const Error& e) {
    if (!Napi::Env(env).IsExceptionPending()) {
      e.ThrowAsJavaScriptException();
    }
    return;
  }

  napi_status status = napi_set_return_value(env, info, result);
  if (status != napi_ok) return;
}

inline Function::CallbackData::CallbackData(VoidFunctionCallback cb, void* data)
  : voidFunctionCallback(cb), data(data) {
}

inline Function::CallbackData::CallbackData(FunctionCallback cb, void* data)
  : functionCallback(cb), data(data) {
}

////////////////////////////////////////////////////////////////////////////////
// Buffer<T> class
////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline Buffer<T> Buffer<T>::New(napi_env env, size_t length) {
  napi_value value;
  void* data;
  napi_status status = napi_create_buffer(env, length * sizeof (T), &data, &value);
  if (status != napi_ok) throw Error::New(env);
  return Buffer(env, value, length, data);
}

template <typename T>
inline Buffer<T> Buffer<T>::New(
    napi_env env, T* data, size_t length, napi_finalize finalizeCallback, void* finalizeHint) {
  napi_value value;
  napi_status status = napi_create_external_buffer(
    env, length * sizeof (T), data, finalizeCallback, finalizeHint, &value);
  if (status != napi_ok) throw Error::New(env);
  return Buffer(env, value, length, data);
}

template <typename T>
inline Buffer<T> Buffer<T>::Copy(napi_env env, const T* data, size_t length) {
  napi_value value;
  napi_status status = napi_create_buffer_copy(
    env, length * sizeof (T), data, nullptr, &value);
  if (status != napi_ok) throw Error::New(env);
  return Buffer(env, value);
}

template <typename T>
inline Buffer<T>::Buffer() : Object(), _length(0), _data(nullptr) {
}

template <typename T>
inline Buffer<T>::Buffer(napi_env env, napi_value value)
  : Object(env, value), _length(0), _data(nullptr) {
}

template <typename T>
inline Buffer<T>::Buffer(napi_env env, napi_value value, size_t length, T* data)
  : Object(env, value), _length(length), _data(data) {
}

template <typename T>
inline size_t Buffer<T>::Length() const {
  EnsureInfo();
  return _length;
}

template <typename T>
inline T* Buffer<T>::Data() const {
  EnsureInfo();
  return _data;
}

template <typename T>
inline void Buffer<T>::EnsureInfo() const {
  // The Buffer instance may have been constructed from a napi_value whose
  // length/data are not yet known. Fetch and cache these values just once,
  // since they can never change during the lifetime of the Buffer.
  if (_data == nullptr) {
    size_t byteLength;
    void* voidData;
    napi_status status = napi_get_buffer_info(_env, _value, &voidData, &byteLength);
    if (status != napi_ok) throw Error::New(_env);
    _length = byteLength / sizeof (T);
    _data = static_cast<T*>(voidData);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Error class
////////////////////////////////////////////////////////////////////////////////

inline Error Error::New(napi_env env) {
  napi_value error = nullptr;
  if (Napi::Env(env).IsExceptionPending()) {
    napi_get_and_clear_last_exception(env, &error);
  }
  else {
    // No JS exception is pending, so check for NAPI error info.
    const napi_extended_error_info* info = napi_get_last_error_info();

    const char* error_message = info->error_message != nullptr ?
      info->error_message : "Error in native callback";
    napi_value message;
    napi_status status = napi_create_string_utf8(
      env,
      error_message,
      strlen(error_message),
      &message);
    assert(status == napi_ok);

    if (status == napi_ok) {
      switch (info->error_code) {
      case napi_object_expected:
      case napi_string_expected:
      case napi_boolean_expected:
      case napi_number_expected:
        status = napi_create_type_error(env, message, &error);
        break;
      default:
        status = napi_create_error(env, message, &error);
        break;
      }
      assert(status == napi_ok);
    }
  }

  return Error(env, error);
}

inline Error Error::New(napi_env env, const char* message) {
  return Error::New<Error>(env, message, strlen(message), napi_create_error);
}

inline Error Error::New(napi_env env, const std::string& message) {
  return Error::New<Error>(env, message.c_str(), message.size(), napi_create_error);
}

inline Error::Error() : Object(), _message(nullptr) {
}

inline Error::Error(napi_env env, napi_value value) : Object(env, value) {
}

inline std::string Error::Message() const {
  if (_message.size() == 0 && _env != nullptr) {
    try {
      const_cast<Error*>(this)->_message = (*this)["message"].As<String>();
    }
    catch (const Error&) {
    }
  }
  return _message;
}

inline void Error::ThrowAsJavaScriptException() const {
  if (_value != nullptr) {
    napi_throw(_env, _value);
  }
}

inline const char* Error::what() const _NOEXCEPT {
  return Message().c_str();
}

template <typename TError>
inline TError Error::New(napi_env env,
                         const char* message,
                         size_t length,
                         create_error_fn create_error) {
  napi_value str;
  napi_status status = napi_create_string_utf8(env, message, length, &str);
  if (status != napi_ok) throw Error::New(env);

  napi_value error;
  status = create_error(env, str, &error);
  if (status != napi_ok) throw Error::New(env);

  return TError(env, error);
}

inline TypeError TypeError::New(napi_env env, const char* message) {
  return Error::New<TypeError>(env, message, strlen(message), napi_create_type_error);
}

inline TypeError TypeError::New(napi_env env, const std::string& message) {
  return Error::New<TypeError>(env, message.c_str(), message.size(), napi_create_type_error);
}

inline TypeError::TypeError() : Error() {
}

inline TypeError::TypeError(napi_env env, napi_value value) : Error(env, value) {
}

inline RangeError RangeError::New(napi_env env, const char* message) {
  return Error::New<RangeError>(env, message, strlen(message), napi_create_range_error);
}

inline RangeError RangeError::New(napi_env env, const std::string& message) {
  return Error::New<RangeError>(env, message.c_str(), message.size(), napi_create_range_error);
}

inline RangeError::RangeError() : Error() {
}

inline RangeError::RangeError(napi_env env, napi_value value) : Error(env, value) {
}

////////////////////////////////////////////////////////////////////////////////
// Reference<T> class
////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline Reference<T> Reference<T>::New(const T& value, int initialRefcount) {
  napi_env env = value.Env();
  napi_value val = value;

  if (val == nullptr) {
    return Reference<T>(env, nullptr);
  }

  napi_ref ref;
  napi_status status = napi_create_reference(env, value, initialRefcount, &ref);
  if (status != napi_ok) throw Error::New(napi_env(env));

  return Reference<T>(env, ref);
}


template <typename T>
inline Reference<T>::Reference() : _env(nullptr), _ref(nullptr), _suppressDestruct(false) {
}

template <typename T>
inline Reference<T>::Reference(napi_env env, napi_ref persistent)
  : _env(env), _ref(persistent) {
}

template <typename T>
inline Reference<T>::~Reference() {
  if (_ref != nullptr) {
    if (!_suppressDestruct) {
      napi_delete_reference(_env, _ref);
    }

    _ref = nullptr;
  }
}

template <typename T>
inline Reference<T>::Reference(Reference<T>&& other) {
  _env = other._env;
  _ref = other._ref;
  other._env = nullptr;
  other._ref = nullptr;
}

template <typename T>
inline Reference<T>& Reference<T>::operator =(Reference<T>&& other) {
  _env = other._env;
  _ref = other._ref;
  other._env = nullptr;
  other._ref = nullptr;
  return *this;
}

template <typename T>
inline Reference<T>::operator napi_ref() const {
  return _ref;
}

template <typename T>
inline bool Reference<T>::operator ==(const Reference<T> &other) const {
  HandleScope scope(_env);
  return this->Value().StrictEquals(other.Value());
}

template <typename T>
inline bool Reference<T>::operator !=(const Reference<T> &other) const {
  return !this->operator ==(other);
}

template <typename T>
inline Env Reference<T>::Env() const {
  return Napi::Env(_env);
}

template <typename T>
inline bool Reference<T>::IsEmpty() const {
  return _ref == nullptr;
}

template <typename T>
inline T Reference<T>::Value() const {
  if (_ref == nullptr) {
    return T(_env, nullptr);
  }

  napi_value value;
  napi_status status = napi_get_reference_value(_env, _ref, &value);
  if (status != napi_ok) throw Error::New(_env);
  return T(_env, value);
}

template <typename T>
inline int Reference<T>::Ref() {
  int result;
  napi_status status = napi_reference_ref(_env, _ref, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

template <typename T>
inline int Reference<T>::Unref() {
  int result;
  napi_status status = napi_reference_unref(_env, _ref, &result);
  if (status != napi_ok) throw Error::New(_env);
  return result;
}

template <typename T>
inline void Reference<T>::Reset() {
  if (_ref != nullptr) {
    napi_status status = napi_delete_reference(_env, _ref);
    if (status != napi_ok) throw Error::New(_env);
    _ref = nullptr;
  }
}

template <typename T>
inline void Reference<T>::Reset(const T& value, int refcount) {
  Reset();
  _env = value.Env();

  napi_value val = value;
  if (val != nullptr) {
    napi_status status = napi_create_reference(_env, value, refcount, &_ref);
    if (status != napi_ok) throw Error::New(_env);
  }
}

template <typename T>
inline void Reference<T>::SuppressDestruct() {
  _suppressDestruct = true;
}

template <typename T>
inline Reference<T> Weak(T value) {
  return Reference<T>::New(value, 0);
}

inline ObjectReference Weak(Object value) {
  return Reference<Object>::New(value, 0);
}

inline FunctionReference Weak(Function value) {
  return Reference<Function>::New(value, 0);
}

template <typename T>
inline Reference<T> Persistent(T value) {
  return Reference<T>::New(value, 1);
}

inline ObjectReference Persistent(Object value) {
  return Reference<Object>::New(value, 1);
}

inline FunctionReference Persistent(Function value) {
  return Reference<Function>::New(value, 1);
}

////////////////////////////////////////////////////////////////////////////////
// ObjectReference class
////////////////////////////////////////////////////////////////////////////////

inline ObjectReference::ObjectReference(): Reference<Object>() {
}

inline ObjectReference::ObjectReference(napi_env env, napi_ref ref): Reference<Object>(env, ref) {
}

inline ObjectReference::ObjectReference(Reference<Object>&& other)
  : Reference<Object>(std::move(other)) {
}

inline ObjectReference& ObjectReference::operator =(Reference<Object>&& other) {
  static_cast<Reference<Object>*>(this)->operator=(std::move(other));
  return *this;
}

inline ObjectReference::ObjectReference(ObjectReference&& other)
  : Reference<Object>(std::move(other)) {
}

inline ObjectReference& ObjectReference::operator =(ObjectReference&& other) {
  static_cast<Reference<Object>*>(this)->operator=(std::move(other));
  return *this;
}

inline Napi::Value ObjectReference::Get(const char* utf8name) const {
  EscapableHandleScope scope(_env);
  return scope.Escape(Value().Get(utf8name));
}

inline Napi::Value ObjectReference::Get(const std::string& utf8name) const {
  EscapableHandleScope scope(_env);
  return scope.Escape(Value().Get(utf8name));
}

inline void ObjectReference::Set(const char* utf8name, napi_value value) {
  HandleScope scope(_env);
  Value().Set(utf8name, value);
}

inline void ObjectReference::Set(const char* utf8name, Napi::Value value) {
  HandleScope scope(_env);
  Value().Set(utf8name, value);
}

inline void ObjectReference::Set(const char* utf8name, const char* utf8value) {
  HandleScope scope(_env);
  Value().Set(utf8name, utf8value);
}

inline void ObjectReference::Set(const char* utf8name, bool boolValue) {
  HandleScope scope(_env);
  Value().Set(utf8name, boolValue);
}

inline void ObjectReference::Set(const char* utf8name, double numberValue) {
  HandleScope scope(_env);
  Value().Set(utf8name, numberValue);
}

inline void ObjectReference::Set(const std::string& utf8name, napi_value value) {
  HandleScope scope(_env);
  Value().Set(utf8name, value);
}

inline void ObjectReference::Set(const std::string& utf8name, Napi::Value value) {
  HandleScope scope(_env);
  Value().Set(utf8name, value);
}

inline void ObjectReference::Set(const std::string& utf8name, std::string& utf8value) {
  HandleScope scope(_env);
  Value().Set(utf8name, utf8value);
}

inline void ObjectReference::Set(const std::string& utf8name, bool boolValue) {
  HandleScope scope(_env);
  Value().Set(utf8name, boolValue);
}

inline void ObjectReference::Set(const std::string& utf8name, double numberValue) {
  HandleScope scope(_env);
  Value().Set(utf8name, numberValue);
}

inline Napi::Value ObjectReference::Get(uint32_t index) const {
  EscapableHandleScope scope(_env);
  return scope.Escape(Value().Get(index));
}

inline void ObjectReference::Set(uint32_t index, napi_value value) {
  HandleScope scope(_env);
  Value().Set(index, value);
}

inline void ObjectReference::Set(uint32_t index, Napi::Value value) {
  HandleScope scope(_env);
  Value().Set(index, value);
}

inline void ObjectReference::Set(uint32_t index, const char* utf8value) {
  HandleScope scope(_env);
  Value().Set(index, utf8value);
}

inline void ObjectReference::Set(uint32_t index, const std::string& utf8value) {
  HandleScope scope(_env);
  Value().Set(index, utf8value);
}

inline void ObjectReference::Set(uint32_t index, bool boolValue) {
  HandleScope scope(_env);
  Value().Set(index, boolValue);
}

inline void ObjectReference::Set(uint32_t index, double numberValue) {
  HandleScope scope(_env);
  Value().Set(index, numberValue);
}

////////////////////////////////////////////////////////////////////////////////
// FunctionReference class
////////////////////////////////////////////////////////////////////////////////

inline FunctionReference::FunctionReference(): Reference<Function>() {
}

inline FunctionReference::FunctionReference(napi_env env, napi_ref ref)
  : Reference<Function>(env, ref) {
}

inline FunctionReference::FunctionReference(Reference<Function>&& other)
  : Reference<Function>(std::move(other)) {
}

inline FunctionReference& FunctionReference::operator =(Reference<Function>&& other) {
  static_cast<Reference<Function>*>(this)->operator=(std::move(other));
  return *this;
}

inline FunctionReference::FunctionReference(FunctionReference&& other)
  : Reference<Function>(std::move(other)) {
}

inline FunctionReference& FunctionReference::operator =(FunctionReference&& other) {
  static_cast<Reference<Function>*>(this)->operator=(std::move(other));
  return *this;
}

inline Napi::Value FunctionReference::operator ()(
    const std::initializer_list<napi_value>& args) const {
  EscapableHandleScope scope(_env);
  return scope.Escape(Value()(args));
}

inline Napi::Value FunctionReference::Call(const std::initializer_list<napi_value>& args) const {
  EscapableHandleScope scope(_env);
  return scope.Escape(Value().Call(args));
}

inline Napi::Value FunctionReference::Call(const std::vector<napi_value>& args) const {
  EscapableHandleScope scope(_env);
  return scope.Escape(Value().Call(args));
}

inline Napi::Value FunctionReference::Call(
    napi_value recv, const std::initializer_list<napi_value>& args) const {
  EscapableHandleScope scope(_env);
  return scope.Escape(Value().Call(recv, args));
}

inline Napi::Value FunctionReference::Call(
    napi_value recv, const std::vector<napi_value>& args) const {
  EscapableHandleScope scope(_env);
  return scope.Escape(Value().Call(recv, args));
}

inline Napi::Value FunctionReference::MakeCallback(const std::initializer_list<napi_value>& args) const {
  EscapableHandleScope scope(_env);
  return scope.Escape(Value().MakeCallback(args));
}

inline Napi::Value FunctionReference::MakeCallback(const std::vector<napi_value>& args) const {
  EscapableHandleScope scope(_env);
  return scope.Escape(Value().MakeCallback(args));
}

inline Napi::Value FunctionReference::MakeCallback(
    napi_value recv, const std::initializer_list<napi_value>& args) const {
  EscapableHandleScope scope(_env);
  return scope.Escape(Value().MakeCallback(recv, args));
}

inline Napi::Value FunctionReference::MakeCallback(
    napi_value recv, const std::vector<napi_value>& args) const {
  EscapableHandleScope scope(_env);
  return scope.Escape(Value().MakeCallback(recv, args));
}

inline Object FunctionReference::New(const std::initializer_list<napi_value>& args) const {
  EscapableHandleScope scope(_env);
  return scope.Escape(Value().New(args)).As<Object>();
}

inline Object FunctionReference::New(const std::vector<napi_value>& args) const {
  EscapableHandleScope scope(_env);
  return scope.Escape(Value().New(args)).As<Object>();
}

////////////////////////////////////////////////////////////////////////////////
// CallbackInfo class
////////////////////////////////////////////////////////////////////////////////

inline CallbackInfo::CallbackInfo(napi_env env, napi_callback_info info)
    : _env(env), _this(nullptr), _dynamicArgs(nullptr), _data(nullptr) {
  napi_status status = napi_get_cb_this(env, info, &_this);
  if (status != napi_ok) throw Error::New(_env);

  status = napi_get_cb_args_length(env, info, &_argc);
  if (status != napi_ok) throw Error::New(_env);

  if (_argc > 0) {
    // Use either a fixed-size array (on the stack) or a dynamically-allocated
    // array (on the heap) depending on the number of args.
    if (_argc <= _staticArgCount) {
      _argv = _staticArgs;
    }
    else {
      _dynamicArgs = new napi_value[_argc];
      _argv = _dynamicArgs;
    }

    status = napi_get_cb_args(env, info, _argv, _argc);
    if (status != napi_ok) throw Error::New(_env);
  }

  status = napi_get_cb_data(env, info, reinterpret_cast<void**>(&_data));
  if (status != napi_ok) throw Error::New(_env);
}

inline CallbackInfo::~CallbackInfo() {
  if (_dynamicArgs != nullptr) {
    delete[] _dynamicArgs;
  }
}

inline Env CallbackInfo::Env() const {
  return Napi::Env(_env);
}

inline int CallbackInfo::Length() const {
  return _argc;
}

inline const Value CallbackInfo::operator [](int index) const {
  return index < _argc ? Value(_env, _argv[index]) : Env().Undefined();
}

inline Object CallbackInfo::This() const {
  if (_this == nullptr) {
    return Env().Global();
  }
  return Object(_env, _this);
}

inline void* CallbackInfo::Data() const {
  return _data;
}

inline void CallbackInfo::SetData(void* data) {
  _data = data;
}

////////////////////////////////////////////////////////////////////////////////
// ObjectWrap<T> class
////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline ObjectWrap<T>::ObjectWrap() {
}

template<typename T>
inline T* ObjectWrap<T>::Unwrap(Object wrapper) {
  T* unwrapped;
  napi_status status = napi_unwrap(wrapper.Env(), wrapper, reinterpret_cast<void**>(&unwrapped));
  if (status != napi_ok) throw Error::New(wrapper.Env());
  return unwrapped;
}

template <typename T>
inline Function ObjectWrap<T>::DefineClass(
    Napi::Env env,
    const char* utf8name,
    const std::initializer_list<ClassPropertyDescriptor<T>>& properties,
    void* data) {
  napi_value value;
  napi_status status = napi_define_class(
    env, utf8name, T::ConstructorCallbackWrapper, data, properties.size(),
    reinterpret_cast<const napi_property_descriptor*>(properties.begin()), &value);
  if (status != napi_ok) throw Error::New(env);

  return Function(env, value);
}

template <typename T>
inline Function ObjectWrap<T>::DefineClass(
    Napi::Env env,
    const char* utf8name,
    const std::vector<ClassPropertyDescriptor<T>>& properties,
    void* data) {
  napi_value value;
  napi_status status = napi_define_class(
    env, utf8name, T::ConstructorCallbackWrapper, data, properties.size(),
    reinterpret_cast<const napi_property_descriptor*>(properties.data()), &value);
  if (status != napi_ok) throw Error::New(env);

  return Function(env, value);
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::StaticMethod(
    const char* utf8name,
    StaticVoidMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  CallbackData* callbackData = new CallbackData({}); // TODO: Delete when the class is destroyed
  callbackData->staticVoidMethodCallback = method;
  callbackData->data = data;

  napi_property_descriptor desc = {};
  desc.utf8name = utf8name;
  desc.method = T::StaticVoidMethodCallbackWrapper;
  desc.data = callbackData;
  desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static_property);
  return desc;
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::StaticMethod(
    const char* utf8name,
    StaticMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  CallbackData* callbackData = new CallbackData({}); // TODO: Delete when the class is destroyed
  callbackData->staticMethodCallback = method;
  callbackData->data = data;

  napi_property_descriptor desc = {};
  desc.utf8name = utf8name;
  desc.method = T::StaticMethodCallbackWrapper;
  desc.data = callbackData;
  desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static_property);
  return desc;
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::StaticAccessor(
    const char* utf8name,
    StaticGetterCallback getter,
    StaticSetterCallback setter,
    napi_property_attributes attributes,
    void* data) {
  CallbackData* callbackData = new CallbackData({}); // TODO: Delete when the class is destroyed
  callbackData->staticGetterCallback = getter;
  callbackData->staticSetterCallback = setter;
  callbackData->data = data;

  napi_property_descriptor desc = {};
  desc.utf8name = utf8name;
  desc.getter = getter != nullptr ? T::StaticGetterCallbackWrapper : nullptr;
  desc.setter = setter != nullptr ? T::StaticSetterCallbackWrapper : nullptr;
  desc.data = callbackData;
  desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static_property);
  return desc;
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::InstanceMethod(
    const char* utf8name,
    InstanceVoidMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  CallbackData* callbackData = new CallbackData({}); // TODO: Delete when the class is destroyed
  callbackData->instanceVoidMethodCallback = method;
  callbackData->data = data;

  napi_property_descriptor desc = {};
  desc.utf8name = utf8name;
  desc.method = T::InstanceVoidMethodCallbackWrapper;
  desc.data = callbackData;
  desc.attributes = attributes;
  return desc;
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::InstanceMethod(
    const char* utf8name,
    InstanceMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  CallbackData* callbackData = new CallbackData({}); // TODO: Delete when the class is destroyed
  callbackData->instanceMethodCallback = method;
  callbackData->data = data;

  napi_property_descriptor desc = {};
  desc.utf8name = utf8name;
  desc.method = T::InstanceMethodCallbackWrapper;
  desc.data = callbackData;
  desc.attributes = attributes;
  return desc;
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::InstanceAccessor(
    const char* utf8name,
    InstanceGetterCallback getter,
    InstanceSetterCallback setter,
    napi_property_attributes attributes,
    void* data) {
  CallbackData* callbackData = new CallbackData({}); // TODO: Delete when the class is destroyed
  callbackData->instanceGetterCallback = getter;
  callbackData->instanceSetterCallback = setter;
  callbackData->data = data;

  napi_property_descriptor desc = {};
  desc.utf8name = utf8name;
  desc.getter = T::InstanceGetterCallbackWrapper;
  desc.setter = T::InstanceSetterCallbackWrapper;
  desc.data = callbackData;
  desc.attributes = attributes;
  return desc;
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::StaticValue(const char* utf8name,
    Napi::Value value, napi_property_attributes attributes) {
  napi_property_descriptor desc = {};
  desc.utf8name = utf8name;
  desc.value = value;
  desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static_property);
  return desc;
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::InstanceValue(
    const char* utf8name,
    Napi::Value value,
    napi_property_attributes attributes) {
  napi_property_descriptor desc = {};
  desc.utf8name = utf8name;
  desc.value = value;
  desc.attributes = attributes;
  return desc;
}

template <typename T>
inline void ObjectWrap<T>::ConstructorCallbackWrapper(
    napi_env env,
    napi_callback_info info) {
  bool isConstructCall;
  napi_status status = napi_is_construct_call(env, info, &isConstructCall);
  if (status != napi_ok) return;

  if (!isConstructCall) {
    napi_throw_type_error(env, "Class constructors cannot be invoked without 'new'");
    return;
  }

  T* instance;
  napi_value wrapper;
  try {
    CallbackInfo callbackInfo(env, info);
    instance = new T(callbackInfo);
    wrapper = callbackInfo.This();
  }
  catch (const Error& e) {
    if (!Napi::Env(env).IsExceptionPending()) {
      e.ThrowAsJavaScriptException();
    }
    return;
  }

  napi_ref ref;
  status = napi_wrap(env, wrapper, instance, FinalizeCallback, nullptr, &ref);
  if (status != napi_ok) return;

  Reference<Object>* instanceRef = instance;
  *instanceRef = Reference<Object>(env, ref);

  status = napi_set_return_value(env, info, wrapper);
  if (status != napi_ok) return;
}

template <typename T>
inline void ObjectWrap<T>::StaticVoidMethodCallbackWrapper(
    napi_env env,
    napi_callback_info info) {
  try {
    CallbackInfo callbackInfo(env, info);
    CallbackData* callbackData = reinterpret_cast<CallbackData*>(callbackInfo.Data());
    callbackInfo.SetData(callbackData->data);
    callbackData->staticVoidMethodCallback(callbackInfo);
  }
  catch (const Error& e) {
    if (!Napi::Env(env).IsExceptionPending()) {
      e.ThrowAsJavaScriptException();
    }
    return;
  }
}

template <typename T>
inline void ObjectWrap<T>::StaticMethodCallbackWrapper(
    napi_env env,
    napi_callback_info info) {
  napi_value result;
  try {
    CallbackInfo callbackInfo(env, info);
    CallbackData* callbackData = reinterpret_cast<CallbackData*>(callbackInfo.Data());
    callbackInfo.SetData(callbackData->data);
    result = callbackData->staticMethodCallback(callbackInfo);
  }
  catch (const Error& e) {
    if (!Napi::Env(env).IsExceptionPending()) {
      e.ThrowAsJavaScriptException();
    }
    return;
  }

  napi_status status = napi_set_return_value(env, info, result);
  if (status != napi_ok) return;
}

template <typename T>
inline void ObjectWrap<T>::StaticGetterCallbackWrapper(
    napi_env env,
    napi_callback_info info) {
  napi_value result;
  try {
    CallbackInfo callbackInfo(env, info);
    CallbackData* callbackData = reinterpret_cast<CallbackData*>(callbackInfo.Data());
    callbackInfo.SetData(callbackData->data);
    result = callbackData->staticGetterCallback(callbackInfo);
  }
  catch (const Error& e) {
    if (!Napi::Env(env).IsExceptionPending()) {
      e.ThrowAsJavaScriptException();
    }
    return;
  }

  napi_status status = napi_set_return_value(env, info, result);
  if (status != napi_ok) return;
}

template <typename T>
inline void ObjectWrap<T>::StaticSetterCallbackWrapper(
    napi_env env,
    napi_callback_info info) {
  try {
    CallbackInfo callbackInfo(env, info);
    CallbackData* callbackData = reinterpret_cast<CallbackData*>(callbackInfo.Data());
    callbackInfo.SetData(callbackData->data);
    callbackData->staticSetterCallback(callbackInfo, callbackInfo[0]);
  }
  catch (const Error& e) {
    if (!Napi::Env(env).IsExceptionPending()) {
      e.ThrowAsJavaScriptException();
    }
    return;
  }
}

template <typename T>
inline void ObjectWrap<T>::InstanceVoidMethodCallbackWrapper(
    napi_env env,
    napi_callback_info info) {
  try {
    CallbackInfo callbackInfo(env, info);
    CallbackData* callbackData = reinterpret_cast<CallbackData*>(callbackInfo.Data());
    callbackInfo.SetData(callbackData->data);
    T* instance = Unwrap(callbackInfo.This());
    auto cb = callbackData->instanceVoidMethodCallback;
    (instance->*cb)(callbackInfo);
  }
  catch (const Error& e) {
    if (!Napi::Env(env).IsExceptionPending()) {
      e.ThrowAsJavaScriptException();
    }
    return;
  }
}

template <typename T>
inline void ObjectWrap<T>::InstanceMethodCallbackWrapper(
    napi_env env,
    napi_callback_info info) {
  napi_value result;
  try {
    CallbackInfo callbackInfo(env, info);
    CallbackData* callbackData = reinterpret_cast<CallbackData*>(callbackInfo.Data());
    callbackInfo.SetData(callbackData->data);
    T* instance = Unwrap(callbackInfo.This());
    auto cb = callbackData->instanceMethodCallback;
    result = (instance->*cb)(callbackInfo);
  }
  catch (const Error& e) {
    if (!Napi::Env(env).IsExceptionPending()) {
      e.ThrowAsJavaScriptException();
    }
    return;
  }

  napi_status status = napi_set_return_value(env, info, result);
  if (status != napi_ok) return;
}

template <typename T>
inline void ObjectWrap<T>::InstanceGetterCallbackWrapper(
    napi_env env,
    napi_callback_info info) {
  napi_value result;
  try {
    CallbackInfo callbackInfo(env, info);
    CallbackData* callbackData = reinterpret_cast<CallbackData*>(callbackInfo.Data());
    callbackInfo.SetData(callbackData->data);
    T* instance = Unwrap(callbackInfo.This());
    auto cb = callbackData->instanceGetterCallback;
    result = (instance->*cb)(callbackInfo);
  }
  catch (const Error& e) {
    if (!Napi::Env(env).IsExceptionPending()) {
      e.ThrowAsJavaScriptException();
    }
    return;
  }

  napi_status status = napi_set_return_value(env, info, result);
  if (status != napi_ok) return;
}

template <typename T>
inline void ObjectWrap<T>::InstanceSetterCallbackWrapper(
    napi_env env,
    napi_callback_info info) {
  try {
    CallbackInfo callbackInfo(env, info);
    CallbackData* callbackData = reinterpret_cast<CallbackData*>(callbackInfo.Data());
    callbackInfo.SetData(callbackData->data);
    T* instance = Unwrap(callbackInfo.This());
    auto cb = callbackData->instanceSetterCallback;
    (instance->*cb)(callbackInfo, callbackInfo[0]);
  }
  catch (const Error& e) {
    if (!Napi::Env(env).IsExceptionPending()) {
      e.ThrowAsJavaScriptException();
    }
    return;
  }
}

template <typename T>
inline void ObjectWrap<T>::FinalizeCallback(void* data, void* /*hint*/) {
  T* instance = reinterpret_cast<T*>(data);
  delete instance;
}

////////////////////////////////////////////////////////////////////////////////
// HandleScope class
////////////////////////////////////////////////////////////////////////////////

inline HandleScope::HandleScope(napi_env env, napi_handle_scope scope)
    : _env(env), _scope(scope) {
}

inline HandleScope::HandleScope(Napi::Env env) : _env(env) {
  napi_status status = napi_open_handle_scope(_env, &_scope);
  if (status != napi_ok) throw Error::New(_env);
}

inline HandleScope::~HandleScope() {
  napi_close_handle_scope(_env, _scope);
}

inline HandleScope::operator napi_handle_scope() const {
  return _scope;
}

inline Env HandleScope::Env() const {
  return Napi::Env(_env);
}

////////////////////////////////////////////////////////////////////////////////
// EscapableHandleScope class
////////////////////////////////////////////////////////////////////////////////

inline EscapableHandleScope::EscapableHandleScope(
  napi_env env, napi_escapable_handle_scope scope) : _env(env), _scope(scope) {
}

inline EscapableHandleScope::EscapableHandleScope(Napi::Env env) : _env(env) {
  napi_status status = napi_open_escapable_handle_scope(_env, &_scope);
  if (status != napi_ok) throw Error::New(_env);
}

inline EscapableHandleScope::~EscapableHandleScope() {
  napi_close_escapable_handle_scope(_env, _scope);
}

inline EscapableHandleScope::operator napi_escapable_handle_scope() const {
  return _scope;
}

inline Env EscapableHandleScope::Env() const {
  return Napi::Env(_env);
}

inline Value EscapableHandleScope::Escape(napi_value escapee) {
  napi_value result;
  napi_status status = napi_escape_handle(_env, _scope, escapee, &result);
  if (status != napi_ok) throw Error::New(_env);
  return Value(_env, result);
}

////////////////////////////////////////////////////////////////////////////////
// AsyncWorker class
////////////////////////////////////////////////////////////////////////////////

inline AsyncWorker::AsyncWorker(const Function& callback)
  : _callback(Napi::Persistent(callback)),
    _persistent(Napi::Persistent(Object::New(callback.Env()))),
    _env(callback.Env()) {
  _work = napi_create_async_work();
}

inline AsyncWorker::~AsyncWorker() {
  if (_work != nullptr) {
    napi_delete_async_work(_work);
    _work = nullptr;
  }
}

inline AsyncWorker::AsyncWorker(AsyncWorker&& other) {
  _env = other._env;
  other._env = nullptr;
  _work = other._work;
  other._work = nullptr;
  _persistent = std::move(other._persistent);
  _errmsg = std::move(other._errmsg);
}

inline AsyncWorker& AsyncWorker::operator =(AsyncWorker&& other) {
  _env = other._env;
  other._env = nullptr;
  _work = other._work;
  other._work = nullptr;
  _persistent = std::move(other._persistent);
  _errmsg = std::move(other._errmsg);
  return *this;
}

inline AsyncWorker::operator napi_work() const {
  return _work;
}

inline Env AsyncWorker::Env() const {
  return Napi::Env(_env);
}

inline void AsyncWorker::Queue() {
  napi_async_set_data(_work, static_cast<void*>(this));
  napi_async_set_execute(_work, OnExecute);
  napi_async_set_complete(_work, OnWorkComplete);
  napi_async_set_destroy(_work, OnDestroy);
  napi_async_queue_worker(_work);
}

inline void AsyncWorker::WorkComplete() {
  HandleScope scope(_env);
  if (_errmsg.size() == 0) {
    OnOK();
  }
  else {
    OnError();
  }
}

inline ObjectReference& AsyncWorker::Persistent() {
  return _persistent;
}

inline void AsyncWorker::OnOK() {
  _callback.MakeCallback(Env().Global(), std::vector<napi_value>());
}

inline void AsyncWorker::OnError() {
  _callback.MakeCallback(Env().Global(), std::vector<napi_value>({
    Error::New(Env(), _errmsg),
  }));
}

inline void AsyncWorker::SetErrorMessage(const std::string& msg) {
  _errmsg = msg;
}

inline const std::string& AsyncWorker::ErrorMessage() const {
  return _errmsg;
}

inline void AsyncWorker::OnExecute(void* this_pointer) {
  AsyncWorker* self = static_cast<AsyncWorker*>(this_pointer);
  self->Execute();
}

inline void AsyncWorker::OnWorkComplete(void* this_pointer) {
  AsyncWorker* self = static_cast<AsyncWorker*>(this_pointer);
  self->WorkComplete();
}

inline void AsyncWorker::OnDestroy(void* this_pointer) {
  AsyncWorker* self = static_cast<AsyncWorker*>(this_pointer);
  delete self;
}

} // namespace Napi

#endif // SRC_NAPI_INL_H_
