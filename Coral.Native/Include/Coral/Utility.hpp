#pragma once

#include "Core.hpp"
#include "String.hpp"
#include <type_traits>

namespace Coral {
    template <template <typename...> class Template, typename T>
    struct is_specialization_of : std::false_type {};

    template <template <typename...> class Template, typename... Args>
    struct is_specialization_of<Template, Template<Args...>> : std::true_type {};

    template <template <typename...> class Template, typename T>
    constexpr inline bool is_specialization_of_v = is_specialization_of<Template, T>::value;


    class Type;
    template<typename TValue> class alignas(8) Array;
    class ManagedObject;

    enum class ManagedType : uint32_t
    {
		Unknown,
		
        Void,

        SByte,
        Byte,
        Short,
        UShort,
        Int,
		UInt,
		Long,
		ULong,

        Float,
        Double,

        Bool,

        Char,

        String,
        Array,
        Object,

        Pointer
    };

    template<typename TArg>
    constexpr ManagedType GetManagedType()
	{
		if constexpr (std::is_pointer_v<std::remove_reference_t<TArg>>)
			return ManagedType::Pointer;
        else if constexpr (std::same_as<TArg, uint8_t> || std::same_as<TArg, std::byte>)
            return ManagedType::Byte;
        else if constexpr (std::same_as<TArg, uint16_t>)
            return ManagedType::UShort;
        else if constexpr (std::same_as<TArg, uint32_t> || (std::same_as<TArg, unsigned long> && sizeof(TArg) == 4))
            return ManagedType::UInt;
        else if constexpr (std::same_as<TArg, uint64_t> || (std::same_as<TArg, unsigned long> && sizeof(TArg) == 8))
            return ManagedType::ULong;
        else if constexpr (std::same_as<TArg, char8_t> || std::same_as<TArg, int8_t>)
            return ManagedType::SByte;
        else if constexpr (std::same_as<TArg, int16_t>)
            return ManagedType::Short;
        else if constexpr (std::same_as<TArg, int32_t> || (std::same_as<TArg, long> && sizeof(TArg) == 4))
            return ManagedType::Int;
        else if constexpr (std::same_as<TArg, int64_t> || (std::same_as<TArg, long> && sizeof(TArg) == 8))
            return ManagedType::Long;
        else if constexpr (std::same_as<TArg, float>)
            return ManagedType::Float;
        else if constexpr (std::same_as<TArg, double>)
            return ManagedType::Double;
        else if constexpr (std::same_as<TArg, bool>)
            return ManagedType::Bool;
        else if constexpr (std::same_as<TArg, Coral::Char>)
            return ManagedType::Char;
        else if constexpr (std::same_as<TArg, Coral::ManagedObject>)
            return ManagedType::Object;
        else if constexpr (is_specialization_of_v<Coral::Array, TArg>)
            return ManagedType::Array;
        else if constexpr (std::same_as<TArg, std::string> || std::same_as<TArg, Coral::String>)
			return ManagedType::String;
		else if constexpr (std::same_as<TArg, void>)
			return ManagedType::Void;
        else
            return ManagedType::Unknown;
    }

    template <typename TArg, size_t TIndex>
    inline void AddToArrayI(const void** InArgumentsArr, ManagedType* InParameterTypes, TArg&& InArg)
    {
        ManagedType managedType = GetManagedType<std::remove_const_t<std::remove_reference_t<TArg>>>();
        InParameterTypes[TIndex] = managedType;

        if constexpr (std::is_pointer_v<std::remove_reference_t<TArg>>)
        {
            InArgumentsArr[TIndex] = reinterpret_cast<const void*>(InArg);
        }
        else
        {
            InArgumentsArr[TIndex] = reinterpret_cast<const void*>(&InArg);
        }
    }

    /*
     * TODO(Emily): Work out a way to allow method invoke to use C++-y types (i.e. `std::string` instead of `Coral::String`).
     * 				See Testing/Main.cpp:StringTest/BoolTest.
     */
    template <typename... TArgs>
    inline void AddToArray(const void** InArgumentsArr, ManagedType* InParameterTypes, TArgs&&... InArgs)
    {
        constexpr size_t N = sizeof...(TArgs);
        (AddToArrayI<TArgs, std::index_sequence_for<TArgs...>::size() - N>(InArgumentsArr, InParameterTypes, std::forward<TArgs>(InArgs)), ...);
    }

    template <typename... TArgs>
    struct MethodParams
    {
        // Default + move semantics
        MethodParams() = default;
        MethodParams(const MethodParams&) = delete;
        MethodParams& operator=(const MethodParams&) = delete;
        MethodParams(MethodParams&&) = default;
        MethodParams& operator=(MethodParams&&) = default;

        // Perfect-forwarding constructor
        template <typename... UArgs>
        explicit MethodParams(UArgs&&... InParameters)
        {
            if constexpr (sizeof...(UArgs) > 0)
            {
                AddToArray(parameterValues, parameterTypes,
                    std::forward<UArgs>(InParameters)...);
                paramCount = sizeof...(UArgs);
            }
        }

    private:
        friend class Type;
        friend class ManagedObject;

        // Arrays always valid; if no params, they're just empty
        const void* parameterValues[sizeof...(TArgs) == 0 ? 1 : sizeof...(TArgs)]{};
        ManagedType parameterTypes[sizeof...(TArgs) == 0 ? 1 : sizeof...(TArgs)]{};
        int32_t paramCount = 0;
    };
}
