#ifndef _TEMPLATE_SUPPORT_H_
#define _TEMPLATE_SUPPORT_H_

#include <string>
#include <filesystem>
#include <vector>

void createExampleTemplate(const std::filesystem::path& atPath);

void listMacros(std::ostream& os);

std::string fillTemplate(const std::vector<const std::string>& templateContent,
                         const std::string comment,
                         const std::string description,
                         const std::filesystem::path& filePath,
                         const std::filesystem::path& basepath);

#endif