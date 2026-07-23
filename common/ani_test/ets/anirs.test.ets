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

import {Callback, BusinessError, AsyncCallback, ErrorCallback} from "@ohos.base";

export default namespace ani_test {
    loadLibrary("ani_test")

    export enum EnumNumber {
        ONE = 1,
        TWO = 2,
        THREE = 3,
    }

    export enum EnumString
    {
        ONE = "ONE",
        TWO = "TWO",
        THREE = "THREE",
    }

    export class PrimitiveTest {
        primitiveBool: boolean;
        primitiveI8: byte;
        primitiveI16: short;
        primitiveI32: int;
        primitiveI64: long;
        primitiveF32: float;
        primitiveF64: double;
    }

    export class StructArray
    {
        arrayBool: boolean[] = []
    }

    export enum Action {
      DOWNLOAD,
      UPLOAD
    }

    export class Config {
      action: Action;
      url: string;
    }

    export class ResponseCode {
        code: int;
        url: string;
    }

    export enum HttpProtocol {
        HTTP1_1,
        HTTP2,
        HTTP3
    }

    export type unionType = boolean | byte | short | int | long | float 
                            | double | String | ArrayBuffer | null 
                            | Record<string, string> | string[] 
                            | Uint8Array | Int8Array | Uint16Array | Int16Array | Uint32Array | Int32Array ;

    export type RecordData = undefined | null | Object | Record<string, RecordData> | Array<RecordData>;

    export class AniRefStruct {
        aniObjString: string;
        aniString: string;
    }

    export class ArrayBufferStruct {
        buffer1: ArrayBuffer;
        buffer2: Int32Array;
    }

    export enum HttpDataType {
        STRING = 0,
        OBJECT = 1,
        ARRAY_BUFFER = 2
    }

    export class HttpRequestOptions {
        extraData?: string | RecordData | ArrayBuffer;
        expectDataType?: HttpDataType;
    }

    export class HttpResponse {
        result: string | RecordData | ArrayBuffer;
        resultType: HttpDataType;
    }

    export native function primitiveTest(input:PrimitiveTest): PrimitiveTest;
    export native function anirefDeserializeTest(input: string): boolean;
    export native function anirefStructDeTest(input: AniRefStruct): boolean;
    export native function anirefArrayDeTest(input: Array<string>): Array<string>;
    export native function anirefSerializeTest(input: string): boolean;
    export native function anirefStructTest(input: AniRefStruct): AniRefStruct;
    export native function anirefArrayTest(input: Array<string>): Array<string>;
    export native function returnAniRefTest(): string;

    export native function optionBool(input?:boolean): boolean | undefined;
    export native function optionByte(input?:byte): byte | undefined;
    export native function optionShort(input?:short): short | undefined;
    export native function optionInt(input?:int): int | undefined;
    export native function optionLong(input?:long): long | undefined;
    export native function optionDouble(input?:double): double | undefined;

    export native function arrayBool(input: boolean[]): boolean[];
    export native function arrayByte(input: byte[]): byte[];
    export native function arrayShort(input: Array<short>): Array<short>;
    export native function arrayInt(input: int[]): int[];
    export native function arrayLong(input: long[]): long[];
    export native function arrayFloat(input: float[]): float[];
    export native function arrayDouble ( input: double[] ): double[];

    export native function enumTestNumber ( input: EnumNumber): EnumNumber
    export native function enumTestString ( input: EnumString): EnumString
    export native function enumTestStruct(input: ResponseCode | HttpProtocol | int): ResponseCode | HttpProtocol | int

    export native function recordString(input:Record<String,String>) : Record<String, String>
    export native function recordLong(input:Record<Long, Long>) : Record<Long, Long>
    export native function recordStruct(input: Record<int, ResponseCode>): Record<int, ResponseCode>

    export native function unionTest ( input: unionType ): unionType;
    export native function unionTest2 ( input?: unionType ): unionType;

    export native function arrayBufferTest(input: ArrayBuffer): ArrayBuffer;
    export native function changeArrayBuffer(input: ArrayBuffer): ArrayBuffer;
    export native function createArrayBuffer(): ArrayBuffer;
    export native function uint8ArrayTest(input: Uint8Array): Uint8Array;
    export native function int8ArrayTest(input: Int8Array): Int8Array;
    export native function uint16ArrayTest(input: Uint16Array): Uint16Array;
    export native function int16ArrayTest(input: Int16Array): Int16Array;
    export native function uint32ArrayTest(input: Uint32Array): Uint32Array;
    export native function int32ArrayTest(input: Int32Array): Int32Array;
    export native function changeUint8Array(input: Uint8Array): Uint8Array;
    export native function changeInt8Array(input: Int8Array): Int8Array;
    export native function changeUint16Array(input: Uint16Array): Uint16Array;
    export native function changeInt16Array(input: Int16Array): Int16Array;
    export native function changeUint32Array(input: Uint32Array): Uint32Array;
    export native function changeInt32Array(input: Int32Array): Int32Array;

