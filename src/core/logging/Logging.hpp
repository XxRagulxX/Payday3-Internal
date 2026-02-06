#pragma once

#include <string>
#include <source_location>
#include <format>
#include <optional>
#include <filesystem>
#include <MinHook.h>
#include <iostream>
#include <fstream>
#include <ios>
#include <system_error>
#include <chrono>
#include <Windows.h>

// Helper functions for logging
namespace Utils
{
	std::optional<std::filesystem::path> GetLogFilePath(const std::string& sLogFileExtension);

	void LogHook(const std::string& sHookName, const MH_STATUS eStatus, std::source_location location = std::source_location::current());

	void LogHook(const std::string& sHookName, const std::string& sReason, const std::string& sMessage, std::source_location location);

	void LogError(const int iErrorCode, std::source_location location = std::source_location::current());

	void LogError(const std::string& sErrorMessage, std::source_location location = std::source_location::current());

	void LogDebug(const std::string& sDebugMessage, std::source_location location = std::source_location::current());

	// Because std::format verifies values at compile time sometimes we need to use its runtime version std::vformat this is my wrapper for it
	template<typename... Args>
	inline std::string _Format(const std::format_string<Args...> fmt, Args&&... args)
	{
		return std::vformat(fmt.get(), std::make_format_args(args...));
	}
}