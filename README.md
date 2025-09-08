# Coral

Coral is a C++ wrapper around the .NET HostFXR library, the purpose of Coral is to provide an interface similar to [Mono](https://www.mono-project.com/) when it comes to C++/C# interop, but in a more modern style, and using .NET Core instead of .NET Framework.
This version of Coral is a fork of the [StudioCherno Coral](https://github.com/StudioCherno/Coral), designed to be more efficient (Calling methods, accessing fields and properties now requires caching the MethodInfo/FieldInfo/PropertyInfo), include support for catching exception objects in c++ (similar to mono), adding delegate supports, (un)boxing value types, stronger support for using `object` types in method/field/property/delegate interop, and many other utilities!

The goal of the API is to keep it as simple and flexible as possible, while remaining fast and (relatively) safe.

## Supported Platforms
* Windows x64 (VS2022)
* Linux x64 (Last tested on Ubuntu 22.04)

### Building
Coral uses the CMake build system.
Just add Coral as a subdirectory, and add `Coral` to your target link libraries.

## Compiling
* Coral has been tested to compile with MSVC / CL (Visual Studio 2019 and Visual Studio 2022) and Clang 16+
* It uses C++20 and depends on the .NET SDK being present on the system
* It also contains a config file where you can define a custom standard library to be used `set(CORAL_CUSTOM_CONFIGURATION "your path here")`


## License
Coral is licensed under the [MIT](./LICENSE) license
