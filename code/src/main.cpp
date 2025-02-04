#define UNICODE
#define _UNICODE

#include "profile.hpp"
#include <windows.h>
#include <string>

void startup(std::wstring app, std::wstring cmd, int argc, wchar_t *argv[])
{
	// Build full command
	for (int i = argc - 1; i > 0; i--)
	{
		std::wstring shortcut = L"%" + std::to_wstring(i);
		size_t start_pos = cmd.find(shortcut);
		if (start_pos != std::wstring::npos)
		{
			cmd.replace(start_pos, shortcut.length(), argv[i]);
		}
	}
	cmd = L"\"" + app + L"\" " + cmd;

	// Additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// Set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the program up
	if (CreateProcess(nullptr, (LPWSTR)cmd.c_str(), nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	int argc;
	wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (argc < 1)
	{
		MessageBox(nullptr, L"Executable Path not found.", L"Error", MB_ICONERROR);
		return -1;
	}

	std::wstring full_path = argv[0];
	std::wstring exe_path = full_path.substr(full_path.find_last_of(L"/\\") + 1);
	std::wstring ini_path = exe_path;
	ini_path[ini_path.length() - 3] = L'i';
	ini_path[ini_path.length() - 2] = L'n';
	ini_path[ini_path.length() - 1] = L'i';
	profile_ini(ini_path);

	if (argc < 2)
	{
		MessageBox(nullptr, L"File Path not found.", L"Error", MB_ICONERROR);
		return -1;
	}

	startup(profile_get_str(L"PATH"), profile_get_str(L"CMD"), argc, argv);
	return 0;
}
