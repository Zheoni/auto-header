#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "parsegar/parsegar.hpp"
#include "template-support.hpp"

struct config {
    bool verbose;
    bool recursive;
    std::filesystem::path path;
};

void processFile(const std::filesystem::path& path);
void processDirectory(const std::filesystem::path& path);
void addHeader(std::fstream& file, const std::filesystem::path& filePath,
               const std::string& comment, const std::string& description);
config parseInput(int argc, const char** argv);

config programConfig;

int main(int argc, const char** argv) {
    
    programConfig = parseInput(argc, argv);

    processDirectory(programConfig.path);
}

void processFile(const std::filesystem::path& path) {
    if (programConfig.verbose) std::cout << path.relative_path().c_str() << '\n';

    std::fstream file(path.c_str());

    if (file.is_open()) {
        std::string firstLine;
        getline(file, firstLine);

        size_t keyPos = firstLine.find("$AH$");

        if (keyPos != std::string::npos) {
            std::string comment = firstLine.substr(0, keyPos);
            std::string description =
                firstLine.substr(keyPos + 4, firstLine.size());

            addHeader(file, path, comment, description);
        }

        file.close();
    }
}

void processDirectory(const std::filesystem::path& path) {
    if (programConfig.verbose) std::cout << path.relative_path().c_str() << '\n';

    namespace fs = std::filesystem;
    for (const fs::directory_entry& entry : fs::directory_iterator(path)) {
        if (entry.path().filename().c_str()[0] == '.') {
            continue;
        } else if (entry.is_directory()) {
            if (programConfig.recursive) processDirectory(entry.path());
        } else {
            processFile(entry.path());
        }
    }
}

void addHeader(std::fstream& file, const std::filesystem::path& filePath,
               const std::string& comment, const std::string& description) {
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string header = fillTemplate(".ahtemplate", comment, description, filePath);

    file.close();

    std::ofstream newFile(filePath); // Maybe do it in a copy and then restore if something go wrong?

    if (newFile.is_open()) {
        newFile << header;
        newFile << buffer.rdbuf();
    }

    newFile.close();
}

config parseInput(int argc, const char** argv) {
    config programConfig;

    argsConfig argConf;
    argConf.addFlag("-v");
    argConf.addFlag("-r");
    argConf.addArgument("-p");

    argsParser args(argConf, argc, argv);
    std::string inputPath = args.getArgument("-p");

    std::filesystem::path path;
    if (inputPath.size() <= 0) {
        programConfig.path = std::filesystem::current_path();
    } else {
        programConfig.path = inputPath;
    }

    programConfig.verbose = args.getFlag("-v");
    programConfig.recursive = args.getFlag("-r");

    return programConfig;
}