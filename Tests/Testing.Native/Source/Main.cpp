#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <chrono>
#include <functional>
#include <ranges>

#include <Coral/HostInstance.hpp>
#include <Coral/DotnetServices.hpp>
#include <Coral/GC.hpp>
#include <Coral/NativeArray.hpp>
#include <Coral/Attribute.hpp>
#include <Coral/String.hpp>
#include <Coral/Array.hpp>

static Coral::Type g_TestsType;

static void ExceptionCallback(Coral::StdStringView InMessage)
{
    std::cout << "\033[1;31m " << "Unhandled native exception: " << InMessage << "\033[0m\n";
}

static char8_t SByteMarshalIcall(char8_t InValue) { return InValue * 2; }
static uint8_t ByteMarshalIcall(uint8_t InValue) { return InValue * 2; }
static int16_t ShortMarshalIcall(int16_t InValue) { return InValue * 2; }
static uint16_t UShortMarshalIcall(uint16_t InValue) { return InValue * 2; }
static int32_t IntMarshalIcall(int32_t InValue) { return InValue * 2; }
static uint32_t UIntMarshalIcall(uint32_t InValue) { return InValue * 2; }
static int64_t LongMarshalIcall(int64_t InValue) { return InValue * 2; }
static uint64_t ULongMarshalIcall(uint64_t InValue) { return InValue * 2; }
static float FloatMarshalIcall(float InValue) { return InValue * 2.0f; }
static double DoubleMarshalIcall(double InValue) { return InValue * 2.0; }
static bool BoolMarshalIcall(bool InValue)
{
    std::cout << "C++: " << (uint32_t)InValue << std::endl;
    return !InValue;
}
static int32_t* IntPtrMarshalIcall(int32_t* InValue)
{
    *InValue *= 2;
    return InValue;
}
static Coral::NativeString StringMarshalIcall(Coral::NativeString InStr)
{
    return InStr;
}
static void StringMarshalIcall2(Coral::NativeString InStr)
{
    std::cout << std::string(InStr) << std::endl;
}
static bool TypeMarshalIcall(Coral::ReflectionType InReflectionType)
{
    const Coral::Type& type = InReflectionType;
    return type == g_TestsType;
}

struct DummyStruct
{
    int32_t X;
    float Y;
    int32_t Z;
};
static DummyStruct DummyStructMarshalIcall(DummyStruct InStruct)
{
    InStruct.X *= 2;
    InStruct.Y *= 2.0f;
    InStruct.Z *= 2;
    return InStruct;
}

static DummyStruct* DummyStructPtrMarshalIcall(DummyStruct* InStruct)
{
    InStruct->X *= 2;
    InStruct->Y *= 2.0f;
    InStruct->Z *= 2;
    return InStruct;
}

static Coral::NativeArray<int32_t> EmptyArrayIcall()
{
    std::vector<int32_t> empty;
    return Coral::NativeArray<int32_t>::New(empty);
}

static Coral::NativeArray<float> FloatArrayIcall()
{
    std::vector<float> floats = { 5.0f, 10.0f, 15.0f, 50.0f };
    return Coral::NativeArray<float>::New(floats);
}

static Coral::Object instance;
static Coral::Object NativeInstanceIcall()
{
    return instance;
}

