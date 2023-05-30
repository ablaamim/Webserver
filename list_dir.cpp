#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <dirent.h>

void list_directories_recursive(const std::string& path, std::vector<std::string>& directoryList) {
    DIR* dir;
    struct dirent* ent;

    if ((dir = opendir(path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                std::string entryName = path + "/" + ent->d_name;
                directoryList.push_back(entryName);

                if (ent->d_type == DT_DIR) {
                    list_directories_recursive(entryName, directoryList);
                }
            }
        }

        closedir(dir);
    } else {
        perror("Could not open directory");
    }
}

std::vector<std::string> listing_directory(const std::string& path) {
    std::vector<std::string> directoryList;
    list_directories_recursive(path, directoryList);
    return directoryList;
}

int main() {
    std::string path = "/Users/ablaamim/Desktop/Webserver/media";
    std::vector<std::string> directories = listing_directory(path);

    for (std::vector<std::string>::const_iterator it = directories.begin(); it != directories.end(); ++it) {
        std::cout << *it << std::endl;
    }

    return 0;
}