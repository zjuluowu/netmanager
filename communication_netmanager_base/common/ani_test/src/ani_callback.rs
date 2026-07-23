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

use std::thread;

use ani_rs::{
    business_error::BusinessError,
    global::GlobalRef,
    objects::{AniAsyncCallback, AniErrorCallback, AniFnObject, AniObject, AniRef},
    typed_array::{ArrayBuffer, Uint32Array},
    AniEnv, AniVm,
};

#[ani_rs::native]
pub fn execute_callback1(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    callback.execute_local(env, (1,)).unwrap();
    Ok(())
}

#[ani_rs::native]
pub fn execute_callback2(callback: AniFnObject) -> Result<(), BusinessError> {
    callback.execute_current((2,)).unwrap();
    Ok(())
}

#[ani_rs::native]
pub fn execute_callback3(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let global_callback = callback.into_global_callback::<(i32,)>(env).unwrap();
    global_callback.execute_spawn_thread((3,));
    Ok(())
}

#[ani_rs::native]
pub fn execute_callback4(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let global_callback = callback.into_global_callback::<(i32,)>(env).unwrap();

    thread::spawn(move || {
        global_callback.execute((4,));
    });

    Ok(())
}

#[ani_rs::native]
pub fn execute_callback5(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let global_callback = callback
        .into_global_callback::<(ArrayBuffer,)>(env)
        .unwrap();

    thread::spawn(move || {
        let buff = ArrayBuffer::new_with_vec(vec![1, 2, 3]);
        global_callback.execute_spawn_thread((buff,));
    });

    Ok(())
}

#[ani_rs::native]
pub fn execute_callback6(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let global_callback = callback
        .into_global_callback::<(Uint32Array,)>(env)
        .unwrap();

    thread::spawn(move || {
        let buff = Uint32Array::new_with_vec(vec![10, 20, 30]);
        global_callback.execute_spawn_thread((buff,));
    });

    Ok(())
}

#[ani_rs::native]
pub fn execute_async_callback1(
    env: &AniEnv,
    async_callback: AniAsyncCallback,
) -> Result<(), BusinessError> {
    let err = BusinessError::new(401, "failed1".to_string());
    async_callback.execute_local(env, Some(err), (1,)).unwrap();
    Ok(())
}

#[ani_rs::native]
pub fn execute_async_callback2(async_callback: AniAsyncCallback) -> Result<(), BusinessError> {
    let err = BusinessError::new(402, "failed2".to_string());
    async_callback.execute_current(Some(err), (2,)).unwrap();
    Ok(())
}

#[ani_rs::native]
pub fn execute_async_callback3(
    env: &AniEnv,
    async_callback: AniAsyncCallback,
) -> Result<(), BusinessError> {
    let err = BusinessError::new(403, "failed3".to_string());
    let global_callback = async_callback.into_global_callback::<(i32,)>(env).unwrap();
    global_callback.execute_spawn_thread(Some(err), (3,));
    Ok(())
}

#[ani_rs::native]
pub fn execute_async_callback4(
    env: &AniEnv,
    async_callback: AniAsyncCallback,
) -> Result<(), BusinessError> {
    let err = BusinessError::new(404, "failed4".to_string());
    let global_callback = async_callback.into_global_callback::<(i32,)>(env).unwrap();
    thread::spawn(move || {
        global_callback.execute(Some(err), (4,));
    });

    Ok(())
}

#[ani_rs::native]
pub fn execute_error_callback1(
    env: &AniEnv,
    error_callback: AniErrorCallback,
) -> Result<(), BusinessError> {
    let err = BusinessError::new(401, "failed1".to_string());
    error_callback.execute_local(env, err).unwrap();
    Ok(())
}

#[ani_rs::native]
pub fn execute_error_callback2(error_callback: AniErrorCallback) -> Result<(), BusinessError> {
    let err = BusinessError::new(402, "failed2".to_string());
    error_callback.execute_current(err).unwrap();
    Ok(())
}

#[ani_rs::native]
pub fn execute_error_callback3(
    env: &AniEnv,
    error_callback: AniErrorCallback,
) -> Result<(), BusinessError> {
    let err = BusinessError::new(403, "failed3".to_string());
    let global_callback = error_callback.into_global_callback(env).unwrap();
    global_callback.execute_spawn_thread(err);
    Ok(())
}