    export native function createUint8Array(input: Uint8Array): Uint8Array;
    export native function createInt8Array(input: Int8Array): Int8Array;
    export native function createUint16Array(input: Uint16Array): Uint16Array;
    export native function createInt16Array(input: Int16Array): Int16Array;
    export native function createUint32Array(input: Uint32Array): Uint32Array;
    export native function createInt32Array(input: Int32Array): Int32Array;
    export native function arrayBufferStrcutTest(input: ArrayBufferStruct): ArrayBufferStruct

    export native function structEnum(input: Config): Config;
    export native function executeCallback1(callback: Callback<int>): void;
    export native function executeCallback2(callback: Callback<int>): void;
    export native function executeCallback3(callback: Callback<int>): void;
    export native function executeCallback4(callback: Callback<int>): void;
    export native function executeCallback5(callback: Callback<ArrayBuffer>): void;
    export native function executeCallback6(callback: Callback<Uint32Array>): void;
    export native function executeAsyncCallback1(callback: AsyncCallback<int>): void;
    export native function executeAsyncCallback2(callback: AsyncCallback<int>): void;
    export native function executeAsyncCallback3(callback: AsyncCallback<int>): void;
    export native function executeAsyncCallback4(callback: AsyncCallback<int>): void;

    export native function executeErrorCallback1(callback: ErrorCallback): void;
    export native function executeErrorCallback2(callback: ErrorCallback): void;
    export native function executeErrorCallback3(callback: ErrorCallback): void;
    export native function executeErrorCallback4(callback: ErrorCallback): void;
    export native function executeAniRefCallback1(callback: Callback<string>): void;
    export native function executeAniRefCallback2(callback: Callback<string>): void;
    export native function executeAniRefCallback3(callback: Callback<AniRefStruct>): void;
    export native function executeAniRefCallback4(callback: Callback<AniRefStruct>): void;
    export native function executeMultiCallbacks(callback1: Callback<int>, callback2: AsyncCallback<int>, callback3: ErrorCallback): void;
    export native function executeThrowErrorCallback1(callback: Callback<int>): void;
    export native function executeThrowErrorCallback2(callback: AsyncCallback<int>): void;
    export native function sendEventTest1(callback: Callback<int>): void;
    export native function sendEventTest2(callback: Callback<int>): void;

    export native function businessErrorTest(): void;

    export native function jsonSerDeserTest(data: RecordData): RecordData;
    export native function jsonStringifyTest1(data: RecordData): string;
    export native function jsonParseTest1(data: string): RecordData;
    export native function executeJsonCallback1(callback: Callback<Record<string, number>>): void;
    export native function executeJsonCallback2(callback: Callback<Record<string, number>>): void;
    export native function jsonRequestTest(options: HttpRequestOptions): string;
    export native function jsonResponseTest1(callback: AsyncCallback<HttpResponse>, testCase: int): void;
    export native function jsonResponseTest2(callback: AsyncCallback<HttpResponse>, testCase: int): void;
}

function assertEQ<T, U>(expected: T, actual: U, message?: string): void {
  if (expected !== actual) {
    const errorMessage = message || `[AniRs] Assertion failed: expected ${expected}, but got ${actual}`;
    throw new Error(errorMessage);
  }
}

async function msSleep(count:int) : Promise<int> {
    return new Promise<int>((resolve, reject) => {
        setTimeout(() => {
        resolve(0)
        }, count)
    })
}

function main():void{
    console.log("[AniRs] start ani rs test");
    primitive_test();
    option_test();
    array_test();
    enum_test();
    record_test();
    union_test();
    array_buffer_test();
    type_array_test();
    struct_test();
    callback_test();
    business_test();
    json_test();
    console.log("[AniRs] end ani rs test");
}

function primitive_test():void {
    let input:ani_test.PrimitiveTest = {
        primitiveBool: true,
        primitiveI8: -128,
        primitiveI16: 32767,
        primitiveI32: -2147483648,
        primitiveI64: 9223372036854775807,
        primitiveF32: 3.13f,
        primitiveF64: 3.14159,
    }
    let output = ani_test.primitiveTest(input);
    
    assertEQ(input.primitiveBool,  output.primitiveBool);
    assertEQ(input.primitiveI8, output.primitiveI8);
    assertEQ(input.primitiveI16, output.primitiveI16);
    assertEQ(input.primitiveI32, output.primitiveI32);
    assertEQ(input.primitiveI64, output.primitiveI64);
    assertEQ(input.primitiveF32, output.primitiveF32);
    assertEQ(input.primitiveF64, output.primitiveF64);

    let output2 = ani_test.anirefDeserializeTest('hello');
    assertEQ(output2, true);

    let input3: ani_test.AniRefStruct = {
        aniObjString: 'hello world',
        aniString: 'hello world',
    }
    let output3 = ani_test.anirefStructDeTest(input3)
    assertEQ(output3, true);

    let input4 = ['rust', 'java', 'c++']
    let output4 = ani_test.anirefArrayDeTest(input4)
    assertEQ(JSON.stringify(input4), JSON.stringify(output4))

    let output5 = ani_test.anirefSerializeTest('hello');
    assertEQ(output5, true);

    let input6: ani_test.AniRefStruct = {
        aniObjString: 'hello world',
        aniString: 'hello world',
    }
    let output6 = ani_test.anirefStructTest(input6)
    assertEQ(JSON.stringify(input6), JSON.stringify(output6))

    let input7 = ['rust', 'java', 'c++']
    let output7 = ani_test.anirefArrayDeTest(input7)
    assertEQ(JSON.stringify(input7), JSON.stringify(output7))

    let output8 = ani_test.returnAniRefTest();
    assertEQ(output8, 'hello world')
}

