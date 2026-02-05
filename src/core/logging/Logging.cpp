#include "Logging.hpp"

namespace Utils
{
	std::optional<std::filesystem::path> GetLogFilePath(const std::string& sLogFileExtension)
	{
		wchar_t exePath[MAX_PATH];
		if (GetModuleFileNameW(nullptr, exePath, MAX_PATH) == 0)
		{
			return std::nullopt;
		}

		std::filesystem::path logPath = std::filesystem::path(exePath).parent_path() / ("src" + sLogFileExtension);
		return logPath;
	}

	void LogHook(const std::string& sHookName, const MH_STATUS eStatus, std::source_location location)
	{
		std::string sMessage;
		if (eStatus == MH_OK)
		{
			sMessage = _Format("[{}:{}] [+] Hook: {} - OK\n", location.file_name(), location.line(), sHookName);
			std::cout << colors::green << sMessage << colors::reset;
		}
		else
		{
			sMessage = _Format("[{}:{}] [-] Hook: {} - Failed with status: {}\n", location.file_name(), location.line(), sHookName, static_cast<int>(eStatus));
			std::cerr << colors::red << sMessage << colors::reset;
		}
	}

	void LogHook(const std::string& sHookName, const std::string& sReason, const std::string& sMessage, std::source_location location)
	{
		std::string sFullMessage = _Format("[{}:{}] [-] Hook: {} - {} - {}\n", location.file_name(), location.line(), sHookName, sReason, sMessage);
		std::cerr << colors::red << sFullMessage << colors::reset;
	}

	void LogError(const int iErrorCode, std::source_location location)
	{
		std::string sMessage = _Format("[{}:{}] [-] Error Code: {:#x}\n", location.file_name(), location.line(), iErrorCode);
		std::cerr << colors::red << sMessage << colors::reset;
	}

	void LogError(const std::string& sErrorMessage, std::source_location location)
	{
		std::string sMessage = _Format("[{}:{}] [-] Error: {}\n", location.file_name(), location.line(), sErrorMessage);
		std::cerr << colors::red << sMessage << colors::reset;
	}

	void LogDebug(const std::string& sDebugMessage, std::source_location location)
	{
		std::string sMessage = _Format("[{}:{}] [*] Debug: {}\n", location.file_name(), location.line(), sDebugMessage);
		std::cout << colors::cyan << sMessage << colors::reset;
	}
}
