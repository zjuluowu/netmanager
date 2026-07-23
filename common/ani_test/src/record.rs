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

use ani_rs::business_error::BusinessError;

use crate::ani_enum::ResponseCode;

#[ani_rs::native]
pub fn record_string<'local>(
    input: HashMap<String, String>,
) -> Result<HashMap<String, String>, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn record_long<'local>(input: HashMap<i64, i64>) -> Result<HashMap<i64, i64>, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn record_struct(input: HashMap<i32, ResponseCode>) -> Result<HashMap<i32, ResponseCode>, BusinessError> {
    Ok(input)
}
