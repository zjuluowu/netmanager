# ANI_RS 用户指南

ani_rs为用户提供了ArkTS 1.2和Rust交互的能力。

ani_rs提供序列化和反序列化能力：

- 序列化：将Rust类型转化为Arkts1.2类型。
- 反序列化：将Arkts1.2类型转化为Rust类型。

ani_rs提供Rust方法和ets native函数/方法绑定能力。

## 命名约定

Rust 和 ArkTS的代码风格有很大区别，Rust 社区喜欢 `snake_case` 风格，而 ArkTS社区延用JavaScript ，更喜欢 `camelCase` 风格。**ani_rs** 会自动转换两种风格。

- Rust定义的结构体名或枚举名可以随意定义。
- Rust结构体中的字段名必须保持snake命名风格，最终会通过ani_rs过程宏将序列化名修改为小驼峰。Rust结构体字段名转换成小驼峰后必须和Arkts结构体字段名一致。
- Rust枚举中的字段名必须保持大驼峰，最终会通过ani_rs过程宏将序列化名修改为全大写。Rust枚举字段名转换成全大写后必须和Arkts结构体字段名一致。

```rust
#[ani_rs::ani(path = "@ohos.anirs.test.ani_test.ActionInner")]
pub enum Action {
    Download, //命名与Arkts一致，仅命名风格区别，ani_rs过程宏自动切换命名风格
    Upload,
}

#[ani_rs::ani(path = "@ohos.anirs.test.ani_test.ConfigInner")]
pub struct Config {
    pub config_action: Action, //命名与Arkts一致，仅命名风格区别，ani_rs过程宏自动切换命名风格
    pub config_url: String,
}
```

test.ets

```Js
export enum ActionInner {
    DOWNLOAD,
    UPLOAD
}

export class ConfigInner {
    configAction: ActionInner;
    configUrl: string;
}
```

如果结构体仅涉及序列化，不涉及反序列化(即仅存在rust到arkts的转换)，可以如下定义：

```rust
#[ani_rs::ani(path = "@ohos.anirs.test.ani_test.ConfigInner", output = "only")]
pub struct Config {
    pub config_action: Action,
    pub config_url: String,
}
```

如果结构体仅涉及反序列化，不涉及序列化(即仅存在arkts到rust的转换)，可以如下定义。 

```rust
#[ani_rs::ani]
pub struct Config {
    pub config_action: Action,
    pub config_url: String,
}
```

## Rust方法与native中的函数/方法绑定

通过**ani_constructor**宏完成rust函数和ets native函数或方法绑定。通过**namespace**标签标注绑定命名空间中的方法，通过**class**标签标注绑定class中的方法。冒号左侧为arkts中的native方法，冒号右侧为Rust方法。

Rust中完成函数/方法绑定：

```rust
ani_constructor!(
    namespace "@ohos.net.connection.connection"
    [
        "addCustomDnsRuleSync": connection::add_custom_dns_rule,
    ]
    class "@ohos.net.connection.connection.NetConnectionInner"
    [
        "onNetAvailable": connection::on_net_available,
        "onNetBlockStatusChange": connection::on_net_block_status_change,
    ]
);
```

ets中native方法定义：

```javascript
export native function addCustomDnsRuleSync(host: string, ip: Array<string>): int;

class NetConnectionInner {
	native onNetAvailable(callback: Callback<NetHandle>):void;
    native onNetBlockStatusChange(callback: Callback<NetBlockStatusInfo>):void;
}
```

Rust中函数定义。

1、通过#[ani_rs::native]过程宏自动完成参数和返回值的转换。

```rust
#[ani_rs::native]
pub fn add_custom_dns_rule(env: &AniEnv, host: String, ips: Vec<String>) -> Result<i32, BusinessError> {...}
```

2、class中的方法必须比namespace中的方法多一个this参数。

```rust
#[ani_rs::native]
pub fn on_net_available(
    env: &AniEnv,
    this: NetConnection,
    callback: AniFnObject,
) -> Result<(), BusinessError> {...}
```

3、第一个参数env不需要可以省略，需要时参数名必须是env。

4、函数的返回值必须是Result<T, BusinessError>.



## 类型转化

Rust 和 ArkTS类型之间的转换。

