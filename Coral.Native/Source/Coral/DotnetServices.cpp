#include "Coral/DotnetServices.hpp"
#include "Coral/Object.hpp"

#include "CoralManagedFunctions.hpp"

namespace Coral {

    bool DotnetServices::RunMSBuild(const MSBuildRunnerParameters& InParameters, Object* OutException)
    {
        NativeString s = NativeString::New(InParameters.InSolutionPath);
        Bool32 result;
        const void* userdata = reinterpret_cast<const void*>(&InParameters);

        static auto logMessageCallback =
            [](const void* InUserData,
                MSBuildLogSeverity InSeverity, const char* InMessage,
                const char* InFile, const char* InCode, const char* InSubcategory,
                int32_t InLineNumber, int32_t InEndLineNumber, int32_t InColumnNumber, int32_t InEndColumnNumber)
        {
            const MSBuildRunnerParameters* pParams = reinterpret_cast<const MSBuildRunnerParameters*>(InUserData);
            pParams->InLogMessageCallback({
                .Severity = InSeverity,
                .Message = InMessage,
                .File = InFile,
                .Code = InCode,
                .Subcategory = InSubcategory,
                .LineNumber = InLineNumber,
                .EndLineNumber = InEndLineNumber,
                .ColumnNumber = InColumnNumber,
                .EndColumnNumber = InEndColumnNumber,
            });
        };
        static auto buildStartedCallback = [](const void* InUserData)
        {
            const MSBuildRunnerParameters* pParams = reinterpret_cast<const MSBuildRunnerParameters*>(InUserData);
            pParams->InBuildStartedCallback({

            });
        };
        static auto buildFinishedCallback = [](const void* InUserData, Bool32 InBuildSuccess)
        {
            const MSBuildRunnerParameters* pParams = reinterpret_cast<const MSBuildRunnerParameters*>(InUserData);
            pParams->InBuildFinishedCallback({
                .BuildSuccess = !!InBuildSuccess,
            });
        };

        s_ManagedFunctions.RunMSBuildFptr(userdata, s, InParameters.InBuildDebug,
            InParameters.InLogMessageCallback ? logMessageCallback : nullptr,
            InParameters.InBuildStartedCallback ? buildStartedCallback : nullptr,
            InParameters.InBuildFinishedCallback ? buildFinishedCallback : nullptr,
            &result, OutException);

        NativeString::Free(s);

        return !!result;
    }
}
