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

use ani_rs::business_error::BusinessError;

#[ani_rs::native]
pub fn array_bool(input: Vec<bool>) -> Result<Vec<bool>, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn array_byte(input: Vec<i8>) -> Result<Vec<i8>, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn array_i16(input: Vec<i16>) -> Result<Vec<i16>, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn array_i32(input: Vec<i32>) -> Result<Vec<i32>, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn array_i64(input: Vec<i64>) -> Result<Vec<i64>, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn array_f32(input: Vec<f32>) -> Result<Vec<f32>, BusinessError> {
    Ok(input)
}
#[ani_rs::native]
pub fn array_f64(input: Vec<f64>) -> Result<Vec<f64>, BusinessError> {
    Ok(input)
}