| ArkTS 1.2 类型         | Rust类型         | 备注                                                         |
| ---------------------- | ---------------- | ------------------------------------------------------------ |
| undefined 或者 ？      | Option<T>        | 如 int \| undefined 对应Option<i32>。 input?:int 对应Option<i32> |
| null                   | ()               | 无                                                           |
| boolean                | bool             | 无                                                           |
| byte                   | i8               | 无                                                           |
| short                  | i16              | 无                                                           |
| int                    | i32              | 无                                                           |
| long                   | i64              | 无                                                           |
| float                  | f32              | 无                                                           |
| double/number          | f64              | arkts中number为double的别名                                  |
| char                   | 暂不支持         | arkts中的char是utf16，Rust暂不支持                           |
| string                 | String           | 无                                                           |
| Record                 | HashMap          | 无                                                           |
| RecordData             | JsonValue        | 代表层级不确定的Arkts类型                                    |
| Array                  | Vec              | 无                                                           |
| ArrayBuffer            | ArrayBuffer      | 无                                                           |
| Int8Array              | Int8Array        | 无                                                           |
| Uint8Array             | Uint8Array       | 无                                                           |
| Int16Array             | Int16Array       | 无                                                           |
| Uint16Array            | Uint16Array      | 无                                                           |
| Int32Array             | Int32Array       | 无                                                           |
| Uint32Array            | Uint32Array      | 无                                                           |
| function / Callback    | AniFnObject      | @ohos.base.d.ets中的Callback或者普通的arkts方法              |
| AsyncCallback          | AniAsyncCallback | @ohos.base.d.ets中的AsyncCallback                            |
| ErrorCallback          | AniErrorCallback | @ohos.base.d.ets中的ErrorCallback                            |
| bigint                 | 暂不支持         | 暂不支持                                                     |
| union                  | enum             | 如 boolean \| byte \| int                                    |
| enum                   | enum             |                                                              |
| class                  | struct           |                                                              |
| 除基础类型外的任意类型 | AniRef/AniObject | 除基础类型外的任意类型都可以使用AniRef或AniObject接收，支持用户手动解析 |

### Arkts基础类型

arkts基础类型包括boolean、byte、short、int、long、float、double/number，可以直接使用对应的Rust类型用作参数和返回值使用。

rust：

```rust
#[ani_rs::native]
pub fn primitive_test(input: i32) -> Result<i64, BusinessError> {...}
```

ets:

```javascript
export native function primitiveTest(input: int): long
```

### string

代表 ArkTS的 `string` 类型。

rust：

```rust
#[ani_rs::native]
pub fn string_test(input: String) -> Result<String, BusinessError> {...}
```

ets:

```javascript
export native function stringTest(input:string): string
```

### Record

rust：

```rust
#[ani_rs::native]
pub fn record_struct(input: HashMap<i32, ResponseCode>) -> Result<HashMap<i32, ResponseCode>, BusinessError> {...}
```

ets:

```javascript
export native function recordStruct(input: Record<int, ResponseCode>): Record<int, ResponseCode>
```

### Array

rust:

```rust
#[ani_rs::native]
pub fn array_i64(input: Vec<i64>) -> Result<Vec<i64>, BusinessError> {...}
```

ets:

```javascript
export native function arrayLong(input: Array<long>): Array<long>
```

### ArrayBuffer

rust:

```rust
#[ani_rs::native]
pub fn array_buffer_test(input: ArrayBuffer) -> Result<ArrayBuffer, BusinessError> {...}
```

ets:

```javascript
export native function arrayBufferTest(input: ArrayBuffer): ArrayBuffer
```

rust中定义ArrayBuffer结构体，并提供一系列方法供开发者使用。

```rust
pub struct ArrayBuffer {
    /* private fields */
}

impl ArrayBuffer {
    pub fn new_with_vec(data: Vec<u8>) -> Self
    pub fn len(&self) -> usize
    pub fn to_vec(&self) -> Vec<u8>
}
impl AsRef<[u8]> for ArrayBuffer
impl AsMut<[u8]> for ArrayBuffer
impl Deref for ArrayBuffer
impl DerefMut for ArrayBuffer
```

> 从Arkts传递给 Rust 的 `ArrayBuffer` 是一个 **引用**， 不会执行任何数据 `Copy` 或 `Clone`，在Rust侧对 `ArrayBuffer` 的每次更改都会反映到原始的 Arkts的`ArrayBuffer`中。

### TypeArray

rust:

```rust
#[ani_rs::native]
pub fn int32_array_test(input: Int32Array) -> Result<Int32Array, BusinessError> {...}
```

ets:

```javascript
export native function int32ArrayTest(input: Int32Array): Int32Array
```

