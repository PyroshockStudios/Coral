using Microsoft.Build.Evaluation;
using Microsoft.Build.Execution;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

using System.Runtime.InteropServices;
using System.Collections.Generic;

using Coral.Managed.Interop;

namespace Coral.Managed;

public static class MSBuildRunner
{
    [UnmanagedCallersOnly]
    internal static unsafe void Run(
        IntPtr InUserData,
        NativeString InSolutionPath,
        Bool32 InBuildDebug,
        IntPtr InLogMessageCallback,
        IntPtr InBuildStartedCallback,
        IntPtr InBuildFinishedCallback,
        Bool32* OutBuildSuccess,
        NativeInstance<Exception?>* OutBuildException
    )
    {
        var logger = new MSBuildLogger(
             InUserData,
             Marshalling.MarshalDelegate<MSBuildLogMessageHandler>(InLogMessageCallback),
             Marshalling.MarshalDelegate<MSBuildBuildStartedHandler>(InBuildStartedCallback),
             Marshalling.MarshalDelegate<MSBuildBuildFinishedHandler>(InBuildFinishedCallback)
        );

        var projectCollection = new ProjectCollection();

        var buildParamters = new BuildParameters(projectCollection);
        buildParamters.Loggers = new List<ILogger>() { logger };

        var globalProperty = new Dictionary<string, string?>();
        globalProperty.Add("Configuration", InBuildDebug ? "Debug" : "Release");

        BuildManager.DefaultBuildManager.ResetCaches();
        var buildRequest = new BuildRequestData(InSolutionPath.ToString() ?? ".", globalProperty, null, new string[] { "Build" }, null);

        var buildResult = BuildManager.DefaultBuildManager.Build(buildParamters, buildRequest);

        *OutBuildSuccess = buildResult.OverallResult == BuildResultCode.Success;
        *OutBuildException = buildResult.Exception;
    }
}
