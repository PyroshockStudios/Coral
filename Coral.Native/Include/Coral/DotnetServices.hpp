#pragma once

#include "Core.hpp"

namespace Coral {
    class Object;
    enum class MSBuildLogSeverity : int32_t
    {
        Message = 0,
        Warning = 1,
        Error = 2
    };
    struct MSBuildLogMessageHandlerParams
    {
        MSBuildLogSeverity Severity;
        StdStringView Message;
        StdStringView File;
        StdStringView Code;
        StdStringView Subcategory;
        int32_t LineNumber;
        int32_t EndLineNumber;
        int32_t ColumnNumber;
        int32_t EndColumnNumber;
    };
    struct MSBuildBuildStartedHandlerParams
    {
        int _unused;
    };
    struct MSBuildBuildFinishedHandlerParams
    {
        bool BuildSuccess;
    };
    struct MSBuildRunnerParameters
    {
        StdString InSolutionPath = ".";
        bool InBuildDebug = true;
        StdFunction<void(const MSBuildLogMessageHandlerParams&)> InLogMessageCallback = nullptr;
        StdFunction<void(const MSBuildBuildStartedHandlerParams&)> InBuildStartedCallback = nullptr;
        StdFunction<void(const MSBuildBuildFinishedHandlerParams&)> InBuildFinishedCallback = nullptr;
    };

    class DotnetServices
    {
    public:
        static bool RunMSBuild(const MSBuildRunnerParameters& InParameters, Object* OutException = nullptr);
    };
}
