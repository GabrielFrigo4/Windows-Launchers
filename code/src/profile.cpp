#define UNICODE
#define _UNICODE

#include "profile.hpp"
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

void profile_ini(std::wstring path_ini)
{
	std::wifstream ini_file(path_ini);

	std::wstring line;
	while (std::getline(ini_file, line))
	{
		bool comment = false;
		for (size_t i = 0; i < line.length(); i++)
		{
			bool flag = false;
			switch (line[i])
			{
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
			if (flag)
			{
				break;
			}
		}
		if (comment)
		{
			continue;
		}

		std::wsmatch match;
		if (std::regex_search(line, match, pattern_float))
		{
			profile_float[match[1]] = std::stod(match[2]);
		}
		if (std::regex_search(line, match, pattern_int))
		{
			profile_int[match[1]] = std::stoll(match[2], nullptr, 10);
		}
		if (std::regex_search(line, match, pattern_hex))
		{
			profile_int[match[1]] = std::stoll(match[2], nullptr, 16);
		}
		if (std::regex_search(line, match, pattern_bin))
		{
			profile_int[match[1]] = std::stoll(match[2], nullptr, 2);
		}
		if (std::regex_search(line, match, pattern_word))
		{
			profile_str[match[1]] = match[2];
		}
		if (std::regex_search(line, match, pattern_squot))
		{
			profile_str[match[1]] = match[2];
		}
		if (std::regex_search(line, match, pattern_dquot))
		{
			profile_str[match[1]] = match[2];
		}
	}
}

double profile_get_float(std::wstring key)
{
	return profile_float[key];
}

long long int profile_get_int(std::wstring key)
{
	return profile_int[key];
}

std::wstring profile_get_str(std::wstring key)
{
	return profile_str[key];
}