function option_test():void {
    let input0 = true;
    let output0 = ani_test.optionBool(input0);
    assertEQ(input0, output0);
    let input1 = undefined;
    let output1 = ani_test.optionBool(input1);
    assertEQ(input1, output1);

    let input2:byte = -128;
    let output2 = ani_test.optionByte( input2 );
    assertEQ(input2, output2);
    let input3 = undefined;
    let output3 = ani_test.optionByte(input3);
    assertEQ( input3, output3 );
    
    let input4:short = 32767;
    let output4 = ani_test.optionShort( input4 );
    assertEQ(input4, output4);
    let input5 = undefined;
    let output5 = ani_test.optionShort(input5);
    assertEQ( input5, output5 );

    let input6:int = -2147483648;
    let output6 = ani_test.optionInt( input6 );
    assertEQ(input6, output6);
    let input7 = undefined;
    let output7 = ani_test.optionInt(input7);
    assertEQ( input7, output7 );

    let input8:long =  9223372036854775807;
    let output8 = ani_test.optionLong( input8 );
    assertEQ(input8, output8);
    let input9 = undefined;
    let output9 = ani_test.optionLong(input9);
    assertEQ( input9, output9 );

    let input10:double =  3.14159;
    let output10 = ani_test.optionDouble( input10 );
    assertEQ(input10, output10);
    let input11 = undefined;
    let output11 = ani_test.optionDouble(input11);
    assertEQ( input11, output11 );
}

function array_test (): void
{
    let input0: boolean[] = [ true, false, true ];
    let output0 = ani_test.arrayBool( input0 );
    assertEQ( JSON.stringify( input0 ), JSON.stringify( output0 ) );
   
    let input1: byte[] = [ 1, 2, 3 ];
    let output1 = ani_test.arrayByte( input1 );
    assertEQ( JSON.stringify(input1), JSON.stringify(output1) );

    let input2: Array<short> = [ 1, 2, 3 ];
    let output2 = ani_test.arrayShort( input2 );
    assertEQ( JSON.stringify(input2), JSON.stringify(output2) );

    let input3: int[] = [ 1, 2, 3 ];
    let output3 = ani_test.arrayInt( input3 );
    assertEQ( JSON.stringify(input3), JSON.stringify(output3) );

    let input4: long[] = [ 1, 2, 3 ];
    let output4 = ani_test.arrayLong( input4 );
    assertEQ( JSON.stringify(input4), JSON.stringify(output4) );

    let input41: float[] = [ 1.0f, 2.0f, 3.0f ];
    let output41 = ani_test.arrayFloat( input41 );
    assertEQ( JSON.stringify( input41 ), JSON.stringify( output41 ) );

    let input5: double[] = [ 1.0, 2.0, 3.0 ];
    let output5 = ani_test.arrayDouble( input5 );
    assertEQ( JSON.stringify( input5 ), JSON.stringify( output5 ) );
    
    let input6: boolean[] = [];
    let output6 = ani_test.arrayBool( input6 );
    assertEQ( JSON.stringify(input6), JSON.stringify(output6) );

    let input7: byte[] = [];
    let output7 = ani_test.arrayByte( input7 );
    assertEQ( JSON.stringify(input7), JSON.stringify(output7) );

    let input8: short[] = [];
    let output8 = ani_test.arrayShort( input8 );
    assertEQ( JSON.stringify(input8), JSON.stringify(output8) );

    let input9: int[] = [];
    let output9 = ani_test.arrayInt( input9 );
    assertEQ( JSON.stringify(input9), JSON.stringify(output9) );

    let input10: long[] = [];
    let output10 = ani_test.arrayLong( input10 );
    assertEQ( JSON.stringify(input10), JSON.stringify(output10) );

    let input11: double[] = [];
    let output11 = ani_test.arrayDouble( input11 );
    assertEQ( JSON.stringify(input11), JSON.stringify(output11) );
}

