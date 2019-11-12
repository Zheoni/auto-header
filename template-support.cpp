#include "template-support.hpp"
#include "sysutils.hpp"

#include <fstream>
#include <ctime>

std::string fillKeyProperty(const std::string& name,
                            const std::string& description,
                            const std::filesystem::path& path) {
    if (name == "$de")
        return description;
    else if (name == "$fn")
        return path.filename().c_str();
    else if (name == "$us")
        return getUsername();
    else if (name == "$di")
        return path.parent_path().filename().c_str(); // maybe wrong for a file in / directory?
    else if (name == "$da") {
        time_t rawtime;
        struct tm* time_info;

        std::time(&rawtime);
        time_info = localtime(&rawtime);

        return std::string(std::to_string(time_info->tm_mon + 1) + '/' +
                              std::to_string(time_info->tm_mday) + '/' +
                              std::to_string(time_info->tm_year + 1900));
    } else if (name[1] == 'l')
        return std::string(20, name[2]);
    else return name;
}

std::string fillTemplate(const std::filesystem::path& templatePath,
                         const std::string comment,
                         const std::string description,
                         const std::filesystem::path& filePath) {
    const char key = '$';

    std::ifstream templateFile(templatePath);
    std::string line, filledTemplate;

    while (getline(templateFile, line)) {
        size_t keyPos = line.find(key);
        size_t lastWritten = 0;

        if (keyPos == std::string::npos) {
            filledTemplate += comment + line + '\n';
        } else {
            filledTemplate += comment + line.substr(0, keyPos);
            lastWritten = keyPos;
            std::string property;
            while (keyPos != std::string::npos) {
                property = fillKeyProperty(line.substr(keyPos, 3), description,
                                           filePath);

                filledTemplate += property;

                lastWritten = keyPos + 3;

                keyPos = line.find(key, keyPos + 3);
            }

            filledTemplate +=
                line.substr(lastWritten, std::string::npos) + '\n';
        }
    }

    return filledTemplate;
}