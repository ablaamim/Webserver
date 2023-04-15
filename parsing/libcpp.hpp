#ifndef LIBCPP_HPP
#define LIBCPP_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

bool isDigit(const std::string &str);

std::vector<std::string> split(std::string str, std::string delim);

std::string to_string(int nbr);

#endif