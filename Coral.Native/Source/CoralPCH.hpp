#pragma once

#include <iostream>
#include <cstring>

#ifdef _WIN32
	#define NOMINMAX
	#include <Windows.h>
#endif

#include <filesystem>

// HostFXR / CoreCLR
//#include <nethost.h>
#include <coreclr_delegates.h>
#include <hostfxr.h>

#include "Coral/Config.hpp"