static void RegisterTestInternalCalls(Coral::Assembly& InAssembly)
{
    InAssembly.AddInternalCall("Testing.Managed.Tests", "SByteMarshalIcall", reinterpret_cast<void*>(&SByteMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "ByteMarshalIcall", reinterpret_cast<void*>(&ByteMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "ShortMarshalIcall", reinterpret_cast<void*>(&ShortMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "UShortMarshalIcall", reinterpret_cast<void*>(&UShortMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "IntMarshalIcall", reinterpret_cast<void*>(&IntMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "UIntMarshalIcall", reinterpret_cast<void*>(&UIntMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "LongMarshalIcall", reinterpret_cast<void*>(&LongMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "ULongMarshalIcall", reinterpret_cast<void*>(&ULongMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "FloatMarshalIcall", reinterpret_cast<void*>(&FloatMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "DoubleMarshalIcall", reinterpret_cast<void*>(&DoubleMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "BoolMarshalIcall", reinterpret_cast<void*>(&BoolMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "IntPtrMarshalIcall", reinterpret_cast<void*>(&IntPtrMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "StringMarshalIcall", reinterpret_cast<void*>(&StringMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "StringMarshalIcall2", reinterpret_cast<void*>(&StringMarshalIcall2));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "DummyStructMarshalIcall", reinterpret_cast<void*>(&DummyStructMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "DummyStructPtrMarshalIcall", reinterpret_cast<void*>(&DummyStructPtrMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "TypeMarshalIcall", reinterpret_cast<void*>(&TypeMarshalIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "EmptyArrayIcall", reinterpret_cast<void*>(&EmptyArrayIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "FloatArrayIcall", reinterpret_cast<void*>(&FloatArrayIcall));
    InAssembly.AddInternalCall("Testing.Managed.Tests", "NativeInstanceIcall", reinterpret_cast<void*>(&NativeInstanceIcall));
}

static Coral::Method SByteTest_Fn;
static Coral::Method ByteTest_Fn;
static Coral::Method ShortTest_Fn;
static Coral::Method UShortTest_Fn;
static Coral::Method IntTest_Fn;
static Coral::Method UIntTest_Fn;
static Coral::Method LongTest_Fn;
static Coral::Method ULongTest_Fn;
static Coral::Method FloatTest_Fn;
static Coral::Method DoubleTest_Fn;
static Coral::Method BoolTest_Fn;
static Coral::Method IntPtrTest_Fn;
static Coral::Method StringTest_Fn;
static Coral::Method ArrayTest_Fn;
static Coral::Method ObjectTest_Fn;
static Coral::Method DummyStructTest_Fn;
static Coral::Method DummyStructPtrTest_Fn;
static Coral::Method DummyStructPtrTestBad_Fn;
static Coral::Method OverloadIntTest_Fn;
static Coral::Method OverloadFloatTest_Fn;

struct Test
{
    std::string Name;
    std::function<bool()> Func;
};
static std::vector<Test> tests;

static void RegisterTest(Coral::StdStringView InName, std::function<bool()> InFunc)
{
    tests.push_back(Test { std::string(InName), std::move(InFunc) });
}

static void RegisterMemberMethodTests(Coral::Object& InObject)
{
    using Coral::MethodParams;

    SByteTest_Fn = InObject.GetType().GetMethod("SByteTest");
    ByteTest_Fn = InObject.GetType().GetMethod("ByteTest");
    ShortTest_Fn = InObject.GetType().GetMethod("ShortTest");
    UShortTest_Fn = InObject.GetType().GetMethod("UShortTest");
    IntTest_Fn = InObject.GetType().GetMethod("IntTest");
    UIntTest_Fn = InObject.GetType().GetMethod("UIntTest");
    LongTest_Fn = InObject.GetType().GetMethod("LongTest");
    ULongTest_Fn = InObject.GetType().GetMethod("ULongTest");
    FloatTest_Fn = InObject.GetType().GetMethod("FloatTest");
    DoubleTest_Fn = InObject.GetType().GetMethod("DoubleTest");
    BoolTest_Fn = InObject.GetType().GetMethod("BoolTest");
    IntPtrTest_Fn = InObject.GetType().GetMethod("IntPtrTest");
    StringTest_Fn = InObject.GetType().GetMethod("StringTest");
    ArrayTest_Fn = InObject.GetType().GetMethod("ArrayTest");
    ObjectTest_Fn = InObject.GetType().GetMethod("ObjectTest");
    DummyStructTest_Fn = InObject.GetType().GetMethod("DummyStructTest");
    DummyStructPtrTest_Fn = InObject.GetType().GetMethod("DummyStructPtrTestGood");
    DummyStructPtrTestBad_Fn = InObject.GetType().GetMethod("DummyStructPtrTestWrong");
    OverloadIntTest_Fn = InObject.GetType().GetMethodByParamTypes("OverloadTest", { Coral::Type::IntType() });
    OverloadFloatTest_Fn = InObject.GetType().GetMethodByParamTypes("OverloadTest", { Coral::Type::FloatType() });

    RegisterTest("SByteTest", [&InObject]() mutable
    {
        return InObject.InvokeMethod<int8_t>(SByteTest_Fn, MethodParams { int8_t(10) }) == 20;
    });
    RegisterTest("ByteTest", [&InObject]() mutable
    {
        return InObject.InvokeMethod<uint8_t>(ByteTest_Fn, MethodParams { uint8_t(10) }) == 20;
    });
    RegisterTest("ShortTest", [&InObject]() mutable
    {
        return InObject.InvokeMethod<int16_t>(ShortTest_Fn, MethodParams { int16_t(10) }) == 20;
    });
    RegisterTest("UShortTest", [&InObject]() mutable
    {
        return InObject.InvokeMethod<uint16_t>(UShortTest_Fn, MethodParams { uint16_t(10) }) == 20;
    });
    RegisterTest("IntTest", [&InObject]() mutable
    {
        return InObject.InvokeMethod<int32_t>(IntTest_Fn, MethodParams { int32_t(10) }) == 20;
    });
    RegisterTest("UIntTest", [&InObject]() mutable
    {
        return InObject.InvokeMethod<uint32_t>(UIntTest_Fn, MethodParams { uint32_t(10) }) == 20;
    });
    RegisterTest("LongTest", [&InObject]() mutable
    {
        return InObject.InvokeMethod<int64_t>(LongTest_Fn, MethodParams { int64_t(10) }) == 20;
    });
    RegisterTest("ULongTest", [&InObject]() mutable
    {
        return InObject.InvokeMethod<uint64_t>(ULongTest_Fn, MethodParams { uint64_t(10) }) == 20;
    });
    RegisterTest("FloatTest", [&InObject]() mutable
    {
        return InObject.InvokeMethod<float>(FloatTest_Fn, MethodParams { float(10.0f) }) - 20.0f < 0.001f;
    });
    RegisterTest("DoubleTest", [&InObject]() mutable
    {
        return InObject.InvokeMethod<double>(DoubleTest_Fn, MethodParams { double(10.0) }) - 20.0 < 0.001;
    });
    RegisterTest("BoolTest", [&InObject]() mutable
    {
        return InObject.InvokeMethod<Coral::Bool32>(BoolTest_Fn, MethodParams { Coral::Bool32(false) });
    });
    RegisterTest("IntPtrTest", [&InObject]() mutable
    {
        int32_t v = 10;
        return *InObject.InvokeMethod<int32_t*>(IntPtrTest_Fn, MethodParams { (int32_t*)(&v) }) == 50;
    });
    RegisterTest("StringTest", [&InObject]() mutable
    {
        Coral::ScopedNativeString str = InObject.InvokeMethod<Coral::NativeString>(StringTest_Fn, MethodParams { Coral::NativeString::New("Hello") });
        return str == Coral::StdStringView("Hello, World!");
    });
    RegisterTest("ManagedStringTest", [&InObject]() mutable
    {
        Coral::String str = InObject.InvokeMethod<Coral::String>(StringTest_Fn, MethodParams { Coral::String::CreateStringUtf8("Hello") });
        return str.GetStringUtf8() == "Hello, World!";
    });
    RegisterTest("ArrayTest", [&InObject]() mutable
    {
        int result = InObject.InvokeMethod<int32_t>(ArrayTest_Fn, MethodParams { Coral::NativeArray<int32_t>::New({ 5, 2, -1, 8 }) });
        return result == 14;
    });
    RegisterTest("ManagedArrayTest", [&InObject]() mutable
    {
        Coral::Array array = Coral::Array::CreateEmptyArray(4, Coral::Type::IntType());
        array.SetElement(5, 0);
        array.SetElement(2, 1);
        array.SetElement(-1, 2);
        array.SetElement(Coral::Object::Box(8, Coral::Type::IntType()), 3); // try boxed value too
        int result = InObject.InvokeMethod<int32_t>(ArrayTest_Fn, MethodParams { array });
        return result == 14;
    });
    RegisterTest("ObjectTest", [&InObject]() mutable
    {
        Coral::ScopedNativeString str = InObject.InvokeMethod<Coral::NativeString>(ObjectTest_Fn, MethodParams { InObject });
        return str == (Coral::StdStringView) "Type:MemberMethodTest";
    });

    RegisterTest("DummyStructTest", [&InObject]() mutable
    {
        DummyStruct value = {
            .X = 10,
            .Y = 10.0f,
            .Z = 10,
        };
        auto result = InObject.InvokeMethod<DummyStruct>(DummyStructTest_Fn, MethodParams { value });
        return result.X == 20 && result.Y - 20.0f < 0.001f && result.Z == 20;
    });
    RegisterTest("DummyStructPtrTestGood", [&InObject]() mutable
    {
        DummyStruct value = {
            .X = 10,
            .Y = 10.0f,
            .Z = 10,
        };
        auto* result = InObject.InvokeMethod<DummyStruct*>(DummyStructPtrTest_Fn, MethodParams { &value });
        return result->X == 20 && result->Y - 20.0f < 0.001f && result->Z == 20;
    });
    RegisterTest("DummyStructPtrTestBad", [&InObject]() mutable
    {
        DummyStruct value = {
            .X = 10,
            .Y = 10.0f,
            .Z = 10,
        };
        Coral::Object exception;
        auto* result = InObject.InvokeMethod<DummyStruct*>(DummyStructPtrTestBad_Fn, MethodParams { &value }, &exception);
        return exception.IsValid() && result == nullptr;
    });

    RegisterTest("OverloadTest (Int)", [&InObject]() mutable
    {
        return InObject.InvokeMethod<int32_t>(OverloadIntTest_Fn, MethodParams { 50 }) == 1050;
    });

    RegisterTest("OverloadTest (Float)", [&InObject]() mutable
    {
        return InObject.InvokeMethod<float>(OverloadFloatTest_Fn, MethodParams { 5.0f }) == 15.0f;
    });
}

static void RegisterFieldMarshalTests(Coral::Object& InObject)
{
    // ---- Fields ----
    RegisterTest("SByteFieldTest", [&InObject]() mutable
    {
        auto field = InObject.GetType().GetField("SByteFieldTest");
        auto value = InObject.GetFieldValue<char8_t>(field);
        if (value != 10) return false;
        InObject.SetFieldValue<char8_t>(field, 20);
        value = InObject.GetFieldValue<char8_t>(field);
        return value == 20;
    });

    RegisterTest("ByteFieldTest", [&InObject]() mutable
    {
        auto field = InObject.GetType().GetField("ByteFieldTest");
        auto value = InObject.GetFieldValue<uint8_t>(field);
        if (value != 10) return false;
        InObject.SetFieldValue<uint8_t>(field, 20);
        value = InObject.GetFieldValue<uint8_t>(field);
        return value == 20;
    });

    RegisterTest("ShortFieldTest", [&InObject]() mutable
    {
        auto field = InObject.GetType().GetField("ShortFieldTest");
        auto value = InObject.GetFieldValue<int16_t>(field);
        if (value != 10) return false;
        InObject.SetFieldValue<int16_t>(field, 20);
        value = InObject.GetFieldValue<int16_t>(field);
        return value == 20;
    });

    RegisterTest("UShortFieldTest", [&InObject]() mutable
    {
        auto field = InObject.GetType().GetField("UShortFieldTest");
        auto value = InObject.GetFieldValue<uint16_t>(field);
        if (value != 10) return false;
        InObject.SetFieldValue<uint16_t>(field, 20);
        value = InObject.GetFieldValue<uint16_t>(field);
        return value == 20;
    });

    RegisterTest("IntFieldTest", [&InObject]() mutable
    {
        auto field = InObject.GetType().GetField("IntFieldTest");
        auto value = InObject.GetFieldValue<int32_t>(field);
        if (value != 10) return false;
        InObject.SetFieldValue<int32_t>(field, 20);
        value = InObject.GetFieldValue<int32_t>(field);
        return value == 20;
    });

    RegisterTest("UIntFieldTest", [&InObject]() mutable
    {
        auto field = InObject.GetType().GetField("UIntFieldTest");
        auto value = InObject.GetFieldValue<uint32_t>(field);
        if (value != 10) return false;
        InObject.SetFieldValue<uint32_t>(field, 20);
        value = InObject.GetFieldValue<uint32_t>(field);
        return value == 20;
    });

    RegisterTest("LongFieldTest", [&InObject]() mutable
    {
        auto field = InObject.GetType().GetField("LongFieldTest");
        auto value = InObject.GetFieldValue<int64_t>(field);
        if (value != 10) return false;
        InObject.SetFieldValue<int64_t>(field, 20);
        value = InObject.GetFieldValue<int64_t>(field);
        return value == 20;
    });

    RegisterTest("ULongFieldTest", [&InObject]() mutable
    {
        auto field = InObject.GetType().GetField("ULongFieldTest");
        auto value = InObject.GetFieldValue<uint64_t>(field);
        if (value != 10) return false;
        InObject.SetFieldValue<uint64_t>(field, 20);
        value = InObject.GetFieldValue<uint64_t>(field);
        return value == 20;
    });

    RegisterTest("FloatFieldTest", [&InObject]() mutable
    {
        auto field = InObject.GetType().GetField("FloatFieldTest");
        auto value = InObject.GetFieldValue<float>(field);
        if (std::abs(value - 10.0f) > 0.001f) return false;
        InObject.SetFieldValue<float>(field, 20);
        value = InObject.GetFieldValue<float>(field);
        return std::abs(value - 20.0f) < 0.001f;
    });

    RegisterTest("DoubleFieldTest", [&InObject]() mutable
    {
        auto field = InObject.GetType().GetField("DoubleFieldTest");
        auto value = InObject.GetFieldValue<double>(field);
        if (std::abs(value - 10.0) > 0.001) return false;
        InObject.SetFieldValue<double>(field, 20);
        value = InObject.GetFieldValue<double>(field);
        return std::abs(value - 20.0) < 0.001;
    });

    RegisterTest("BoolFieldTest", [&InObject]() mutable
    {
        auto field = InObject.GetType().GetField("BoolFieldTest");
        auto value = InObject.GetFieldValue<bool>(field);
        if (value != false) return false;
        InObject.SetFieldValue<bool>(field, true);
        value = InObject.GetFieldValue<bool>(field);
        return static_cast<bool>(value);
    });

    RegisterTest("StringFieldTest", [&InObject]() mutable
    {
        auto field = InObject.GetType().GetField("StringFieldTest");
        auto value = InObject.GetFieldValue<std::string>(field);
        if (value != "Hello") return false;
        InObject.SetFieldValue<std::string>(field, "Hello, World!");
        value = InObject.GetFieldValue<std::string>(field);
        return value == "Hello, World!";
    });

    // ---- Properties ----
    RegisterTest("SBytePropertyTest", [&InObject]() mutable
    {
        auto prop = InObject.GetType().GetProperty("SBytePropertyTest");
        auto value = InObject.GetPropertyValue<char8_t>(prop);
        if (value != 10) return false;
        InObject.SetPropertyValue<char8_t>(prop, 20);
        value = InObject.GetPropertyValue<char8_t>(prop);
        return value == 20;
    });

    RegisterTest("BytePropertyTest", [&InObject]() mutable
    {
        auto prop = InObject.GetType().GetProperty("BytePropertyTest");
        auto value = InObject.GetPropertyValue<uint8_t>(prop);
        if (value != 10) return false;
        InObject.SetPropertyValue<uint8_t>(prop, 20);
        value = InObject.GetPropertyValue<uint8_t>(prop);
        return value == 20;
    });

    RegisterTest("ShortPropertyTest", [&InObject]() mutable
    {
        auto prop = InObject.GetType().GetProperty("ShortPropertyTest");
        auto value = InObject.GetPropertyValue<int16_t>(prop);
        if (value != 10) return false;
        InObject.SetPropertyValue<int16_t>(prop, 20);
        value = InObject.GetPropertyValue<int16_t>(prop);
        return value == 20;
    });

    RegisterTest("UShortPropertyTest", [&InObject]() mutable
    {
        auto prop = InObject.GetType().GetProperty("UShortPropertyTest");
        auto value = InObject.GetPropertyValue<uint16_t>(prop);
        if (value != 10) return false;
        InObject.SetPropertyValue<uint16_t>(prop, 20);
        value = InObject.GetPropertyValue<uint16_t>(prop);
        return value == 20;
    });

    RegisterTest("IntPropertyTest", [&InObject]() mutable
    {
        auto prop = InObject.GetType().GetProperty("IntPropertyTest");
        auto value = InObject.GetPropertyValue<int32_t>(prop);
        if (value != 10) return false;
        InObject.SetPropertyValue<int32_t>(prop, 20);
        value = InObject.GetPropertyValue<int32_t>(prop);
        return value == 20;
    });

    RegisterTest("UIntPropertyTest", [&InObject]() mutable
    {
        auto prop = InObject.GetType().GetProperty("UIntPropertyTest");
        auto value = InObject.GetPropertyValue<uint32_t>(prop);
        if (value != 10) return false;
        InObject.SetPropertyValue<uint32_t>(prop, 20);
        value = InObject.GetPropertyValue<uint32_t>(prop);
        return value == 20;
    });

    RegisterTest("LongPropertyTest", [&InObject]() mutable
    {
        auto prop = InObject.GetType().GetProperty("LongPropertyTest");
        auto value = InObject.GetPropertyValue<int64_t>(prop);
        if (value != 10) return false;
        InObject.SetPropertyValue<int64_t>(prop, 20);
        value = InObject.GetPropertyValue<int64_t>(prop);
        return value == 20;
    });

    RegisterTest("ULongPropertyTest", [&InObject]() mutable
    {
        auto prop = InObject.GetType().GetProperty("ULongPropertyTest");
        auto value = InObject.GetPropertyValue<uint64_t>(prop);
        if (value != 10) return false;
        InObject.SetPropertyValue<uint64_t>(prop, 20);
        value = InObject.GetPropertyValue<uint64_t>(prop);
        return value == 20;
    });

    RegisterTest("FloatPropertyTest", [&InObject]() mutable
    {
        auto prop = InObject.GetType().GetProperty("FloatPropertyTest");
        auto value = InObject.GetPropertyValue<float>(prop);
        if (std::abs(value - 10.0f) > 0.001f) return false;
        InObject.SetPropertyValue<float>(prop, 20);
        value = InObject.GetPropertyValue<float>(prop);
        return std::abs(value - 20.0f) < 0.001f;
    });

    RegisterTest("DoublePropertyTest", [&InObject]() mutable
    {
        auto prop = InObject.GetType().GetProperty("DoublePropertyTest");
        auto value = InObject.GetPropertyValue<double>(prop);
        if (std::abs(value - 10.0) > 0.001) return false;
        InObject.SetPropertyValue<double>(prop, 20);
        value = InObject.GetPropertyValue<double>(prop);
        return std::abs(value - 20.0) < 0.001;
    });

    RegisterTest("BoolPropertyTest", [&InObject]() mutable
    {
        auto prop = InObject.GetType().GetProperty("BoolPropertyTest");
        auto value = InObject.GetPropertyValue<Coral::Bool32>(prop);
        if (value != false) return false;
        InObject.SetPropertyValue<Coral::Bool32>(prop, true);
        value = InObject.GetPropertyValue<Coral::Bool32>(prop);
        return static_cast<bool>(value);
    });

    RegisterTest("StringPropertyTest", [&InObject]() mutable
    {
        auto prop = InObject.GetType().GetProperty("StringPropertyTest");
        std::string value = InObject.GetPropertyValue<Coral::NativeString>(prop);
        if (value != "Hello") return false;
        InObject.SetPropertyValue(prop, Coral::NativeString::New("Hello, World!"));
        value = InObject.GetPropertyValue<Coral::NativeString>(prop);
        return value == "Hello, World!";
    });
}
static void RegisterDelegateTests(Coral::Object& InObject)
{
    using Coral::MethodParams;

    RegisterTest("CallDelegate", [&InObject]() mutable
    {
        Coral::Object fooDelegate = InObject.GetType().InvokeStaticMethod<Coral::Object>(InObject.GetType().GetMethod("GetFooDelegate", true));
        auto attributes = fooDelegate.GetType().GetAttributes();
        if (attributes.empty()) return false;
        if (attributes[0].GetFieldValue<int32_t>("SomeValue") != 2) return false;
        Coral::Object myDelegate = InObject.GetType().InvokeStaticMethod<Coral::Object>(InObject.GetType().GetMethod("GetMyDelegate", true), MethodParams { 81 });
        Coral::ScopedNativeString str = myDelegate.InvokeDelegate<Coral::NativeString>(MethodParams { fooDelegate });
        return str == (Coral::StdStringView) "Knock Knock... Hello 81";
    });
}

static void RunTests()
{
    size_t passedTests = 0;
    for (size_t i = 0; i < tests.size(); i++)
    {
        const auto& test = tests[i];
        bool result = test.Func();
        if (result)
        {
            std::cout << "\033[1;32m[" << i + 1 << " / " << tests.size() << " (" << test.Name << "): Passed\033[0m\n";
            passedTests++;
        }
        else
        {
            std::cerr << "\033[1;31m[" << i + 1 << " / " << tests.size() << " (" << test.Name << "): Failed\033[0m\n";
        }
    }
    std::cout << "[NativeTest]: Done. " << passedTests << " passed, " << tests.size() - passedTests << " failed.\n";
}

int main([[maybe_unused]] int argc, char** argv)
{
    using Coral::MethodParams;
    auto exeDir = std::filesystem::path(argv[0]).parent_path();
    auto coralDir = exeDir.string();
    Coral::HostSettings settings = {
        .CoralDirectory = coralDir,
        .ExceptionCallback = ExceptionCallback
    };
    Coral::HostInstance hostInstance;
    hostInstance.Initialize(settings);

    //Coral::DotnetServices::RunMSBuild((exeDir.parent_path().parent_path() / "CoralManaged.sln").string());

    std::string testDllPath = exeDir.parent_path().string() + ":" + exeDir.parent_path().parent_path().string();
    auto loadContext = hostInstance.CreateAssemblyLoadContext("TestContext", testDllPath);

    auto assemblyPath = exeDir / "Testing.Managed.dll";
    auto& assembly = loadContext.LoadAssembly(assemblyPath.string());

    RegisterTestInternalCalls(assembly);
    assembly.UploadInternalCalls();

    Coral::Type testsType = assembly.GetType("Testing.Managed.Tests");
    g_TestsType = testsType;
    auto staticMethodTest0 = testsType.GetMethodByParamTypes("StaticMethodTest", { Coral::Type::FloatType() }, true);
    auto staticMethodTest1 = testsType.GetMethodByParamTypes("StaticMethodTest", { Coral::Type::IntType() }, true);
    testsType.InvokeStaticMethod(staticMethodTest0, MethodParams { 50.0f });
    testsType.InvokeStaticMethod(staticMethodTest1, MethodParams { 1000 });

    Coral::Type instanceTestType = assembly.GetType("Testing.Managed.InstanceTest");
    instance = instanceTestType.CreateInstance();
    instance.SetFieldValue(instanceTestType.GetField("X"), 500.0f);

    Coral::Object testsInstance = testsType.CreateInstance();
    testsInstance.InvokeMethod(testsType.GetMethod("RunManagedTests"));
    testsInstance.Destroy();

    Coral::Type fieldTestType = assembly.GetType("Testing.Managed.FieldMarshalTest");
    std::cout << fieldTestType.IsAssignableTo(fieldTestType) << std::endl;

    auto fieldTestObject = fieldTestType.CreateInstance();

    auto dummyClassInstance = assembly.GetType("Testing.Managed.DummyClass").CreateInstance();
    dummyClassInstance.SetFieldValue(dummyClassInstance.GetType().GetField("X"), 500.0f);

    struct DummyStruct
    {
        float X;
    } ds;
    ds.X = 50.0f;
    fieldTestObject.SetFieldValue(fieldTestType.GetField("DummyClassTest"), dummyClassInstance);
    fieldTestObject.SetFieldValue(fieldTestType.GetField("DummyStructTest"), ds);
    fieldTestObject.InvokeMethod(fieldTestType.GetMethod("TestClassAndStruct"));
    dummyClassInstance.Destroy();

    for (auto fieldInfo : fieldTestType.GetFields())
    {
        auto attributes = fieldInfo.GetAttributes();
        for (auto attrib : attributes)
        {
            auto attribType = attrib.GetType();

            if (attribType.GetFullName() == "Testing.Managed.DummyAttribute")
                std::cout << attrib.GetFieldValue<float>("SomeValue") << std::endl;
        }
    }

    for (auto propertyInfo : fieldTestType.GetProperties())
    {
        auto attributes = propertyInfo.GetAttributes();
        for (auto attrib : attributes)
        {
            auto attribType = attrib.GetType();

            if (attribType.GetFullName() == "Testing.Managed.DummyAttribute")
                std::cout << attrib.GetFieldValue<float>("SomeValue") << std::endl;
        }
    }

    Coral::Type memberMethodTestType = assembly.GetType("Testing.Managed.MemberMethodTest");

    // for (auto methodInfo : memberMethodTestType.GetMethods())
    // {
    // 	auto& type = methodInfo.GetReturnType();
    // 	auto accessibility = methodInfo.GetAccessibility();
    // 	std::cout << methodInfo.GetName() << ", Returns: " << type.GetFullName() << std::endl;
    // 	const auto& parameterTypes = methodInfo.GetParameterTypes();
    // 	for (const auto& paramType : parameterTypes)
    // 	{
    // 		std::cout << "\t" << paramType->GetFullName() << std::endl;
    // 	}

    // 	auto attributes = methodInfo.GetAttributes();
    // 	for (auto attrib : attributes)
    // 	{
    // 		auto& attribType = attrib.GetType();

    // 		if (attribType.GetFullName() == "Testing.Managed.DummyAttribute")
    // 			std::cout << attrib.GetFieldValue<float>("SomeValue") << std::endl;
    // 	}
    // }

    auto memberMethodTest = memberMethodTestType.CreateInstance();

    Coral::Type delegateTestType = assembly.GetType("Testing.Managed.DelegateTest");
    auto delegateTest = delegateTestType.CreateInstance();

    RegisterFieldMarshalTests(fieldTestObject);
    RegisterMemberMethodTests(memberMethodTest);
    RegisterDelegateTests(delegateTest);
    RunTests();

    memberMethodTest.Destroy();
    fieldTestObject.Destroy();
    delegateTest.Destroy();

    Coral::Type virtualMethodTestType1 = assembly.GetType("Testing.Managed.Override1");
    Coral::Type virtualMethodTestType2 = assembly.GetType("Testing.Managed.Override2");

    auto instance1 = virtualMethodTestType1.CreateInstance();
    auto instance2 = virtualMethodTestType2.CreateInstance();

    instance1.InvokeMethod(virtualMethodTestType1.GetMethod("TestMe"));
    instance2.InvokeMethod(virtualMethodTestType2.GetMethod("TestMe"));

    instance.Destroy();
    instance1.Destroy();
    instance2.Destroy();

    auto loadContext2 = hostInstance.CreateAssemblyLoadContext("ALCTestMulti", testDllPath);
    auto& multiAssembly = loadContext2.LoadAssembly(assemblyPath.string());

    if (multiAssembly.GetType("Testing.Managed.DummyClass") != assembly.GetType("Testing.Managed.DummyClass"))
    {
        std::cout << "\033[1;32mMultiple instances of the same DLL seem to be working\033[0m" << std::endl;
    }
    else
    {
        std::cout << "\033[1;31mType cache is clashing between multiple instances of the same DLL\033[0m" << std::endl;
    }

    hostInstance.UnloadAssemblyLoadContext(loadContext);

    Coral::GC::Collect();

    loadContext = hostInstance.CreateAssemblyLoadContext("ALC2", testDllPath);
    auto& newAssembly = loadContext.LoadAssembly(assemblyPath.string());

    RegisterTestInternalCalls(newAssembly);
    newAssembly.UploadInternalCalls();

    Coral::Type testsType2 = newAssembly.GetType("Testing.Managed.Tests");
    g_TestsType = testsType2;

    Coral::Type instanceTestType2 = newAssembly.GetType("Testing.Managed.InstanceTest");
    instance = instanceTestType2.CreateInstance();
    instance.SetFieldValue(instanceTestType2.GetField("X"), 500.0f);

    Coral::Type multiInheritanceTestType = newAssembly.GetType("Testing.Managed.MultiInheritanceTest");
    std::cout << "Class: " << std::string(multiInheritanceTestType.GetFullName()) << std::endl;
    std::cout << "\tBase: " << std::string(multiInheritanceTestType.GetBaseType().GetFullName()) << std::endl;
    std::cout << "\tInterfaces:" << std::endl;

    const auto& interfaceTypes = multiInheritanceTestType.GetInterfaceTypes();
    for (const auto& type : interfaceTypes)
    {
        std::cout << "\t\t" << std::string(type.GetFullName()) << std::endl;
    }

    Coral::Object testsInstance2 = testsType2.CreateInstance();
    testsInstance2.InvokeMethod(testsType2.GetMethod("RunManagedTests"));
    testsInstance2.Destroy();
    instance.Destroy();

    return 0;
}
