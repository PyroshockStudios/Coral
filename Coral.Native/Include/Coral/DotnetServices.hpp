#pragma once

#include <string>

namespace Coral {
    class DotnetServices
    {
    public:
        static bool RunMSBuild(const StdString& InSolutionPath, bool InBuildDebug = true);
    };
}