function enum_test(): void
{
    let input0 = ani_test.EnumNumber.ONE;
    let output0 = ani_test.enumTestNumber( input0 );
    assertEQ( input0, output0 );

    let input1 = ani_test.EnumNumber.TWO;
    let output1 = ani_test.enumTestNumber( input1 );
    assertEQ( input1, output1 );

    let input2 = ani_test.EnumNumber.THREE;
    let output2 = ani_test.enumTestNumber( input2 );
    assertEQ( input2, output2 );

    let input3 = ani_test.EnumString.ONE;
    let output3 = ani_test.enumTestString( input3 );
    assertEQ( input3, output3 );

    let input4 = ani_test.EnumString.TWO;
    let output4 = ani_test.enumTestString( input4 );
    assertEQ( input4, output4 );

    let input5 = ani_test.EnumString.THREE;
    let output5 = ani_test.enumTestString( input5 );
    assertEQ( input5, output5 );

    let intput6: ani_test.ResponseCode = {code: 10, url: "www.baidu.com"};
    let output6 = ani_test.enumTestStruct(intput6)
    assertEQ(JSON.stringify(intput6), JSON.stringify(output6) );
    let intput7 = 666;
    let output7 = ani_test.enumTestStruct(intput7)
    assertEQ(JSON.stringify(intput7), JSON.stringify(output7) );
    let intput8 = ani_test.HttpProtocol.HTTP2;
    let output8 = ani_test.enumTestStruct(intput8)
    assertEQ(intput8, output8 );
}

function record_test (): void
{
    let input:Record<String,String> = { "hello": "rust", "nihao": "ani" };
    let output = ani_test.recordString(input);
    assertEQ( output.get("hello") , "rust" );
    assertEQ( output.get( "nihao" ), "ani" );
    
    let input1: Record<Long, Long> = { 1: 2, 3: 4 };
    let output1 = ani_test.recordLong( input1 );
    assertEQ( output1.get( 1 ), 2 );
    assertEQ( output1.get( 3 ), 4 );

    let inputResponse1: ani_test.ResponseCode = {code: 10, url: "www.baidu.com"};
    let inputResponse2: ani_test.ResponseCode = {code: 20, url: "www.google.com"};
    let input2: Record<int, ani_test.ResponseCode> = {1: inputResponse1, 2: inputResponse2};
    let output2 = ani_test.recordStruct(input2);
    assertEQ(JSON.stringify(output2.get(1)), JSON.stringify(inputResponse1));
    assertEQ(JSON.stringify(output2.get(2)), JSON.stringify(inputResponse2));
}

