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

#[ani_rs::ani(path = "anirs.test.ani_test.EnumNumber")]
#[derive(Debug)]
enum EnumNumber {
    One = 1,
    Two = 2,
    Three = 3,
}

#[ani_rs::ani(path = "anirs.test.ani_test.EnumString")]
#[derive(Debug)]
enum EnumString {
    One = 1,
    Two = 2,
    Three = 3,
}

#[ani_rs::native]
pub fn enum_test_number<'local>(input: EnumNumber) -> Result<EnumNumber, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn enum_test_string<'local>(input: EnumString) -> Result<EnumString, BusinessError> {
    Ok(input)
}

#[ani_rs::ani(path = "anirs.test.ani_test.ResponseCode")]
#[derive(Debug)]
pub struct ResponseCode {
    code: i32,
    url: String,
}

#[ani_rs::ani(path = "anirs.test.ani_test.HttpProtocol")]
#[derive(Debug)]
pub enum HttpProtocol {
    Http1_1,
    Http2,
    Http3,
}

#[derive(serde::Serialize, serde::Deserialize)]
#[derive(Debug)]
pub enum ResponseCodeOutput {
    #[serde(rename = "anirs.test.ani_test.ResponseCode")]
    Code(ResponseCode),
    #[serde(rename = "anirs.test.ani_test.HttpProtocol")]
    Proto(HttpProtocol),
    I32(i32),
}

#[ani_rs::native]
pub fn enum_test_struct(input: ResponseCodeOutput) -> Result<ResponseCodeOutput, BusinessError> {
    Ok(input)
}