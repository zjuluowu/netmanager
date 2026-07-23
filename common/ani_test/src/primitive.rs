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

use ani_rs::objects::AniObject;
use ani_rs::{business_error::BusinessError, objects::AniRef};
use ani_rs::AniEnv;

#[ani_rs::ani(path = "anirs.test.ani_test.PrimitiveTest")]
struct PrimitiveTest {
    primitive_bool: bool,
    primitive_i8: i8,
    primitive_i16: i16,
    primitive_i32: i32,
    primitive_i64: i64,
    primitive_f32: f32,
    primitive_f64: f64,
}

#[ani_rs::native]
pub fn primitive_test<'local>(input: PrimitiveTest) -> Result<PrimitiveTest, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn aniref_deserialize_test<'local>(env: &AniEnv, input: AniRef<'local>) -> Result<bool, BusinessError> {
    let ani_obj = AniObject::from(input);
    let raw1 = ani_obj.as_raw() as i64;
    let res: AniObject<'_> = env.deserialize(ani_obj).unwrap();
    let raw2 = res.as_raw() as i64;
    Ok(raw1 == raw2)
}

#[ani_rs::ani(path = "anirs.test.ani_test.AniRefStruct")]
#[derive(Debug)]
struct AniRefStruct<'local> {
    ani_obj_string: AniObject<'local>,
    ani_string: String,
}

#[ani_rs::native]
pub fn aniref_struct_de_test<'local>(env: &AniEnv, input: AniRefStruct<'local>) -> Result<bool, BusinessError> {
    let s: String = env.deserialize(input.ani_obj_string).unwrap();
    Ok(s == input.ani_string)
}

#[ani_rs::native]
pub fn aniref_array_de_test<'local>(env: &AniEnv, input: Vec<AniObject<'local>>) -> Result<Vec<String>, BusinessError> {
    let res: Vec<String> = input.into_iter().map(|str_obj| {
        let s: String = env.deserialize(str_obj).unwrap();
        s
    }).collect();
    
    Ok(res)
}

#[ani_rs::native]
pub fn aniref_serialize_test<'local>(env: &AniEnv, input: AniRef<'local>) -> Result<bool, BusinessError> {
    let raw1 = input.as_raw() as i64;
    let res = env.serialize(&input).unwrap();
    let raw2 = res.as_raw() as i64;
    let _s: String = env.deserialize(AniObject::from(res)).unwrap();
    Ok(raw1 == raw2)
}

#[ani_rs::native]
pub fn aniref_struct_test<'local>(input: AniRefStruct<'local>) -> Result<AniRefStruct<'local>, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn aniref_array_test<'local>(input: Vec<AniObject<'local>>) -> Result<Vec<AniObject<'local>>, BusinessError> {
    Ok(input)
}

#[ani_rs::native]
pub fn return_ani_ref_test<'local>(env: &AniEnv<'local>) -> Result<AniRef<'local>, BusinessError> {
    let res = String::from("hello world");
    let res_ref = env.serialize(&res).unwrap();
    Ok(res_ref)
}
