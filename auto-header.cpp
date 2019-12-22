#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <exception>
#include <vector>
#include "parsegar/parsegar.hpp"
#include "template-support.hpp"

struct config {
    bool verbose;
    bool recursive;
    bool keep;
    bool createExample;
    bool listMacros;
    bool help;
    std::filesystem::path path;
};

void processFile(const std::filesystem::path& path);
void processDirectory(const std::filesystem::path& path);
void addHeader(std::fstream& file, const std::filesystem::path& filePath,
               const std::string& comment, const std::string& description);
config parseInput(int argc, const char** argv);
void printHelp(std::ostream& os);

config programConfig;

int main(int argc, const char** argv) {
    try {
        programConfig = parseInput(argc, argv);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (programConfig.help) {
        printHelp(std::cout);
    } else if (programConfig.createExample) {
        createExampleTemplate(programConfig.path);
    } else if (programConfig.listMacros) {
        listMacros(std::cout);
    } else {
        try {
            processDirectory(programConfig.path);
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }
    return 0;
}

void processFile(const std::filesystem::path& path) {
    if (programConfig.verbose)
        std::cout << path.relative_path().c_str() << '\n';

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
    if (programConfig.verbose)
        std::cout << path.relative_path().c_str() << '\n';

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

    static const std::vector<const std::string> templateContent = [&]() {
        std::ifstream templateFile(programConfig.path.append(".ahtemplate"));
        if (!templateFile.is_open()) {
            throw std::invalid_argument(
                "Cannot find/open template file with name \".ahtemplate\".");
        }
        std::stringstream buffer;
        buffer << templateFile.rdbuf();

        std::string line;
        std::vector<const std::string> lines;
        while (std::getline(buffer, line)) {
            lines.emplace_back(line);
        }

        return lines;
    }();

    std::string header;

    try {
        header = fillTemplate(templateContent, comment, description, filePath,
                              programConfig.path);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return;
    }

    file.close();

    std::ofstream newFile;
    if (programConfig.keep) {
        std::string newFilename =
            filePath.stem().string() + 'h' + filePath.extension().string();
        newFile.open(filePath.filename().replace_filename(newFilename));
    } else {
        newFile.open(filePath);
    }

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
    argConf.addFlag("--verbose");
    argConf.addFlag("-r");
    argConf.addFlag("--recursive");
    argConf.addFlag("-k");
    argConf.addFlag("--keep");
    argConf.addFlag("--create-example");
    argConf.addFlag("--list-macros");
    argConf.addFlag("-h");
    argConf.addFlag("--help");

    argConf.addArgument("-p");

    argsParser args(argConf, argc, argv);
    std::string inputPath = args.getArgument("-p");

    std::filesystem::path path;
    if (inputPath.size() <= 0) {
        programConfig.path = std::filesystem::current_path();
    } else {
        programConfig.path = inputPath;
    }

    programConfig.verbose = args.getFlag("-v") || args.getFlag("--verbose");
    programConfig.recursive = args.getFlag("-r") || args.getFlag("--recursive");
    programConfig.keep = args.getFlag("-k") || args.getFlag("--keep");
    programConfig.createExample = args.getFlag("--create-example");
    programConfig.listMacros = args.getFlag("--list-macros");
    programConfig.help = args.getFlag("-h") || args.getFlag("--help");

    if (programConfig.createExample && programConfig.listMacros) {
        throw std::invalid_argument(
            "--create-example and --list-macros can't be put together.");
    } else if ((programConfig.createExample || programConfig.listMacros) &&
               (programConfig.recursive || programConfig.keep)) {
        throw std::invalid_argument(
            "--create-example and --list-macros can't be with the recursive or "
            "the keep flag");
    }

    return programConfig;
}

void printHelp(std::ostream& os) {
    static constexpr const char* helpstr =
        "auto-header [options]\n\
\t-h / --help\t\tThis list\n\
\t--create-example\tCreates an example template\n\
\t--list-macros\t\tList all the available macros to use inside a template\n\
\n\
\t-v / --verbose\t\tList all directories and files as they are being processed\n\
\t-r / --recursive\tWhen a directory is found, process all of it content\n\
\t-k / --keep\t\tDo not modify the original file, just creates a copy\n\
\t-p [base path]\t\tChange the base path for the program to another one\n";

    os << helpstr;
}