rust中定义一系列TypeArray相关的结构体，包括Int8Array、Int16Array、Int32Array、Uint8Array、Uint16Array、Uint32Array，并提供一系列方法供开发者使用，以Int8Array为例，其余结构体类似。

```rust
pub struct Int8Array {
    /* private fields */
}
impl Int8Array {
    pub fn new_with_vec(data: Vec<i8>) -> Self
    pub fn len(&self) -> usize
    pub fn to_vec(&self) -> Vec<i8>
}
impl AsRef<[i8]> for Int8Array
impl AsMut<[i8]> for Int8Array
impl Deref for Int8Array
impl DerefMut for Int8Array
```

> 从Arkts传递给 Rust 的 `TypeArray` 是一个 **引用**， 不会执行任何数据 `Copy` 或 `Clone`，在Rust侧对 `TypeArray` 的每次更改都会反映到原始的 Arkts的`TypeArray`中。

### Callback/AsyncCallback/ErrorCallback

rust的类型Callback、AsyncCallback、ErrorCallback分别代表@ohos.base.d.ets定义的Callback、AsyncCallback和ErrorCallback回调，用于在Rust中调用Arks方法。

rust:

```rust
#[ani_rs::native]
pub fn execute_callback(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    callback.execute_local(env, (1,)).unwrap();
    Ok(())
}

#[ani_rs::native]
pub fn execute_async_callback(
    env: &AniEnv,
    async_callback: AniAsyncCallback,
) -> Result<(), BusinessError> {
    let err = BusinessError::new(401, "failed1".to_string());
    async_callback.execute_local(env, Some(err), (1,)).unwrap();
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
```

ets:

```javascript
export native function executeCallback(callback: Callback<int>): void
export native function executeAsyncCallback(callback: AsyncCallback<int>): void
export native function executeErrorCallback(callback: ErrorCallback): void
```

rust为Callback、AsyncCallback、ErrorCallback实现了一系列方法，以Callback为例，其余类似。

```rust
pub struct AniFnObject<'local>(/* private fields */);

impl<'local> AniFnObject<'local> {
    // 在Arkts主线程中执行callback
    pub fn execute_local<T>(&self, env: &AniEnv<'local>, input: T) -> Result<AniRef, AniError>
    // 在任意线程中执行callback
    pub fn execute_current<T>(&self, input: T) -> Result<AniRef, AniError>
    // 转换成具有全局作用域的GlobalRefCallback，用于需要保存callback的情况
    pub fn into_global_callback<T: InputVec + Send + 'static>(
        self,
        env: &AniEnv<'local>,
    ) -> Result<GlobalRefCallback<T>, AniError>
}

pub struct GlobalRefCallback<T: InputVec + Send + 'static> {
    /* private fields */
}

impl<T> GlobalRefCallback<T> {
    //支持在任意线程中调用，会将当前callback抛到arkts主线程中执行
    pub fn execute(&self, input: T)
}
```

### JsonValue

RecordData用于表示结构层级不确定的类型，当前支持在Rust中将RecordData序列化为String，以及将String反序列化为RecordData。

rust：

```rust
#[ani_rs::native]
pub fn json_ser_deser_test(json: JsonValue) -> Result<JsonValue, BusinessError> {...}
```

ets:

```javascript
export native function jsonSerDeserTest(data: RecordData): RecordData;
```

JsonValue对应的方法

```rust
pub struct JsonValue<'local>(/* private fields */);
impl JsonValue<'_> {
    //将RecordData序列化为String
    pub fn stringify(&self, env: &AniEnv) -> Result<String, AniError>
    //将String反序列化为RecordData
    pub fn parse<'local>(
        env: &AniEnv<'local>,
        param_string: &String,
    ) -> Result<JsonValue<'local>, AniError>
    // 转换成具有全局作用域的RecordData，用于需要直接保存RecordData的情况
    pub fn into_global(self, env: &AniEnv) -> Result<GlobalRef<JsonValue<'static>>, AniError>
```

### enum表示arkts union

Arkts中使用 `|`表示union类型，如`boolean | byte | short`，Rust中使用enum表示，并手动加上#[derive(Serialize, Deserialize)]。

rust:

```rust
#[derive(Serialize, Deserialize)]
enum Data {
    Boolean(bool), //命名必须为Boolean，下同
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
```

ets:

