#pragma once

#include "Core.hpp"
#include "String.hpp"

namespace Coral {

	class Type;
	class Object;
	class Attribute;

	class PropertyInfo
	{
	public:
		String GetName() const;
		Type& GetType();

		std::vector<Attribute> GetAttributes() const;

		constexpr inline operator bool()
		{
			return m_Handle != -1;
		}
	private:
		ManagedHandle m_Handle = -1;
		Type* m_Type = nullptr;

		friend class Type;
		friend class Object;
	};
	
}
