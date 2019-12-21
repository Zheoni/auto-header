#ifndef _TEMPLATE_SUPPORT_H_
#define _TEMPLATE_SUPPORT_H_

#include <string>
#include <filesystem>

void createExampleTemplate(const std::filesystem::path& atPath);

void listMacros(std::ostream& os);

std::string fillTemplate(const std::filesystem::path& templatePath,
                         const std::string comment,
                         const std::string description,
                         const std::filesystem::path& filePath);

#endif