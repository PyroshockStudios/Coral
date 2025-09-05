#pragma once

#include "Core.hpp"
#include "StableVector.hpp"

namespace Coral {
    class Type;

    class TypeCache
    {
    public:
        static TypeCache& Get();

        Type* CacheType(Type&& InType);

        Type* GetTypeByName(std::string_view InName) const;

        Type* GetTypeByID(TypeId InTypeID) const;


        static Type* GetType(std::string_view InName) {
            return Get().GetTypeByName(InName);
        }
        static Type* GetType(TypeId InTypeID) {
            return Get().GetTypeByID(InTypeID);
        }

        void Clear();

        static Type* VoidType() { return Get().m_VoidType; }
		static Type* ByteType() { return Get().m_ByteType; }
		static Type* SByteType() { return Get().m_SByteType; }
        static Type* ShortType() { return Get().m_ShortType; }
        static Type* UShortType() { return Get().m_UShortType; }
        static Type* IntType() { return Get().m_IntType; }
        static Type* UIntType() { return Get().m_UIntType; }
        static Type* LongType() { return Get().m_LongType; }
        static Type* ULongType() { return Get().m_ULongType; }
        static Type* FloatType() { return Get().m_FloatType; }
        static Type* DoubleType() { return Get().m_DoubleType; }
        static Type* BoolType() { return Get().m_BoolType; }
        static Type* CharType() { return Get().m_CharType; }
		static Type* StringType() { return Get().m_StringType; }
        static Type* ObjectType() { return Get().m_ObjectType; }
        static Type* IntPtrType() { return Get().m_IntPtrType; }
		static Type* UIntPtrType() { return Get().m_UIntPtrType; }
		static Type* DecimalType() { return Get().m_DecimalType; }
		static Type* DateTimeType() { return Get().m_DateTimeType; }
		static Type* ExceptionType() { return Get().m_ExceptionType; }
		static Type* ArrayType() { return Get().m_ArrayType; }

	private:
        friend class AssemblyLoadContext;

        StableVector<Type> m_Types;
        std::unordered_map<std::string, Type*> m_NameCache;
        std::unordered_map<TypeId, Type*> m_IDCache;

        Type* m_VoidType = {};
        Type* m_ByteType = {};
        Type* m_SByteType = {};
        Type* m_ShortType = {};
        Type* m_UShortType = {};
        Type* m_IntType = {};
        Type* m_UIntType = {};
        Type* m_LongType = {};
        Type* m_ULongType = {};
        Type* m_FloatType = {};
        Type* m_DoubleType = {};
        Type* m_BoolType = {};
        Type* m_CharType = {};
        Type* m_StringType = {};
		Type* m_ObjectType = {};
        Type* m_IntPtrType = {};
        Type* m_UIntPtrType = {};
		Type* m_DecimalType = {};
		Type* m_DateTimeType = {};
		Type* m_ExceptionType = {};
        Type* m_ArrayType = {};
    };

}
