// Copyright (C) 2025 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "license");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "aS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

mod ani_enum;
mod ani_union;
mod array;
mod array_buffer;
mod option;
mod primitive;
mod record;
mod ani_struct;
mod ani_callback;
mod business_error;
mod ani_json;

ani_rs::ani_constructor!(
    namespace "anirs.test.ani_test"
    [
        "primitiveTest" : primitive::primitive_test,
        "anirefDeserializeTest": primitive::aniref_deserialize_test,
        "anirefStructDeTest": primitive::aniref_struct_de_test,
        "anirefArrayDeTest": primitive::aniref_array_de_test,
        "anirefSerializeTest": primitive::aniref_serialize_test,
        "anirefStructTest": primitive::aniref_struct_test,
        "anirefArrayTest": primitive::aniref_array_test,
        "returnAniRefTest": primitive::return_ani_ref_test,
        "optionBool" : option::option_bool,
        "optionByte" : option::option_byte,
        "optionShort" : option::option_i16,
        "optionInt" : option::option_i32,
        "optionLong" : option::option_i64,
        "optionDouble" : option::option_f64,
        "arrayBool":  array::array_bool,
        "arrayByte" : array::array_byte,
        "arrayShort" :  array::array_i16,
        "arrayInt":  array::array_i32,
        "arrayLong":  array::array_i64,
        "arrayFloat":  array::array_f32,
        "arrayDouble" :  array::array_f64,
        "enumTestNumber": ani_enum::enum_test_number,
        "enumTestString": ani_enum::enum_test_string,
        "recordString" : record::record_string,
        "recordLong" : record::record_long,
        "recordStruct": record::record_struct,
        "unionTest" : ani_union::union_test,
        "unionTest2" : ani_union::union_test2,
        "arrayBufferTest": array_buffer::array_buffer_test,
        "changeArrayBuffer": array_buffer::change_array_buffer,
        "createArrayBuffer": array_buffer::create_array_buffer,
        "uint8ArrayTest": array_buffer::uint8_array_test,
        "int8ArrayTest": array_buffer::int8_array_test,
        "uint16ArrayTest": array_buffer::uint16_array_test,
        "int16ArrayTest": array_buffer::int16_array_test,
        "uint32ArrayTest": array_buffer::uint32_array_test,
        "int32ArrayTest": array_buffer::int32_array_test,
        "changeUint8Array": array_buffer::change_uint8_array,
        "changeInt8Array": array_buffer::change_int8_array,
        "changeUint16Array": array_buffer::change_uint16_array,
        "changeInt16Array": array_buffer::change_int16_array,
        "changeUint32Array": array_buffer::change_uint32_array,
        "changeInt32Array": array_buffer::change_int32_array,
        "createInt8Array": array_buffer::create_int8_array,
        "createUint8Array": array_buffer::create_uint8_array,
        "createInt16Array": array_buffer::create_int16_array,
        "createUint16Array": array_buffer::create_uint16_array,
        "createInt32Array": array_buffer::create_int32_array,
        "createUint32Array": array_buffer::create_uint32_array,
        "arrayBufferStrcutTest": array_buffer::array_buffer_strcut_test,
        "structEnum": ani_struct::struct_enum,
        "enumTestStruct": ani_enum::enum_test_struct,
        "executeCallback1": ani_callback::execute_callback1,
        "executeCallback2": ani_callback::execute_callback2,
        "executeCallback3": ani_callback::execute_callback3,
        "executeCallback4": ani_callback::execute_callback4,
        "executeCallback5": ani_callback::execute_callback5,
        "executeCallback6": ani_callback::execute_callback6,
        "executeAsyncCallback1": ani_callback::execute_async_callback1,
        "executeAsyncCallback2": ani_callback::execute_async_callback2,
        "executeAsyncCallback3": ani_callback::execute_async_callback3,
        "executeAsyncCallback4": ani_callback::execute_async_callback4,
        "executeErrorCallback1": ani_callback::execute_error_callback1,
        "executeErrorCallback2": ani_callback::execute_error_callback2,
        "executeErrorCallback3": ani_callback::execute_error_callback3,
        "executeErrorCallback4": ani_callback::execute_error_callback4,
        "executeAniRefCallback1": ani_callback::execute_ani_ref_callback1,
        "executeAniRefCallback2": ani_callback::execute_ani_ref_callback2,
        "executeAniRefCallback3": ani_callback::execute_ani_ref_callback3,
        "executeAniRefCallback4": ani_callback::execute_ani_ref_callback4,
        "executeMultiCallbacks": ani_callback::execute_multi_callbacks,
        "executeThrowErrorCallback1": ani_callback::execute_throw_error_callback1,
        "executeThrowErrorCallback2": ani_callback::execute_throw_error_callback2,
        "sendEventTest1": ani_callback::send_event_test1,
        "sendEventTest2": ani_callback::send_event_test2,
        "businessErrorTest": business_error::business_error_test,
        "jsonSerDeserTest": ani_json::json_ser_deser_test,
        "jsonStringifyTest1": ani_json::json_stringify_test1,
        "jsonParseTest1": ani_json::json_parse_test1,
        "executeJsonCallback1": ani_json::execute_json_callback1,
        "executeJsonCallback2": ani_json::execute_json_callback2,
        "jsonRequestTest": ani_json::json_request_test,
        "jsonResponseTest1": ani_json::json_response_test1,
        "jsonResponseTest2": ani_json::json_response_test2,
    ]
);
