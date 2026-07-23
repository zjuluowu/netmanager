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

use ani_rs::{
    business_error::BusinessError, global::GlobalRef, objects::{AniAsyncCallback, AniFnObject, AniObject, AniRef, JsonValue}, signature, typed_array::ArrayBuffer, AniEnv
};

#[ani_rs::native]
pub fn json_ser_deser_test(json: JsonValue) -> Result<JsonValue, BusinessError> {
    Ok(json)
}

#[ani_rs::native]
pub fn json_stringify_test1(env: &AniEnv, json: JsonValue) -> Result<String, BusinessError> {
    let res = json.stringify(env)?;
    Ok(res)
}

#[ani_rs::native]
pub fn json_parse_test1<'local>(
    env: &AniEnv<'local>,
    data: String,
) -> Result<JsonValue<'local>, BusinessError> {
    let obj = JsonValue::parse(env, &data)?;
    Ok(obj)
}

#[ani_rs::native]
pub fn execute_json_callback1(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let data = String::from(r#"{"x":1,"y":2}"#);
    let argv = JsonValue::parse(env, &data)?;
    callback.execute_local(env, (argv,))?;
    Ok(())
}

#[ani_rs::native]
pub fn execute_json_callback2(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let global_callback = callback
        .into_global_callback::<(GlobalRef<JsonValue<'static>>,)>(env)?;

    let data = String::from(r#"{"x":1,"y":2}"#);
    let argv = JsonValue::parse(env, &data)?;
    let global_argv = argv.into_global(env)?;
    global_callback.execute_spawn_thread((global_argv,));
    Ok(())
}

#[ani_rs::ani(path = "anirs.test.ani_test.HttpDataType")]
#[derive(Debug)]
pub enum HttpDataType {
    String,
    Object = 1,
    ArrayBuffer = 2,
}

#[ani_rs::ani]
pub struct HttpRequestOptions<'local> {
    pub extra_data: Option<AniObject<'local>>,
    pub expect_data_type: Option<HttpDataType>,
}

#[ani_rs::native]
pub fn json_request_test(
    env: &AniEnv,
    options: HttpRequestOptions,
) -> Result<String, BusinessError> {
    if options.expect_data_type.is_none() || options.extra_data.is_none() {
        return Err(BusinessError::PARAMETER);
    }
    let _data_type = options.expect_data_type.unwrap();
    let obj_data = options.extra_data.unwrap();

    let string_class = env.find_class(signature::STRING)?;
    let array_buffer_class = env.find_class(signature::ARRAY_BUFFER)?;

    let res = if env.instance_of(&obj_data, &string_class)? {
        let res = env.deserialize::<String>(obj_data)?;
        res
    } else if env.instance_of(&obj_data, &array_buffer_class)? {
        let buffer = env.deserialize::<ArrayBuffer>(obj_data)?;
        let res = buffer.as_ref();
        String::from_utf8_lossy(res).to_string()
    } else {
        let json_value = env.deserialize::<JsonValue>(obj_data)?;
        let res = json_value.stringify(env)?;
        res
    };
    Ok(res)
}

#[ani_rs::ani(path = "anirs.test.ani_test.HttpResponse", output = "only")]
pub struct HttpResponse {
    pub result: GlobalRef<AniRef<'static>>,
    pub result_type: HttpDataType,
}

impl HttpResponse {
    pub fn new(result: GlobalRef<AniRef<'static>>, result_type: HttpDataType) -> Self {
        Self {
            result,
            result_type,
        }
    }
}

#[ani_rs::native]
pub fn json_response_test1(
    env: &AniEnv,
    async_callback: AniAsyncCallback,
    test_case: i32,
) -> Result<(), BusinessError> {
    if test_case == 0 {
        let s = String::from("hello world");
        let s_ref = env.serialize(&s)?.into_global(env)?;
        let response = HttpResponse::new(s_ref, HttpDataType::String);
        async_callback.execute_local(env, None, (response,))?;
    } else if test_case == 1 {
        let data =
            String::from(r#"{"x":{"xx":{"xxx1":1,"xxx2":2}},"y":{"yy":{"yyy1":3,"yyy2":4}}}"#);
        let json_value = JsonValue::parse(env, &data)?;
        let json_global = AniRef::from(json_value).into_global(env)?;
        let response = HttpResponse::new(json_global, HttpDataType::Object);
        async_callback.execute_local(env, None, (response,))?;
    } else {
        let data = ArrayBuffer::new_with_vec(vec![48, 49, 50]);
        let buffer_global = env.serialize(&data)?.into_global(env)?;
        let response = HttpResponse::new(buffer_global, HttpDataType::ArrayBuffer);
        async_callback.execute_local(env, None, (response,))?;
    }

    Ok(())
}

#[ani_rs::native]
pub fn json_response_test2(
    env: &AniEnv,
    async_callback: AniAsyncCallback,
    test_case: i32,
) -> Result<(), BusinessError> {
    let global_callback = async_callback
        .into_global_callback::<(HttpResponse,)>(env)?;

    if test_case == 0 {
        let s = String::from("hello world");
        let s_ref = env.serialize(&s)?.into_global(env)?;
        let response = HttpResponse::new(s_ref, HttpDataType::String);
        global_callback.execute(None, (response,));
    } else if test_case == 1 {
        let data =
            String::from(r#"{"x":{"xx":{"xxx1":1,"xxx2":2}},"y":{"yy":{"yyy1":3,"yyy2":4}}}"#);
        let json_value = JsonValue::parse(env, &data)?;
        let json_global = AniRef::from(json_value).into_global(env)?;
        let response = HttpResponse::new(json_global, HttpDataType::Object);
        global_callback.execute(None, (response,));
    } else {
        let data = ArrayBuffer::new_with_vec(vec![48, 49, 50]);
        let buffer_global = env.serialize(&data)?.into_global(env)?;
        let response = HttpResponse::new(buffer_global, HttpDataType::ArrayBuffer);
        global_callback.execute(None, (response,));
    }

    Ok(())
}
