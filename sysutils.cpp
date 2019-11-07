#include "sysutils.hpp"

std::string getUsername() {
    #if defined(_UNIX_PLATFORM_)

    passwd *userData = getpwuid(getuid());

    return std::string(userData->pw_name);

    #elif defined(_WIN_PLATFORM_)
    char user_name[257];
    DWORD user_name_size = sizeof(user_name);
    if (GetUserName(user_name, &user_name_size))
        return std::string(user_name);
    else
        throw std::runtime_error("Cannot fetch username from windows");

    #else
    #error Cannot determine plattform to get username
    #endif
}