#include "parsing.hpp"

void    printVector(std::vector<std::string> lines)
{
    std::cout << "----> Printing vector..." << std::endl;
    for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it)
    {
        std::cout << *it << std::endl;
    }
    std::cout << "----> Printing done." << std::endl;
}

ParseConfiguration::ParseConfiguration()
{
    std::cout << "----> Constructor called." << std::endl;
}

ParseConfiguration::~ParseConfiguration()
{
    std::cout << "----> Destructor called." << std::endl;
}

std::vector<std::string> ParseConfiguration::getlines() const
{
    return (lines);
}

void ParseConfiguration::readFile(const std::string &file)
{
    std::ifstream            ifs(file.c_str());
    std::string              line;
    std::vector<std::string> lines;

    if (!ifs.is_open())
        throw std::runtime_error("Error: File not found.");

    while (std::getline(ifs, line))
    {
        this->lines.push_back(line);
    }
    //printVector(getlines());
}