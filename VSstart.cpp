
#include <fstream>
#include <cctype>
#include <string>
#include <regex>
#include <vector>
#include <filesystem>
#include <windows.h>
#include <nlohmann/json.hpp> 

using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;

string getExecutablePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    string::size_type pos = string(buffer).find_last_of("\\/");
    return string(buffer).substr(0, pos);
}

void replaceSubstringInFile(const string& filePath, const string& target, const string& replacement) {
    ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        return;
    }
    string content((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();
    size_t pos = content.find(target);
    while (pos != string::npos) {
        content.replace(pos, target.length(), replacement);
        pos = content.find(target, pos + replacement.length());
    }
    ofstream outputFile(filePath);
    if (!outputFile.is_open()) {
        return;
    }
    outputFile << content;
    outputFile.close();
}

void processPaths(vector<string>& paths) {
    vector<string> updatedPaths;
    for (auto it = paths.begin(); it != paths.end();) {
        size_t uuidPos = it->find(":uuid:");
        if (uuidPos != string::npos) {
            string basePath = it->substr(0, uuidPos);
            bool searchFiles = true;
            if (uuidPos + 6 < it->length() && ((*it)[uuidPos + 6] == '/' || (*it)[uuidPos + 6] == '\\')) {
                searchFiles = false;
            }
            try {
                for (const auto& entry : fs::directory_iterator(basePath)) {
                    if ((searchFiles && entry.is_regular_file()) || (!searchFiles && entry.is_directory())) {
                        string newPath = basePath + entry.path().filename().string() + it->substr(uuidPos + 6);
                        updatedPaths.push_back(newPath);
                    }
                }
            } catch (const exception& e) {
                return;
            }
            it = paths.erase(it);
        } else {
            ++it;
        }
    }
    paths.insert(paths.end(), updatedPaths.begin(), updatedPaths.end());
}

void processStorageJson(const string& filePath, const string& targetDriveLetter, const string& replacementDriveLetter) {
    ifstream inputFile(filePath);
    if (!inputFile.is_open()) {return;}
    json jsonData;
    inputFile >> jsonData;
    inputFile.close();
    if (jsonData.contains("windowsState")) {
        if (jsonData["windowsState"].contains("lastActiveWindow")){
            if (jsonData["windowsState"]["lastActiveWindow"].contains("workspaceIdentifier")){
                for (const auto& [key, value] : jsonData["windowsState"]["lastActiveWindow"]["workspaceIdentifier"].items()){
                    string val = value.get<string>();
                    if (val.find("workspace.json") != string::npos) {
                        string workspacePath = val;
                        size_t pos = workspacePath.find("file:///");
                        if (pos !=string:: npos) {
                            workspacePath.erase(pos, 8);
                        }
                        pos = workspacePath.find("%3A");
                        if (pos !=string:: npos) {
                            workspacePath.replace(pos, 3, ":");
                        }
                        replaceSubstringInFile(workspacePath, targetDriveLetter, replacementDriveLetter);
                    }
                }
            }
        }
    }
    return;
}

bool processConfpathFile(
    const string& confpathFilePath,
    string& targetSubstring,
    vector<string>& filePaths,
    string& currentDriveLetter) {
    ifstream confpathFile(confpathFilePath);
    if (!confpathFile.is_open()) {
        ofstream confpathFile(confpathFilePath);
    }
    stringstream buffer;
    buffer << confpathFile.rdbuf();
    string fileContent = buffer.str();
    if (fileContent.empty()) {
        fileContent = currentDriveLetter + "\n" + \
"--Below you can add relative paths to files using a changing drive letter------Don't Delete this string---CommentString-\n\
\\data\\user-data\\User\\profiles\\settings.json\n\
\\data\\user-data\\User\\profiles\\:uuid:\\extensions.json\n\
\\data\\user-data\\User\\globalStorage\\storage.json\n\
\\data\\user-data\\User\\profiles\\:uuid:\\settings.json\n";
    }
    confpathFile.close();
    istringstream fileStream(fileContent);
    if (!getline(fileStream, targetSubstring)) {
        return false;
    }
    targetSubstring.erase(targetSubstring.find_last_not_of(" \n\r\t") + 1);
    
    string filePath;
    if (!getline(fileStream, filePath)) {;} // skip comment string
    while (getline(fileStream, filePath)) {
        if (!filePath.empty()) {
            string fullPath = getExecutablePath() + filePath;
            filePaths.push_back(fullPath);
        }
    }
    processPaths(filePaths);
    
    ofstream outputFile(confpathFilePath);
    if (!outputFile.is_open()) {
        return false;
    }
    outputFile << currentDriveLetter[0] << '\n' << fileContent.substr(fileContent.find('\n') + 1);
    outputFile.close();
    return true;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    string currentDriveLetter = getExecutablePath().substr(0, 1);
    currentDriveLetter[0] = toupper(currentDriveLetter[0]);
    string targetSubstring;
    string ConfpathFilePath = "RunPortable.ini";
    vector<string> filePaths;
    if (!processConfpathFile(ConfpathFilePath, targetSubstring, filePaths, currentDriveLetter)) {
        return 1;
    }
    string targetDriveLetter = targetSubstring + ":";
    string targetDriveLetterEncoded = string(1, tolower(targetSubstring[0])) + "%3A";
    string replacementDriveLetter = currentDriveLetter + ":";
    string replacementDriveLetterEncoded = string(1, tolower(currentDriveLetter[0])) + "%3A";
    if (currentDriveLetter!=targetDriveLetter) {
        for (const string& filePath : filePaths) {
            replaceSubstringInFile(filePath, targetDriveLetter, replacementDriveLetter);
            replaceSubstringInFile(filePath, targetDriveLetterEncoded, replacementDriveLetterEncoded);
            if (filePath.find("storage.json") != string::npos) {
                processStorageJson(filePath, targetDriveLetter, replacementDriveLetter);
            }
        }
    }
    LPCWSTR args = GetCommandLineW();
    if (args[0] == L'"'){
        args = wcschr(args + 1, L'"');
        if (args) {args++; }
    } else {args = wcschr(args, L' '); }
    while (args && *args == L' '){ args++; }
    string pathCodeExe = getExecutablePath() + "\\Code.exe";
    wstring wideFullPath(pathCodeExe.begin(), pathCodeExe.end());
    ShellExecuteW(nullptr, L"open", wideFullPath.c_str(), args, nullptr, SW_HIDE);
    return 0;
}
