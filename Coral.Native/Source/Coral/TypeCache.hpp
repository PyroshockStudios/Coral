#pragma once

#include "Coral/Core.hpp"
#include "Coral/StableVector.hpp"

namespace Coral {
    class Type;

    class TypeCache
    {
    public:
        static TypeCache& Get();

        Type* CacheType(Type&& InType);

        Type* GetTypeByName(StdStringView InName) const;

        Type* GetTypeByID(TypeId InTypeID) const;

        static Type* GetType(StdStringView InName)
        {
            return Get().GetTypeByName(InName);
        }
        static Type* GetType(TypeId InTypeID)
        {
            return Get().GetTypeByID(InTypeID);
        }

        void Clear();
    private:
        friend class AssemblyLoadContext;

        StableVector<Type> m_Types;
        StdUnorderedMap<StdString, Type*> m_NameCache;
        StdUnorderedMap<TypeId, Type*> m_IDCache;

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

        friend class Type;
    };

}