```typescript
export type unionType = boolean | byte | short | int | long | float 
                            | double | String | ArrayBuffer | null 
                            | Record<string, string> | string[] 
                            | Uint8Array | Int8Array | Uint16Array | Int16Array | Uint32Array | Int32Array ;

export native function unionTest ( input: unionType ): unionType;
```

用户定义enum用于表示union类型时，对enum中的字段名存在约束，必须按照下表中rust enum字段名进行命名，底层反序列化时根据字段名判断当前传入的arkts对象是否为该类型。

| Rust类型      | rust enum字段名 |
| ------------- | --------------- |
| bool          | Boolean         |
| i8            | I8              |
| i16           | I16             |
| i32           | I32             |
| i64           | I64             |
| f32           | F32             |
| f64           | F64             |
| String        | S               |
| Vec<T>        | Array           |
| HashMap<K, V> | Record          |
| ArrayBuffer   | ArrayBuffer     |
| Int8Array     | Int8Array       |
| Int16Array    | Int16Array      |
| Int32Array    | Int32Array      |
| Uint8Array    | Uint8Array      |
| Uint16Array   | Uint16Array     |
| Uint32Array   | Uint32Array     |
| ()            | Null            |

如果union中出现自定义class类型，用户需要在enum的字段中通过serde rename手动标注class路径名。

rust:

```rust
#[derive(serde::Serialize, serde::Deserialize)]
pub enum ResponseCodeOutput {
    #[serde(rename = "anirs.test.ani_test.ResponseCode")]
    Code(ResponseCode),
    #[serde(rename = "anirs.test.ani_test.HttpProtocol")]
    Proto(HttpProtocol),
    I32(i32),
}

#[ani_rs::native]
pub fn enum_test_struct(input: ResponseCodeOutput) -> Result<ResponseCodeOutput, BusinessError> {...}
```

ets:

```typescript
export native function enumTestStruct(input: ResponseCode | HttpProtocol | int): ResponseCode | HttpProtocol | int
```

### enum表示arkts enum

rust

```rust
#[ani_rs::ani(path = "@ohos.anirs.test.ani_test.EnumNumber")]
enum EnumNumber {
    One = 1,
    Two = 2,
    Three = 3,
}

#[ani_rs::native]
pub fn enum_test_number(input: EnumNumber) -> Result<EnumNumber, BusinessError> {...}
```

ets:

```javascript
export enum EnumNumber {
    ONE = 1,
    TWO = 2,
    THREE = 3,
}
export native function enumTestNumber ( input: EnumNumber): EnumNumber
```

> Rust枚举中的字段名必须保持大驼峰，这符合Rust编程规范。Arkts中枚举字段名必须保持全大写，这符合Arkts编程规范。
>
> 最终会通过ani_rs过程宏将Rust序列化名修改为全大写。Rust枚举字段名和Arkts枚举字段名除命名风格外需保持名字一致。

### struct

rust

```rust
#[ani_rs::ani(path = "@ohos.anirs.test.ani_test.Config")]
pub struct Config {
    pub config_action: Action,
    pub config_url: String,
}

#[ani_rs::native]
pub fn struct_enum(input: Config) -> Result<Config, BusinessError> {...}
```

ets:

```javascript
export class Config {
    configAction: Action;
    configUrl: string;
}
export native function structEnum(input: Config): Config;
```

> Rust结构体的字段名必须保持snake_case风格，这符合Rust编程规范。Arkts中结构体字段名必须保持小驼峰，这符合Arkts编程规范。
>
> 最终会通过ani_rs过程宏将Rust序列化名修改为小驼峰。Rust构体字段名和Arkts结构体字段名除命名风格外需保持名字一致。

### AniRef/AniObject

AniRef或AniObject是Arkts侧传递过来的原始类型，AniRef和AniObject是等价的，定义如下，这两个类型实现了序列化和反序列化，序列化和反序列化结果为自己本身。

```rust
#[repr(transparent)]
#[derive(Debug, Clone)]
pub struct AniRef<'local> {
    pub inner: ani_ref,
    lifetime: PhantomData<&'local ()>,
}

#[repr(transparent)]
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct AniObject<'local>(AniRef<'local>);
```

如果用户想手动解析参数，可以使用AniRef/AniObject，比如下面的用例，用户想根据expect_data_type的类型，手动解析extra_data。

rust: 

