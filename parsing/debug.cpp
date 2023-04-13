#include "debug.hpp"
#include "../MainInc/main.hpp"

void printVector(std::vector<std::string> &vec)
{
    for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
        std::cout << *it << std::endl;
}

void configurationSA::conf::print_data()
{
  for (std::map<std::string, rawConf>::const_iterator it = _data.begin(); it != _data.end(); ++it)
  {
    std::cout << it->first << ": {" << std::endl;
    std::cout << "    keyType: " << it->second.keyType << std::endl;
    std::cout << "    max_Parameters: " << it->second.max_Parameters << std::endl;
    std::cout << "    validParametters: {" << std::endl;
    for (std::set<std::string>::const_iterator it2 = it->second.validParametters.begin(); it2 != it->second.validParametters.end(); ++it2)
        std::cout << "        " << *it2 << std::endl;
    std::cout << "    }" << std::endl;
    std::cout << "}" << std::endl << std::endl;
  }
  std::cout << std::endl << std::endl;
}