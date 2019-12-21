#include "template-support.hpp"
#include "sysutils.hpp"

#include <fstream>
#include <ctime>

void createExampleTemplate(const std::filesystem::path& atPath) {
    static constexpr const char* content =
        "$l=\n\
FILENAME: $fn\n\
DESCRIPTION: $de\n\
DATE: $da\n\
AUTHOR: $us\n\
PROJECT: $di\n\
$l=\n";

    std::ofstream file(atPath.string() + "/.ahtemplate.example");
    file << content;
    file.close();
}

void listMacros(std::ostream& os) {
    static constexpr const char* macros = "$lC => Creates a line by repeating the 'C' character, it can be any character.\n\
$fn => The name of the current file.\n\
$de => The description, a phrase in the comment after the $AH$.\n\
$da => The current date.\n\
$us => The name of the user running auto-header.\n\
$di => The name of the base folder auto-header is running.\n";
    os << macros;
}

std::string fillKeyProperty(const std::string& name,
                            const std::string& description,
                            const std::filesystem::path& filepath,
                            const std::filesystem::path& basepath) {
    if (name == "$de")
        return description;
    else if (name == "$fn")
        return filepath.filename().c_str();
    else if (name == "$us")
        return getUsername();
    else if (name == "$di")
        return basepath.filename().c_str();  // maybe wrong for a file in / directory?
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
    else
        return name;
}

std::string fillTemplate(const std::filesystem::path& templatePath,
                         const std::string comment,
                         const std::string description,
                         const std::filesystem::path& filePath,
                         const std::filesystem::path& basepath) {
    const char key = '$';

    std::ifstream templateFile(templatePath);
    if (!templateFile.is_open()) {
        throw std::invalid_argument("Cannot find/open template file with name \".ahtemplate\".");
    }
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
                                           filePath, basepath);

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