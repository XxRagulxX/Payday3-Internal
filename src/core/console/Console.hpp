#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <format>
#include <io.h>
#include <fcntl.h>

class Console
{
private:
	FILE* m_pSTDOutDummy = nullptr;
	FILE* m_pSTDErrDummy = nullptr;
	FILE* m_pSTDInDummy = nullptr;

	bool m_bInitalized : 1 = false;
	bool m_bVisible    : 1 = false;

public:
	Console(bool bVisibility, std::string sConsoleTitle = "DEBUG CONSOLE", DWORD dwMode = ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING) noexcept;

	~Console() noexcept;

	void Destroy();

	void SetTitle(std::string sTitle);

	bool GetVisibility() const;

	void SetVisibility(bool bVisibility);

	void ToggleVisibility();

	void SetMode(DWORD dwMode);
};