#[ani_rs::native]
pub fn execute_error_callback4(
    env: &AniEnv,
    error_callback: AniErrorCallback,
) -> Result<(), BusinessError> {
    let err = BusinessError::new(404, "failed4".to_string());
    let global_callback = error_callback.into_global_callback(env).unwrap();
    thread::spawn(move || {
        global_callback.execute(err);
    });

    Ok(())
}

#[ani_rs::native]
pub fn execute_ani_ref_callback1(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let s = String::from("rust");
    let argv = env.serialize(&s).unwrap();
    callback.execute_local(env, (argv,)).unwrap();
    Ok(())
}

#[ani_rs::native]
pub fn execute_ani_ref_callback2(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let global_callback = callback
        .into_global_callback::<(GlobalRef<AniRef<'static>>,)>(env)
        .unwrap();
    let s = String::from("rust");
    let argv = env.serialize(&s).unwrap();
    let global_argv = argv.into_global(env).unwrap();
    global_callback.execute_spawn_thread((global_argv,));
    Ok(())
}

#[ani_rs::ani(path = "anirs.test.ani_test.AniRefStruct", output = "only")]
struct AniRefCallbackStruct {
    ani_obj_string: GlobalRef<AniObject<'static>>,
    ani_string: String,
}

#[ani_rs::native]
pub fn execute_ani_ref_callback3(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let s1 = String::from("rust ");
    let s1_ref = env.serialize(&s1).unwrap();
    let s1_obj = AniObject::from(s1_ref);
    let s1_global = s1_obj.into_global(env).unwrap();
    let s2 = String::from("cpp");
    let argv = AniRefCallbackStruct {
        ani_obj_string: s1_global,
        ani_string: s2,
    };

    callback.execute_local(env, (argv,)).unwrap();
    Ok(())
}

#[ani_rs::native]
pub fn execute_ani_ref_callback4(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let global_callback = callback
        .into_global_callback::<(AniRefCallbackStruct,)>(env)
        .unwrap();

    let s1 = String::from("rust ");
    let s1_ref = env.serialize(&s1).unwrap();
    let s1_obj = AniObject::from(s1_ref);
    let s1_global = s1_obj.into_global(env).unwrap();
    let s2 = String::from("cpp");
    let argv = AniRefCallbackStruct {
        ani_obj_string: s1_global,
        ani_string: s2,
    };

    global_callback.execute((argv,));
    Ok(())
}

#[ani_rs::native]
pub fn execute_multi_callbacks(
    env: &AniEnv,
    callback1: AniFnObject,
    callback2: AniAsyncCallback,
    callback3: AniErrorCallback,
) -> Result<(), BusinessError> {
    callback1.execute_local(env, (1,)).unwrap();
    callback2.execute_local(env, None, (2,)).unwrap();
    callback3.execute_local(env, BusinessError::new_static(3, "err")).unwrap();
    Ok(())
}

#[ani_rs::native]
pub fn execute_throw_error_callback1(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let global_callback = callback.into_global_callback(env).unwrap();
    global_callback.execute((1,));
    Ok(())
}

#[ani_rs::native]
pub fn execute_throw_error_callback2(env: &AniEnv, async_callback: AniAsyncCallback) -> Result<(), BusinessError> {
    let global_callback = async_callback.into_global_callback(env).unwrap();
    global_callback.execute(None, (1,));
    Ok(())
}

#[ani_rs::native]
pub fn send_event_test1(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let global_callback = callback.into_global(env).unwrap();

    thread::spawn(move || {
        ani_rs::send_event_from_closure(move || {
            let env = AniVm::get_instance().get_env().unwrap();
            let _ = global_callback.execute_local(&env, (1,)).unwrap();
        }, "send_event_test1").unwrap();
    });

    Ok(())
}

#[ani_rs::native]
pub fn send_event_test2(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let global_callback = callback.into_global(env).unwrap();
    
    ani_rs::send_event_from_closure(move || {
        let env = AniVm::get_instance().get_env().unwrap();
        let _ = global_callback.execute_local(&env, (2,)).unwrap();
    }, "send_event_test2").unwrap();

    Ok(())
}