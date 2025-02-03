#include <windows.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <map>

std::map<std::string, double> profile_float;
std::map<std::string, long long int> profile_int;
std::map<std::string, std::string> profile_str;

std::regex pattern_float("([\\w]*)=([0-9]+\\.[0-9]+)");
std::regex pattern_int("([\\w]*)=([0-9]+)");
std::regex pattern_hex("([\\w]*)=0[xX]([0-9a-fA-F]+)");
std::regex pattern_bin("([\\w]*)=0[bB]([0-1]+)");
std::regex pattern_word("([\\w]*)=([\\w]*)");
std::regex pattern_squot("([\\w]*)=\'([^\\s]*)\'");
std::regex pattern_dquot("([\\w]*)=\"([^\\s]*)\"");

void profile_ini(std::string path_ini) {
	std::ifstream ini_file(path_ini);

	std::string line;
	while (std::getline(ini_file, line)) {
		bool comment = false;
		for (size_t i = 0; i < line.length(); i++) {
			bool flag = false;
			switch (line[i]) {
				case ' ':
					break;
				case '\t':
					break;
				case '#':
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

		std::smatch match;
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

bool startup(std::string app, std::string cmd, int argc, char* argv[])
{
	// Success Boolean
	BOOL success = 0;

	// Build full command
	for (int i = argc - 1; i > 0; i) {
		std::string shortcut = "%" + std::to_string(i);
		size_t start_pos = cmd.find(shortcut);
		if(start_pos != std::string::npos) {
			cmd.replace(start_pos, shortcut.length(), std::string(argv[i]));
		}
	}

	// Additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// Set the size of the structures
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	// Start the program up
	success = CreateProcess(
		(LPCSTR)app.c_str(),			// The path
		(LPSTR)cmd.c_str(),				// Command line
		NULL,							// Process handle not inheritable
		NULL,							// Thread handle not inheritable
		FALSE,							// Set handle inheritance to FALSE
		0,								// No creation flags
		NULL,							// Use parent's environment block
		NULL,							// Use parent's starting directory
		&si,							// Pointer to STARTUPINFO structure
		&pi								// Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);
	if (!success) {
		return success;
	}

	// Close process and thread handles.
	success = CloseHandle(pi.hProcess);
	if (!success) {
		return success;
	}
	success = CloseHandle(pi.hThread);
	if (!success) {
		return success;
	}
	return success;
}

int main(int argc, char* argv[]) {
	if (argc < 1) {
		std::cerr << "Executable Path not found." << std::endl;
		return -1;
	}

	std::string full_path = argv[0];
	std::string exe_path = full_path.substr(full_path.find_last_of("/\\") + 1);
	std::string ini_path = exe_path;
	ini_path[ini_path.length() - 3] = 'i';
	ini_path[ini_path.length() - 2] = 'n';
	ini_path[ini_path.length() - 1] = 'i';
	profile_ini(ini_path);

	if (argc < 2) {
		std::cerr << "File Path not found." << std::endl;
		return -1;
	}

	startup(profile_str["PATH"], profile_str["CMD"], argc, argv);
	return 0;
}
