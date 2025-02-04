#define UNICODE
#define _UNICODE

#include <string>

void profile_ini(std::wstring path_ini);
double profile_get_float(std::wstring key);
long long int profile_get_int(std::wstring key);
std::wstring profile_get_str(std::wstring key);
