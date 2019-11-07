#ifndef _SYSUTILS_H_
#define _SYSUTILS_H_

#include <iostream>
#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__MACH__)
#define _UNIX_PLATFORM_
#include <unistd.h>
#include <pwd.h>
#endif

#if defined(_WIN32)
#define _WIN_PLATFORM_
#include <Windows.h>
#endif

std::string getUsername();

#endif