function union_test (): void 
{
    let input0:byte = 1;
    let output0 = ani_test.unionTest( input0 );
    assertEQ( input0, output0 );
    output0 = ani_test.unionTest2( input0 );
    assertEQ( input0, output0 );

    let input1:short = 2;
    let output1 = ani_test.unionTest( input1 );
    assertEQ( input1, output1 );
    output1 = ani_test.unionTest2( input1 );
    assertEQ( input1, output1 );

    let input2:int = 3;
    let output2 = ani_test.unionTest( input2 );
    assertEQ( input2, output2 );
    output2 = ani_test.unionTest2( input2 );
    assertEQ( input2, output2 );

    let input3:long = 4;
    let output3 = ani_test.unionTest( input3 );
    assertEQ( input3, output3 );
    output3 = ani_test.unionTest2( input3 );
    assertEQ( input3, output3 );

    let input4:double = 5.0;
    let output4 = ani_test.unionTest( input4 );
    assertEQ( input4, output4 );
    output4 = ani_test.unionTest2( input4 );
    assertEQ( input4, output4 );

    let input5:String = "Hello";
    let output5 = ani_test.unionTest( input5 );
    assertEQ( input5, output5 );
    output5 = ani_test.unionTest2( input5 );
    assertEQ( input5, output5 );

    let input6:boolean = true;
    let output6 = ani_test.unionTest( input6 );
    assertEQ( input6, output6 );
    output6 = ani_test.unionTest2( input6 );
    assertEQ( input6, output6 );

    let input7 = ArrayBuffer.from([1,2,3,4]);
    let output7 = ani_test.unionTest( input7 ) as ArrayBuffer;
    assertEQ(input7.at(0), output7.at(0));
    assertEQ(input7.at(1), output7.at(1));
    assertEQ(input7.at(2), output7.at(2));
    assertEQ(input7.at(3), output7.at(3));
    output7 = ani_test.unionTest2( input7 ) as ArrayBuffer;
    assertEQ(input7.at(0), output7.at(0));
    assertEQ(input7.at(1), output7.at(1));
    assertEQ(input7.at(2), output7.at(2));
    assertEQ(input7.at(3), output7.at(3));

    let input8:null = null;
    let output8 = ani_test.unionTest( input8 );
    assertEQ( input8, output8 );
    output8 = ani_test.unionTest2( input8 );
    assertEQ( input8, output8 );

    let input9: Record<String,String> = { "hello": "rust", "nihao": "ani" };
    let output9 = ani_test.unionTest( input9 ) as Record<String,String>;
    assertEQ( output9.get("hello") , "rust" );
    assertEQ( output9.get( "nihao" ), "ani" );
    output9 = ani_test.unionTest2( input9 ) as Record<String,String>;
    assertEQ( output9.get("hello") , "rust" );
    assertEQ( output9.get( "nihao" ), "ani" );
    
    let output10 = ani_test.unionTest2();
    assertEQ( output10, undefined );

    let input11: string[] = ['rust', 'java', 'c++'];
    let output11 = ani_test.unionTest( input11 );
    assertEQ( JSON.stringify(input11), JSON.stringify(output11) );
    output11 = ani_test.unionTest2( input11 );
    assertEQ( JSON.stringify(input11), JSON.stringify(output11) );

    let input12 = new Uint8Array([0,1,2]);
    let output12 = ani_test.unionTest( input12 ) as Uint8Array;
    assertEQ(JSON.stringify(output12), JSON.stringify(input12));
    output12 = ani_test.unionTest2( input12 ) as Uint8Array;
    assertEQ(JSON.stringify(output12), JSON.stringify(input12));

    let input13 = new Int8Array([0,1,2]);
    let output13 = ani_test.unionTest( input13 ) as Int8Array;
    assertEQ(JSON.stringify(output13), JSON.stringify(input13));
    output13 = ani_test.unionTest2( input13 ) as Int8Array;
    assertEQ(JSON.stringify(output13), JSON.stringify(input13));

    let input14 = new Uint16Array([0,1,2]);
    let output14 = ani_test.unionTest( input14 ) as Uint16Array;
    assertEQ(JSON.stringify(output14), JSON.stringify(input14));
    output14 = ani_test.unionTest2( input14 ) as Uint16Array;
    assertEQ(JSON.stringify(output14), JSON.stringify(input14));

    let input15 = new Int16Array([0,1,2]);
    let output15 = ani_test.unionTest( input15 ) as Int16Array;
    assertEQ(JSON.stringify(output15), JSON.stringify(input15));
    output15 = ani_test.unionTest2( input15 ) as Int16Array;
    assertEQ(JSON.stringify(output15), JSON.stringify(input15));

    let input16 = new Uint32Array([0,1,2]);
    let output16 = ani_test.unionTest( input16 ) as Uint32Array;
    assertEQ(JSON.stringify(output16), JSON.stringify(input16));
    output16 = ani_test.unionTest2( input16 ) as Uint32Array;
    assertEQ(JSON.stringify(output16), JSON.stringify(input16));

    let input17 = new Int32Array([0,1,2]);
    let output17 = ani_test.unionTest( input17 ) as Int32Array;
    assertEQ(JSON.stringify(output17), JSON.stringify(input17));
    output17 = ani_test.unionTest2( input17 ) as Int32Array;
    assertEQ(JSON.stringify(output17), JSON.stringify(input17));
}

function array_buffer_test (): void
{
    let input1 = ArrayBuffer.from([1,2,3,4]);
    let output1 = ani_test.arrayBufferTest(input1);
    assertEQ(input1.at(0), output1.at(0));
    assertEQ(input1.at(1), output1.at(1));
    assertEQ(input1.at(2), output1.at(2));
    assertEQ(input1.at(3), output1.at(3));

    let input2 = ArrayBuffer.from([1,2,3]);
    let output2 = ani_test.changeArrayBuffer(input2);
    assertEQ(input2.at(0), 2);
    assertEQ(input2.at(1), 3);
    assertEQ(input2.at(2), 4);
    assertEQ(output2.at(0), 2);
    assertEQ(output2.at(1), 3);
    assertEQ(output2.at(2), 4);

    let output3 = ani_test.createArrayBuffer();
    assertEQ(output3.at(0), 1);
    assertEQ(output3.at(1), 2);
    assertEQ(output3.at(2), 3);
    assertEQ(output3.at(3), 4);
}

