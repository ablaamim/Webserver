#include "debug.hpp"

void printVector(std::vector<std::string> &vec)
{
    for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
        std::cout << *it << std::endl;
}