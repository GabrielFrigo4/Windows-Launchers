#define UNICODE
#define _UNICODE

#include <windows.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <map>

std::map<std::wstring, double> profile_float;
std::map<std::wstring, long long int> profile_int;
std::map<std::wstring, std::wstring> profile_str;

std::wregex pattern_float(L"([\\w]*)=([0-9]+\\.[0-9]+)");
std::wregex pattern_int(L"([\\w]*)=([0-9]+)");
std::wregex pattern_hex(L"([\\w]*)=0[xX]([0-9a-fA-F]+)");
std::wregex pattern_bin(L"([\\w]*)=0[bB]([0-1]+)");
std::wregex pattern_word(L"([\\w]*)=([\\w]*)");
std::wregex pattern_squot(L"([\\w]*)=\'([^\\s]*)\'");
std::wregex pattern_dquot(L"([\\w]*)=\"([^\\s]*)\"");

void profile_ini(std::wstring path_ini) {
	std::wifstream ini_file(path_ini);

	std::wstring line;
	while (std::getline(ini_file, line)) {
		bool comment = false;
		for (size_t i = 0; i < line.length(); i++) {
			bool flag = false;
			switch (line[i]) {
				case L' ':
					break;
				case L'\t':
					break;
				case L'#':
					comment = true;
					flag = true;
					break;
				default:
					comment = false;
					flag = true;
					break;
			}
			if (flag) {
				break;
			}
		}
		if (comment) {
			continue;
		}

		std::wsmatch match;
		if (std::regex_search(line, match, pattern_float)) {
			profile_float[match[1]] = std::stod(match[2]);
		}
		if (std::regex_search(line, match, pattern_int)) {
			profile_int[match[1]] = std::stoll(match[2], nullptr, 10);
		}
		if (std::regex_search(line, match, pattern_hex)) {
			profile_int[match[1]] = std::stoll(match[2], nullptr, 16);
		}
		if (std::regex_search(line, match, pattern_bin)) {
			profile_int[match[1]] = std::stoll(match[2], nullptr, 2);
		}
		if (std::regex_search(line, match, pattern_word)) {
			profile_str[match[1]] = match[2];
		}
		if (std::regex_search(line, match, pattern_squot)) {
			profile_str[match[1]] = match[2];
		}
		if (std::regex_search(line, match, pattern_dquot)) {
			profile_str[match[1]] = match[2];
		}
	}
}

void startup(std::wstring app, std::wstring cmd, int argc, wchar_t* argv[])
{
	// Build full command
	for (int i = argc - 1; i > 0; i--) {
		std::wstring shortcut = L"%" + std::to_wstring(i);
		size_t start_pos = cmd.find(shortcut);
		if(start_pos != std::wstring::npos) {
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

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	int argc;
	wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (argc < 1) {
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

	if (argc < 2) {
		MessageBox(nullptr, L"File Path not found.", L"Error", MB_ICONERROR);
		return -1;
	}

	startup(profile_str[L"PATH"], profile_str[L"CMD"], argc, argv);
	return 0;
}
