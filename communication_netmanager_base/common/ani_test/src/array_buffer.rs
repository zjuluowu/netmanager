// Copyright (C) 2025 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

use ani_rs::{business_error::BusinessError, typed_array::*};

#[ani_rs::native]
pub fn array_buffer_test(input: ArrayBuffer) -> Result<ArrayBuffer, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn change_array_buffer(mut input: ArrayBuffer) -> Result<ArrayBuffer, BusinessError> {
    let data = input.as_mut();
    for item in data {
        *item += 1;
    }
    Ok(input)
}

#[ani_rs::native]
pub fn create_array_buffer() -> Result<ArrayBuffer, BusinessError> {
    let data = vec![1, 2, 3, 4];
    let output = ArrayBuffer::new_with_vec(data);
    Ok(output)
}

#[ani_rs::native]
pub fn int8_array_test(input: Int8Array) -> Result<Int8Array, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn change_int8_array(mut input: Int8Array) -> Result<Int8Array, BusinessError> {
    let data = input.as_mut();
    for item in data {
        *item += 1;
    }
    Ok(input)
}

#[ani_rs::native]
pub fn create_int8_array(input: Int8Array) -> Result<Int8Array, BusinessError> {
    let data = input.to_vec();
    let output = Int8Array::new_with_vec(data);
    Ok(output)
}

#[ani_rs::native]
pub fn uint8_array_test(input: Uint8Array) -> Result<Uint8Array, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn change_uint8_array(mut input: Uint8Array) -> Result<Uint8Array, BusinessError> {
    let data = input.as_mut();
    for item in data {
        *item += 1;
    }
    Ok(input)
}

#[ani_rs::native]
pub fn create_uint8_array(input: Uint8Array) -> Result<Uint8Array, BusinessError> {
    let data = input.to_vec();
    let output = Uint8Array::new_with_vec(data);
    Ok(output)
}

#[ani_rs::native]
pub fn int16_array_test(input: Int16Array) -> Result<Int16Array, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn change_int16_array(mut input: Int16Array) -> Result<Int16Array, BusinessError> {
    let data = input.as_mut();
    for item in data {
        *item += 1;
    }
    Ok(input)
}

#[ani_rs::native]
pub fn create_int16_array(input: Int16Array) -> Result<Int16Array, BusinessError> {
    let data = input.to_vec();
    let output = Int16Array::new_with_vec(data);
    Ok(output)
}

#[ani_rs::native]
pub fn uint16_array_test(input: Uint16Array) -> Result<Uint16Array, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn change_uint16_array(mut input: Uint16Array) -> Result<Uint16Array, BusinessError> {
    let data = input.as_mut();
    for item in data {
        *item += 1;
    }
    Ok(input)
}

#[ani_rs::native]
pub fn create_uint16_array(input: Uint16Array) -> Result<Uint16Array, BusinessError> {
    let data = input.to_vec();
    let output = Uint16Array::new_with_vec(data);
    Ok(output)
}

#[ani_rs::native]
pub fn int32_array_test(input: Int32Array) -> Result<Int32Array, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn change_int32_array(mut input: Int32Array) -> Result<Int32Array, BusinessError> {
    let data = input.as_mut();
    for item in data {
        *item += 1;
    }
    Ok(input)
}

#[ani_rs::native]
pub fn create_int32_array(input: Int32Array) -> Result<Int32Array, BusinessError> {
    let data = input.to_vec();
    let output = Int32Array::new_with_vec(data);
    Ok(output)
}

#[ani_rs::native]
pub fn uint32_array_test(input: Uint32Array) -> Result<Uint32Array, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn change_uint32_array(mut input: Uint32Array) -> Result<Uint32Array, BusinessError> {
    let data = input.as_mut();
    for item in data {
        *item += 1;
    }
    Ok(input)
}

#[ani_rs::native]
pub fn create_uint32_array(input: Uint32Array) -> Result<Uint32Array, BusinessError> {
    let data = input.to_vec();
    let output = Uint32Array::new_with_vec(data);
    Ok(output)
}

#[ani_rs::ani(path = "anirs.test.ani_test.ArrayBufferStruct")]
pub struct ArrayBufferStruct {
    pub buffer1: ArrayBuffer,
    pub buffer2: Int32Array,
}

#[ani_rs::native]
pub fn array_buffer_strcut_test(input: ArrayBufferStruct) -> Result<ArrayBufferStruct, BusinessError> {
    Ok(input)
}