```rust
#[ani_rs::ani(path = "anirs.test.ani_test.HttpDataType")]
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
    let data_type = options.expect_data_type.unwrap();
    let obj_data = options.extra_data.unwrap();
    let res = match data_type {
        HttpDataType::String => {
            let res = env.deserialize::<String>(obj_data).unwrap();
            res
        }
        HttpDataType::Object => {
            let json_value = env.deserialize::<JsonValue>(obj_data).unwrap();
            let res = json_value.stringify(env).unwrap();
            res
        }
        HttpDataType::ArrayBuffer => {
            let buffer = env.deserialize::<ArrayBuffer>(obj_data).unwrap();
            let res = buffer.as_ref();
            String::from_utf8_lossy(res).to_string()
        }
    };
    Ok(res)
}
```

ets:

```javascript
export enum HttpDataType {
    STRING = 0,
    OBJECT = 1,
    ARRAY_BUFFER = 2
}

export class HttpRequestOptions {
    extraData?: string | RecordData | ArrayBuffer;
    expectDataType?: HttpDataType;
}
export native function jsonRequestTest(options: HttpRequestOptions): string;
```

## 生命周期管理

所有在 Rust native 侧创建的对象都是局部引用（*local references*）。这意味着

- 对象生命周期仅在当前调用上下文中有效；
- 一旦退出当前 native 调用，对象可能会被垃圾回收器回收；
- 即使将这些引用存储到全局对象中，也不能保证其在下次使用时仍然有效。

Rust提供GlobalRef类型表示全局引用，AniRef或AniObject通过调用into_global转换为全局引用。当全局引用GlobalRef离开作用域析构时会自动删除引用，避免内存耗尽。

```rust
#[repr(transparent)]
pub struct GlobalRef<T: Into<AniRef<'static>> + Clone>(pub T);

impl AniRef<'_> {
    pub fn into_global(self, env: &AniEnv) -> Result<GlobalRef<AniRef<'static>>, AniError>
}

impl<'local> AniObject<'local> {
    pub fn into_global(self, env: &AniEnv) -> Result<GlobalRef<AniObject<'static>>, AniError>
}
```

### VM

AniEnv只能在当前线程中使用，无法跨线程。如果想在非Arkts主线程中使用Env，需要使用AniVM，在非Arkts主线程调用attach_current_thread获取env，使用完毕后调用detach_current_thread。AniVM单例在ani_constructor!函数绑定时创建。

```rust
#[repr(transparent)]
pub struct AniVm {
    /* private fields */
}
impl AniVm {
    // 获取AniVM单例，单例在ani_constructor!函数绑定时创建
    pub fn get_instance() -> &'static AniVm
    // 若线程已附加到VM，获取env
    pub fn get_env<'local>(&self) -> Result<AniEnv<'local>, AniError>
    // VM绑定当前线程
    pub fn attach_current_thread<'local>(&self) -> Result<AniEnv<'local>, AniError>
    // VM分离当前线程
    pub fn detach_current_thread(&self) -> Result<(), AniError>
}
```

## send event

ani_rs支持将一个Rust闭包发送到Arkts主线程中执行。

```rust
pub fn send_event_from_closure<F>(callback: F, func_name: &str) -> Result<(), AniError>
where
    F: FnOnce() + Send + 'static
```

如果当前的执行环境在非Arkts主线程中，想把任务发送到Arkts主线程中执行，可以使用该接口，该接口支持在任意线程中使用。

```rust
#[ani_rs::native]
pub fn send_event_test(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let global_callback = callback.into_global(env).unwrap();

    thread::spawn(move || {
        ani_rs::send_event_from_closure(move || {
            let env = AniVm::get_instance().get_env().unwrap();
            let _ = global_callback.execute_local(&env, (1,));
        }, "send_event_test").unwrap();
    });

    Ok(())
}
```



## 注入 Env

`#[ani_rs::native]` 宏屏蔽了Arkts类型和Rust类型的转换过程，大多数情况下，不需要调用ani相关接口。但是有时候您仍然需要访问底层的 `ani`接口。

对于这种情况，**ani_rs** 允许您通过 `#[ani_rs::native]` 装饰，将 `Env` 注入到您的 `fn` 中。

rust:

```rust
#[ani_rs::native]
pub fn execute_callback(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    callback.execute_local(env, (1,)).unwrap();
    Ok(())
}
```

env可以作为第一个参数在函数中使用，且参数名必须为env。

## ani_rs约束

1、不支持char。

2、不支持bigint。

3、native函数绑定当前只支持类的非静态方法和命名空间方法，不支持类的静态方法和模块方法。

4、不支持定长数组FixedArray。
