#include "Servers.hpp"
#include "../parsing/ConfigurationParser.hpp"

Servers::Servers(configurationSA &config)
{
    std::cout << "Servers constructor called" << std::endl;

    configurationSA::data_type conf= config.getData();

    std::cout << "conf.size() = " << conf.size() << std::endl;

    if (conf.empty())
    {
        std::cout << "conf is empty" << std::endl;
        exit (EXIT_FAILURE);
    }

    else
    {
        std::cout << "-> conf is not empty <-" << std::endl;
    }
    
    // Loop through the configuration file and create a socket for each server
    for (configurationSA::data_type::iterator iterConf = conf.begin(); iterConf != conf.end(); iterConf++)
    {
        std::cout << "FIRST LOOP " << std::endl;
        for (configurationSA::Server::typeListen::iterator iterListen = iterConf->Listen.begin(); iterListen != iterConf->Listen.end(); iterListen++)
        {
            std::cout << "SECOND LOOP " << std::endl;
            for (std::set<std::string>::iterator iterSet = iterListen->second.begin(); iterSet != iterListen->second.end(); iterSet++)
            {
                std::cout << "THIRD LOOP " << std::endl;
            }
        }
        std::cout << " Server  : " << iterConf - conf.begin() << std::endl;      
    }
}

Servers::~Servers()
{
    std::cout << "Servers destructor called" << std::endl;
}