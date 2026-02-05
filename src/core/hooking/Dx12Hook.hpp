#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>

namespace Dx12Hook
{
	// Public API functions
	bool Initialize();
	void Shutdown();
	bool IsInitialized();
	void SetMenuVisible(bool visible);
	bool IsMenuVisible();
}