function type_array_test(): void
{
    const input1 = new Uint8Array([1,2,3,4,5]);
    let output1 = ani_test.uint8ArrayTest(input1);
    assertEQ(JSON.stringify(input1), JSON.stringify(output1));

    const input2 = new Int8Array([1,2,3,4,5]);
    let output2 = ani_test.int8ArrayTest(input2);
    assertEQ(JSON.stringify(input2), JSON.stringify(output2));

    const input3 = new Uint16Array([1,2,3,4,5]);
    let output3 = ani_test.uint16ArrayTest(input3);
    assertEQ(JSON.stringify(input3), JSON.stringify(output3));

    const input4 = new Int16Array([1,2,3,4,5]);
    let output4 = ani_test.int16ArrayTest(input4);
    assertEQ(JSON.stringify(input4), JSON.stringify(output4));

    const input5 = new Uint32Array([1,2,3,4,5]);
    let output5 = ani_test.uint32ArrayTest(input5);
    assertEQ(JSON.stringify(input5), JSON.stringify(output5));

    const input6 = new Int32Array([1,2,3,4,5]);
    let output6 = ani_test.int32ArrayTest(input6);
    assertEQ(JSON.stringify(input6), JSON.stringify(output6));

    let input7 = new Uint8Array([1,2,3]);
    let output7 = ani_test.changeUint8Array(input7);
    assertEQ(JSON.stringify(input7), JSON.stringify(output7));
    assertEQ(input7.at(0), 2);
    assertEQ(input7.at(1), 3);
    assertEQ(input7.at(2), 4);

    let input8 = new Int8Array([1,2,3]);
    let output8 = ani_test.changeInt8Array(input8);
    assertEQ(JSON.stringify(input8), JSON.stringify(output8));
    assertEQ(input8.at(0), 2);
    assertEQ(input8.at(1), 3);
    assertEQ(input8.at(2), 4);

    let input9 = new Uint16Array([1,2,3]);
    let output9 = ani_test.changeUint16Array(input9);
    assertEQ(JSON.stringify(input9), JSON.stringify(output9));
    assertEQ(input9.at(0), 2);
    assertEQ(input9.at(1), 3);
    assertEQ(input9.at(2), 4);

    let input10 = new Int16Array([1,2,3]);
    let output10 = ani_test.changeInt16Array(input10);
    assertEQ(JSON.stringify(input10), JSON.stringify(output10));
    assertEQ(input10.at(0), 2);
    assertEQ(input10.at(1), 3);
    assertEQ(input10.at(2), 4);

    let input11 = new Uint32Array([1,2,3]);
    let output11 = ani_test.changeUint32Array(input11);
    assertEQ(JSON.stringify(input11), JSON.stringify(output11));
    assertEQ(input11.at(0), 2);
    assertEQ(input11.at(1), 3);
    assertEQ(input11.at(2), 4);

    let input12 = new Int32Array([1,2,3]);
    let output12 = ani_test.changeInt32Array(input12);
    assertEQ(JSON.stringify(input12), JSON.stringify(output12));
    assertEQ(input12.at(0), 2);
    assertEQ(input12.at(1), 3);
    assertEQ(input12.at(2), 4);


    const input13 = new Uint8Array([1,2,3,4,5]);
    let output13 = ani_test.createUint8Array(input13);
    assertEQ(JSON.stringify(input13), JSON.stringify(output13));

    const input14 = new Int8Array([1,2,3,4,5]);
    let output14 = ani_test.createInt8Array(input14);
    assertEQ(JSON.stringify(input14), JSON.stringify(output14));

    const input15 = new Uint16Array([1,2,3,4,5]);
    let output15 = ani_test.createUint16Array(input15);
    assertEQ(JSON.stringify(input15), JSON.stringify(output15));

    const input16 = new Int16Array([1,2,3,4,5]);
    let output16 = ani_test.createInt16Array(input16);
    assertEQ(JSON.stringify(input16), JSON.stringify(output16));

    const input17 = new Uint32Array([1,2,3,4,5]);
    let output17 = ani_test.createUint32Array(input17);
    assertEQ(JSON.stringify(input17), JSON.stringify(output17));

    const input18 = new Int32Array([1,2,3,4,5]);
    let output18 = ani_test.createInt32Array(input18);
    assertEQ(JSON.stringify(input18), JSON.stringify(output18));

    let input19: ani_test.ArrayBufferStruct = {
        buffer1: ArrayBuffer.from([1,2,3]),
        buffer2: new Int32Array([1,2,3,4,5]),
    }
    let output19 = ani_test.arrayBufferStrcutTest(input19);
    assertEQ(input19.buffer1.at(0), output19.buffer1.at(0));
    assertEQ(input19.buffer1.at(1), output19.buffer1.at(1));
    assertEQ(input19.buffer1.at(2), output19.buffer1.at(2));
    assertEQ(JSON.stringify(input19.buffer2), JSON.stringify(output19.buffer2));
}

function struct_test():void
{
    let input1: ani_test.Config = {action: ani_test.Action.UPLOAD, url: "www.huawei.com"};
    let output1 = ani_test.structEnum(input1);
    assertEQ(JSON.stringify(input1), JSON.stringify(output1));
}

