#include "Coral/DotnetServices.hpp"

#include "CoralManagedFunctions.hpp"

namespace Coral {
    bool DotnetServices::RunMSBuild(const StdString& InSolutionPath, bool InBuildDebug)
    {
        NativeString s = NativeString::New(InSolutionPath);
        Bool32 result;

        s_ManagedFunctions.RunMSBuildFptr(s, InBuildDebug, &result);

        NativeString::Free(s);

        return !!result;
    }
}
