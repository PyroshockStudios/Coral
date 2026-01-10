#pragma once

#include "Core.hpp"
#include "MessageLevel.hpp"
#include "Assembly.hpp"
#include "Object.hpp"

#include <functional>
#include <filesystem>

namespace Coral {

    using ExceptionCallbackFn = StdFunction<void(StdStringView)>;

    enum class DotNetTarget : uint32_t
    {
        NONE = 0,
        CORE_2_0 = 0x20,
        CORE_2_1 = 0x21,
        CORE_2_2 = 0x22,
        CORE_3_0 = 0x30,
        CORE_3_1 = 0x31,
        NET_5_0 = 0x50,
        NET_6_0 = 0x60,
        NET_7_0 = 0x70,
        NET_8_0 = 0x80,
        NET_9_0 = 0x90,
        NET_10_0 = 0x100,

        MAX = NET_10_0
    };
    const char* GetDotNetTargetString(DotNetTarget target);

    struct HostSettings
    {
        /// <summary>
        /// The file path to Coral.runtimeconfig.json (e.g C:\Dev\MyProject\ThirdParty\Coral)
        /// </summary>
        StdString CoralDirectory;

        MessageCallbackFn MessageCallback = nullptr;
        MessageLevel MessageFilter = MessageLevel::All;

        ExceptionCallbackFn ExceptionCallback = nullptr;

        /// <summary>
        /// List of major versions that are supported
        /// Descending order of priority (first is highest)
        /// </summary>
        StdVector<DotNetTarget> SupportedTargets = { DotNetTarget::NET_10_0, DotNetTarget::NET_9_0, DotNetTarget::NET_8_0, DotNetTarget::NET_7_0 };
    };

    enum class CoralInitStatus : int32_t
    {
        Success = 0,
        CoralManagedNotFound = -1,
        CoralManagedInitError = -2,
        DotNetNotFound = -3,
    };

    class HostInstance
    {
    public:
        CoralInitStatus Initialize(HostSettings InSettings);
        void Shutdown();

        AssemblyLoadContext CreateAssemblyLoadContext(StdStringView InName);
        void UnloadAssemblyLoadContext(AssemblyLoadContext& InLoadContext);

        // `InDllPath` is a colon-separated list of paths from which AssemblyLoader will try and resolve load paths at runtime.
        // This does not affect the behaviour of LoadAssembly from native code.
        AssemblyLoadContext CreateAssemblyLoadContext(StdStringView InName, StdStringView InDllPath);

        DotNetTarget GetActiveDotNetTarget() const;

    private:
        bool LoadHostFXR();
        bool InitializeCoralManaged();
        void LoadCoralFunctions();

        void* LoadCoralManagedFunctionPtr(UCStringView InAssemblyPath, const UCChar* InTypeName, const UCChar* InMethodName, const UCChar* InDelegateType = CORAL_UNMANAGED_CALLERS_ONLY) const;

        template <typename TFunc>
        TFunc LoadCoralManagedFunctionPtr(const UCChar* InTypeName, const UCChar* InMethodName, const UCChar* InDelegateType = CORAL_UNMANAGED_CALLERS_ONLY) const
        {
            return (TFunc)LoadCoralManagedFunctionPtr(m_CoralManagedAssemblyPath, InTypeName, InMethodName, InDelegateType);
        }

    private:
        HostSettings m_Settings;
        UCString m_CoralManagedAssemblyPath;
        StdVector<DotNetTarget> m_SupportedTargets;
        DotNetTarget m_ActiveTarget = DotNetTarget::NONE;
        void* m_HostFXRContext = nullptr;
        bool m_Initialized = false;

        friend class AssemblyLoadContext;
    };

}