function callback_test():void
{
    let input1: int = 0;
    ani_test.executeCallback1((data: int) => {
        input1 += data;
    })
    assertEQ(input1, 1);

    let input2: int = 0;
    ani_test.executeCallback2((data: int) => {
        input2 += data;
    })
    assertEQ(input2, 2);

    let input3: int = 0;
    ani_test.executeCallback3((data: int) => {
        input3 += data;
    })
    
    await msSleep(500);
    assertEQ(input3, 3);

    // 在devecho上测试, 将callback抛到主线程执行
    // let input4: int = 0;
    // ani_test.executeCallback4((data: int) => {
    //     input4 += data;
    // })

    let input5: int = 0;
    ani_test.executeAsyncCallback1((err: BusinessError|null, data: int|undefined) => {
        assertEQ(err?.code, 401);
        if (data) {
            input5 += data;
        }
        
    })
    assertEQ(input5, 1);

    let input6: int = 0;
    ani_test.executeAsyncCallback2((err: BusinessError|null, data: int|undefined) => {
        assertEQ(err?.code, 402);

        if (data) {
            input6 += data;
        }
        
    })
    assertEQ(input6, 2);

    let input7: int = 0;
    ani_test.executeAsyncCallback3((err: BusinessError|null, data: int|undefined) => {
        assertEQ(err?.code, 403);

        if (data) {
            input7 += data;
        }
        
    })
    
    await msSleep(500);
    assertEQ(input7, 3);

    // 在devecho上测试, 将callback抛到主线程执行
    // let input8: int = 0;
    // ani_test.executeAsyncCallback4((err: BusinessError|null, data: int|undefined) => {
    //     console.log("[ani_test] executeCallback4 data = %{public}d", data);
    //     assertEQ(err?.code, 404);
    //     if (data) {
    //        input8 += data;
    //     }
    // })


    let input9: int = 0;
    ani_test.executeErrorCallback1((err: BusinessError) => {
        assertEQ(err.code, 401);
        input9 += 1;
    })
    assertEQ(input9, 1);

    let input10: int = 0;
    ani_test.executeErrorCallback2((err: BusinessError) => {
        assertEQ(err.code, 402);
        input10 += 2 
    })
    assertEQ(input10, 2);

    let input11: int = 0;
    ani_test.executeErrorCallback3((err: BusinessError) => {
        assertEQ(err.code, 403);
        input11 += 3;
    })
    
    await msSleep(500);
    assertEQ(input11, 3);


    // 在devecho上测试, 将callback抛到主线程执行
    // let input12: int = 0;
    // ani_test.executeErrorCallback4((err: BusinessError) => {
    //     assertEQ(err.code, 404);
    //     input12 += 4;
    // })

    let input13: int = 0;
    ani_test.executeCallback5((data: ArrayBuffer) => {
        input13 += data.at(0) + data.at(1) + data.at(2);
    })
    await msSleep(500);
    assertEQ(input13, 6);

    let input14: double = 0;
    ani_test.executeCallback6((data: Uint32Array) => {
        input14 += data.at(0) as double;
        input14 += data.at(1) as double;
        input14 += data.at(2) as double;
    })
    await msSleep(500);
    assertEQ(input14, 60);


    let input15 = 'hello ';
    ani_test.executeAniRefCallback1((data: string) => {
        input15 += data;
    })
    assertEQ(input15, 'hello rust');

    let input16 = 'hello ';
    ani_test.executeAniRefCallback2((data: string) => {
        input16 += data;
    })
    
    await msSleep(500);
    assertEQ(input16, 'hello rust');

    let input17 = 'hello ';
    ani_test.executeAniRefCallback3((data: ani_test.AniRefStruct) => {
        input17 += data.aniObjString;
        input17 += data.aniString;
    })
    assertEQ(input17, 'hello rust cpp');

    // 在devecho上测试, 将callback抛到主线程执行
    //ani_test.executeAniRefCallback4((data: ani_test.AniRefStruct) => {
    //    input18 += data.aniObjString;
    //    input18 += data.aniString;
    //})

    let output19 = 0
    ani_test.executeMultiCallbacks(
        (data: int) => {
            output19 += data;
        }, 
        (err: BusinessError|null, data: int|undefined) => {
            if (data) {
                output19 += data as int;
            }
        }, 
        (err: BusinessError) => {
            output19 += err.code;
        }
    )
    assertEQ(output19, 6);

    ani_test.executeThrowErrorCallback1((data: int) => {
        throw new Error('[AniRs] i am throw error 1')
    })

    ani_test.executeThrowErrorCallback2((err: BusinessError|null, data: int|undefined) => {
        throw new Error('[AniRs] i am throw error 2')
    })

    ani_test.sendEventTest1((data: int) => {
        console.log('%{public}s', 'sendEventTest1 data = ' + JSON.stringify(data));
    })

    ani_test.sendEventTest2((data: int) => {
        console.log('%{public}s', 'sendEventTest2 data = ' + JSON.stringify(data));
    })
}

function business_test()
{
    try {
        ani_test.businessErrorTest();
        assertEQ(true, false);
    } catch (err: BusinessError) {
        assertEQ(err.code, 201);
        assertEQ(err.message, 'Permission denied');
    }
}

