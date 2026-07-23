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

use std::collections::HashMap;

use ani_rs::{business_error::BusinessError, typed_array::*};
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize)]
enum Data {
    Boolean(bool),
    I8(i8),
    I16(i16),
    I32(i32),
    I64(i64),
    F64(f64),
    S(String),
    ArrayBuffer(ArrayBuffer),
    Null(()),
    Record(HashMap<String, String>),
    Array(Vec<String>),
    Int8Array(Int8Array),
    Uint8Array(Uint8Array),
    Int16Array(Int16Array),
    Uint16Array(Uint16Array),
    Int32Array(Int32Array),
    Uint32Array(Uint32Array),
}

#[ani_rs::native]
pub fn union_test(input: Data) -> Result<Data, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn union_test2(input: Option<Data>) -> Result<Option<Data>, BusinessError> {
    Ok(input)
}