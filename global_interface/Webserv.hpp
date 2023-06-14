#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "../parsing/ConfigurationParser.hpp"
#include "../Servers/Servers.hpp"
#include "../Request/Request.hpp"

class Webserv
{
    public :

        int                         kq;
        int                         log_fd;
        struct kevent*              event_list;
        std::map<int, std::string>  clients;
        std::map<int, Request>      request;

        Webserv();
        Webserv(configurationSA &config, char **env);
        ~Webserv();


        void                        print_request();
        void                        webserv_evfilt_read(struct kevent *curr_event, std::vector<int> & fds_s, configurationSA &config, Servers &servers, char **env);
        void                        webserv_evfilt_write(struct kevent *curr_event);
        void                        change_events(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
        void                        disconnect_client(int client_fd, std::map<int, std::string>& clients, std::string str);
        void                        run(std::vector<int> & fds_socket, configurationSA &config, Servers &servers, char **env);
        void                        delete_event(int fd, int16_t filter, std::string str);
        void                        event_check(int new_events, std::vector<int> &fds_s, configurationSA &config, Servers &server, char **env);
        void                        entry_point(struct kevent *curr_event, Request request, configurationSA &config, Servers &server, char **env);
        configurationSA::Server     Select_server(std::string ip, std::string port, configurationSA::data_type Servers_vector, std::string hostname);
        configurationSA::location   match_location(std::string trgt, configurationSA::Server server);
        void                        client_cleanup(int client_fd);
        void                        check_before_get_chuncked_messages(configurationSA &config, Servers &server, Request & request, struct kevent *curr_event);
        class Webserv_err : public std::exception
        {
            private :
                std::string error;
            
            public :
                Webserv_err(std::string error) : error(error) {}
                virtual const char *what() const throw() { return error.c_str(); }
                ~Webserv_err() throw() {};
        };
};

#endif