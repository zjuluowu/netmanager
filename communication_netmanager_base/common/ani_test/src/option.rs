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
pub fn option_bool(input: Option<bool>) -> Result<Option<bool>, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn option_byte(input: Option<i8>) -> Result<Option<i8>, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn option_i16(input: Option<i16>) -> Result<Option<i16>, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn option_i32(input: Option<i32>) -> Result<Option<i32>, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn option_i64(input: Option<i64>) -> Result<Option<i64>, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn option_f64(input: Option<f64>) -> Result<Option<f64>, BusinessError> {
    Ok(input)
}