function json_test()
{
    let input1: Record<string, number> = {
        'x': 1,
        'y': 2,
    }
    let output1 = ani_test.jsonStringifyTest1(input1)
    assertEQ(output1, '{"x":1,"y":2}');

    let input2: Record<string, Record<string, number>> = {
        'x': {
            'xx': 1
        },
        'y': {
            'yy': 2
        },
    }
    let output2 = ani_test.jsonStringifyTest1(input2)
    assertEQ(output2, '{"x":{"xx":1},"y":{"yy":2}}');

    let input3: Record<string, Record<string, Record<string, number>>> = {
        'x': {
            'xx': {
                'xxx1':1,
                'xxx2':2
            }
        },
        'y': {
            'yy': {
                'yyy1':3,
                'yyy2':4,
            }
        },
    }
    let output3 = ani_test.jsonStringifyTest1(input3)
    assertEQ(output3, '{"x":{"xx":{"xxx1":1,"xxx2":2}},"y":{"yy":{"yyy1":3,"yyy2":4}}}');

    let input4 = '{"x":1,"y":2}'
    let output4 = ani_test.jsonParseTest1(input4)
    assertEQ(JSON.stringify(output4), input4)

    let input5 = '{"x":{"xx":1},"y":{"yy":2}}'
    let output5 = ani_test.jsonParseTest1(input5)
    assertEQ(JSON.stringify(output5), input5)

    let input6 = '{"x":{"xx":{"xxx1":1,"xxx2":2}},"y":{"yy":{"yyy1":3,"yyy2":4}}}'
    let output6 = ani_test.jsonParseTest1(input6)
    assertEQ(JSON.stringify(output6), input6)

    let input7: Record<string, Record<string, Record<string, number>>> = {
        'x': {
            'xx': {
                'xxx1':1,
                'xxx2':2
            }
        },
        'y': {
            'yy': {
                'yyy1':3,
                'yyy2':4,
            }
        },
    }
    let output7 = ani_test.jsonSerDeserTest(input7)
    assertEQ(JSON.stringify(input7), JSON.stringify(output7))

    let input8: number = 0
    ani_test.executeJsonCallback1((data: Record<string, number>) => {
        input8 += data.get('x') as number
        input8 += data.get('y') as number
    })
    assertEQ(input8, 3)

    let input9: number = 0
    ani_test.executeJsonCallback1((data: Record<string, number>) => {
        input9 += data.get('x') as number
        input9 += data.get('y') as number
    })
    await msSleep(500);
    assertEQ(input9, 3)

    let input10: ani_test.HttpRequestOptions = {
        extraData: 'hello',
        expectDataType: ani_test.HttpDataType.STRING
    }
    let output10 = ani_test.jsonRequestTest(input10);
    assertEQ(output10, 'hello')

    let input11: ani_test.HttpRequestOptions = {
        extraData: input7,
        expectDataType: ani_test.HttpDataType.OBJECT
    }
    let output11 = ani_test.jsonRequestTest(input11);
    assertEQ(output11, '{"x":{"xx":{"xxx1":1,"xxx2":2}},"y":{"yy":{"yyy1":3,"yyy2":4}}}')

    let input12: ani_test.HttpRequestOptions = {
        extraData: ArrayBuffer.from([48,49,50,51]),
        expectDataType: ani_test.HttpDataType.ARRAY_BUFFER
    }
    let output12 = ani_test.jsonRequestTest(input12);
    assertEQ(output12, '0123')

    let output13: string = ''
    ani_test.jsonResponseTest1((err: BusinessError|null, data: ani_test.HttpResponse|undefined) => {
        if (data?.resultType == ani_test.HttpDataType.STRING) {
            output13 = data?.result as string
        }
    }, 0)
    assertEQ(output13, 'hello world')

    let output14: Record<string, Record<string, Record<string, number>>> = {}
    ani_test.jsonResponseTest1((err: BusinessError|null, data: ani_test.HttpResponse|undefined) => {
        if (data?.resultType == ani_test.HttpDataType.OBJECT) {
            output14 = data?.result as Record<string, Record<string, Record<string, number>>>
        }
    }, 1)
    assertEQ(JSON.stringify(output14), '{"x":{"xx":{"xxx1":1,"xxx2":2}},"y":{"yy":{"yyy1":3,"yyy2":4}}}')

    let output15: ArrayBuffer = new ArrayBuffer(3)
    ani_test.jsonResponseTest1((err: BusinessError|null, data: ani_test.HttpResponse|undefined) => {
        if (data?.resultType == ani_test.HttpDataType.ARRAY_BUFFER) {
            output15 = data?.result as ArrayBuffer
        }
    }, 2)
    assertEQ(output15.at(0), 48);
    assertEQ(output15.at(1), 49);
    assertEQ(output15.at(2), 50);

    // 在devecho上测试, 将callback抛到主线程执行
    //ani_test.jsonResponseTest2((err: BusinessError|null, data: ani_test.HttpResponse|undefined) => {
    //    console.log('%{public}s', ' data = ' + JSON.stringify(data));
    //}, 0)
    //ani_test.jsonResponseTest2((err: BusinessError|null, data: ani_test.HttpResponse|undefined) => {
    //    console.log('%{public}s', ' data = ' + JSON.stringify(data));
    //}, 1)
    //ani_test.jsonResponseTest2((err: BusinessError|null, data: ani_test.HttpResponse|undefined) => {
    //    console.log('%{public}s', ' data = ' + JSON.stringify(data));
    //}, 2)

}