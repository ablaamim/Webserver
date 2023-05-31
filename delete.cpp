#include <iostream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>

void deleteFiles(const std::string& directory) {
    DIR* dir = opendir(directory.c_str());
    if (dir == NULL) {
        std::cout << "Error opening directory: " << directory << std::endl;
        return;
    }

    dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string file_name = entry->d_name;
        std::string full_path = directory + "/" + file_name;

        struct stat file_stat;
        if (stat(full_path.c_str(), &file_stat) == -1) {
            std::cout << "Error getting file status for: " << full_path << std::endl;
            continue;
        }

        if (S_ISREG(file_stat.st_mode)) {  // Check if it's a regular file
            if (remove(full_path.c_str()) != 0) {
                std::cout << "Error deleting file: " << full_path << std::endl;
            } else {
                std::cout << "Deleted file: " << full_path << std::endl;
            }
        }
    }

    closedir(dir);
}

int main() {
    std::string directory_path = "/Users/ablaamim/Desktop/DEL_TEST";  // Replace with your directory path
    deleteFiles(directory_path);
    return 0;
}