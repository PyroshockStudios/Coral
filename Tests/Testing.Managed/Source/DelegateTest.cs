using System;
using System.Runtime.InteropServices;

namespace Testing.Managed;

[AttributeUsage(AttributeTargets.Delegate)]
public class DummyDelegateAttribute : Attribute
{
    public int SomeValue;
}


[DummyDelegate(SomeValue = 2)]
public delegate string FooDelegate(int num);
public delegate string MyDelegate(FooDelegate @delegate);

public class DelegateTest
{
    public static FooDelegate GetFooDelegate()
    {
        return (x) => { return $"Hello {x}"; };
    }
    public static MyDelegate GetMyDelegate(int arg)
    {
        return (x) => { return $"Knock Knock... {x(arg)}"; };
    }
}