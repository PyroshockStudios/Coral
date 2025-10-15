#include "TypeCache.hpp"
#include "CoralManagedFunctions.hpp"
#include "Coral/Type.hpp"

namespace Coral {
    TypeCache& TypeCache::Get()
    {
        static TypeCache s_Instance; 
        return s_Instance;
    }
    void TypeCache::Clear()
    {
        *this = {};
    }

}
