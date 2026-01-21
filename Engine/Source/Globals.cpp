#include "Globals.h"
#include "filesystem"

void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4095, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4095, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugStringA(tmp_string2);
	if (app) app->addLog(tmp_string2);
}

std::string getAssetsPath(const std::string& relativePath)
{
	std::filesystem::path deliveryPath = std::filesystem::path() / relativePath;

	std::filesystem::path devPath = std::filesystem::path() / ".." / "Game" / relativePath;

	if (std::filesystem::exists(devPath)) 
	{
		return devPath.generic_string();
	}

	return deliveryPath.generic_string();

}