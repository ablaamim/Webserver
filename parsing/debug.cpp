#include "debug.hpp"
#include "../MainInc/main.hpp"

void print_vector(std::vector<std::string> &vec)
{
    for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
        std::cout << *it << std::endl;
}