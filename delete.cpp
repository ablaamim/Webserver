#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <string>

bool isDirectoryEmpty(const std::string& folderPath);

bool deleteFolderOrFile(const std::string& path) {
    if (access(path.c_str(), F_OK) != 0) {
        std::cerr << "Path '" << path << "' does not exist." << std::endl;
        return false;
    }

    if (access(path.c_str(), W_OK) != 0) {
        std::cerr << "No write permissions for '" << path << "'." << std::endl;
        return false;
    }

    if (isDirectoryEmpty(path)) {
        if (rmdir(path.c_str()) == 0) {
            std::cout << "Folder '" << path << "' deleted successfully." << std::endl;
            return true;
        } else {
            std::cerr << "Error deleting folder: " << path << std::endl;
        }
    } else {
        if (remove(path.c_str()) == 0) {
            std::cout << "File '" << path << "' deleted successfully." << std::endl;
            return true;
        } else {
            std::cerr << "Error deleting file: " << path << std::endl;
        }
    }

    return false;
}

bool isDirectoryEmpty(const std::string& folderPath) {
    DIR* dir = opendir(folderPath.c_str());
    if (dir == NULL) {
        std::cerr << "Error opening directory: " << folderPath << std::endl;
        return false;
    }

    dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..") {
            closedir(dir);
            return false;  // Directory is not empty
        }
    }

    closedir(dir);
    return true;  // Directory is empty
}

// Example usage:
int main() {
    std::string folderPath = "/Users/ablaamim/Desktop/Webserver/lol/";
    std::string filePath = "DEL_NO_PERMISSIONS/file.txt";

    deleteFolderOrFile(folderPath);
    deleteFolderOrFile(filePath);

    return 0;
}
