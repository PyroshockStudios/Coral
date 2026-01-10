using Microsoft.Build.Evaluation;
using Microsoft.Build.Execution;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

using System.Runtime.InteropServices;
using System.Collections.Generic;

using Coral.Managed.Interop;

namespace Coral.Managed;

public enum MSBuildLogSeverity : int
{
    Message,
    Warning,
    Error
}

public delegate void MSBuildLogMessageHandler(IntPtr InUserData, 
    MSBuildLogSeverity InSeverity, string InMessage,
    string InFile, string InCode, string InSubcategory,
    int InLineNumber, int InEndLineNumber, int InColumnNumber, int InEndColumnNumber);
public delegate void MSBuildBuildStartedHandler(IntPtr InUserData);
public delegate void MSBuildBuildFinishedHandler(IntPtr InUserData, Bool32 InBuildSuccess);
internal class MSBuildLogger : ILogger
{
    private IntPtr userdata;
    private MSBuildLogMessageHandler? onLogMessage = null;
    private MSBuildBuildStartedHandler? onBuildStarted = null;
    private MSBuildBuildFinishedHandler? onBuildFinished = null;

    internal MSBuildLogger(IntPtr userdata, MSBuildLogMessageHandler? onLogMessage = null, MSBuildBuildStartedHandler? onBuildStarted = null,
        MSBuildBuildFinishedHandler? onBuildFinished = null)
    {
        this.userdata = userdata;
        this.onLogMessage = onLogMessage;
        this.onBuildStarted = onBuildStarted;
        this.onBuildFinished = onBuildFinished;
    }

    public LoggerVerbosity Verbosity { get => LoggerVerbosity.Normal; set { } }
    public string? Parameters { get; set; }

    public void Initialize(IEventSource eventSource)
    {
        eventSource.MessageRaised +=
            (sender, args) =>
            {
                onLogMessage?.Invoke(userdata, MSBuildLogSeverity.Message, args.Message ?? "", args.File, args.Code, args.Subcategory, args.LineNumber,
                args.EndLineNumber, args.ColumnNumber, args.EndColumnNumber);
            };
        eventSource.ErrorRaised +=
            (sender, args) =>
            {
                onLogMessage?.Invoke(userdata, MSBuildLogSeverity.Error, args.Message ?? "", args.File, args.Code, args.Subcategory, args.LineNumber,
                args.EndLineNumber, args.ColumnNumber, args.EndColumnNumber);
            };
        eventSource.WarningRaised +=
            (sender, args) =>
            {
                onLogMessage?.Invoke(userdata, MSBuildLogSeverity.Warning, args.Message ?? "", args.File, args.Code, args.Subcategory, args.LineNumber,
                args.EndLineNumber, args.ColumnNumber, args.EndColumnNumber);
            };
        eventSource.BuildStarted +=
            (sender, args) =>
            {
                onBuildStarted?.Invoke(userdata);
            };
        eventSource.BuildFinished +=
            (sender, args) =>
            {
                onBuildFinished?.Invoke(userdata, args.Succeeded);
            };
        eventSource.ProjectStarted +=
            (sender, args) => { };
        eventSource.ProjectFinished +=
            (sender, args) => { };
        eventSource.TargetStarted +=
            (sender, args) => { };
        eventSource.TargetFinished +=
            (sender, args) => { };
        eventSource.TaskStarted +=
            (sender, args) => { };
        eventSource.TaskFinished +=
            (sender, args) => { };
        eventSource.CustomEventRaised +=
            (sender, args) => { };
        eventSource.StatusEventRaised +=
            (sender, args) => { };
        eventSource.AnyEventRaised +=
            (sender, args) => { };
    }

    public void Shutdown()
    {
    }
}

