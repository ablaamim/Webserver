
#include "../MainInc/main.hpp"

<<<<<<< HEAD
void Webserv::change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);
}

void Webserv::disconnect_client(int client_fd, std::map<int, std::string>& clients)
{
    std::cout << "client disconnected: " << client_fd << std::endl;
    close(client_fd);
    clients.erase(client_fd);
}

void Webserv::delete_event(int fd, int16_t filter)
=======
int Webserv::event_check(struct kevent *event, int kq_return)
>>>>>>> origin
{
    struct kevent temp_event;

    EV_SET(&temp_event, fd, filter, EV_DELETE, 0, 0, NULL);
    if (kevent(this->kq, &temp_event, 1, NULL, 0, NULL) == -1)
        throw Webserv::Webserv_err("kevent error");
}

void Webserv::webserv_evfilt_read(struct kevent *curr_event, std::vector<int> & fds_s)
{
    int client_socket;
    char buf[BUFFER_SIZE] = {0};
    int n = 0;

    if(fds_s.end() != std::find(fds_s.begin(), fds_s.end(), curr_event->ident))
    {
        if((client_socket =  accept(curr_event->ident, NULL, NULL)) < 0)
            throw Webserv::Webserv_err("accept error");
        std::cout << "accept new client: " << client_socket << std::endl;
        change_events(this->change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        change_events(this->change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
        this->clients[client_socket] = "";
    }
    else if (this->clients.find(curr_event->ident)!= this->clients.end())
    {
        n = read(curr_event->ident, buf, BUFFER_SIZE - 1);
        //std::cout << "read " << n << " bytes from " << curr_event->ident << std::endl;
        if (n < 0)
        {
            delete_event(curr_event->ident, EVFILT_READ);
            disconnect_client(curr_event->ident, this->clients);
            return ;
        }
        else if (n == 0)
        {
            delete_event(curr_event->ident, EVFILT_READ);
            disconnect_client(curr_event->ident, this->clients);
            return ;
        }
        else
        {
            buf[n] = '\0';
            this->clients[curr_event->ident] = buf;
        
            std::cout << "received data from " << curr_event->ident << ": " << this->clients[curr_event->ident] << std::endl;

        //char *temp = strdup(this->clients[curr_event->ident].c_str());
        //std::cout << " **** " <<temp <<  "***" << std::endl;
        
        char *temp = ft_strdup(buf, n + 1);

        std::cout << "********************************************************" << std::endl;
        std::cout << "                  DUPLICATED REQUEST DATA : " << std::endl << temp << std::endl;
        std::cout << temp <<  "********************************************************" << std::endl;
        
        
        // Parse HTTP request
        std::stringstream ss(buf);
        std::string request_type, request_path, http_version, request_body;
        ss >> request_type >> request_path >> http_version >> request_body;
            
        std::cout << "request_type : " << request_type << std::endl;
        std::cout << "request_path : " << request_path << std::endl;
        std::cout << "http_version : " << http_version << std::endl;
        std::cout << "request_body : " << request_body << std::endl;
            
        // Open picture file and read contents into memory
        std::ifstream picture_file("1337.jpeg", std::ios::binary);
        std::stringstream picture_data;
        picture_data << picture_file.rdbuf();
        std::string picture_string = picture_data.str();
            
        // Construct HTTP response    
        std::stringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: image/jpeg\r\n";
        response << "Content-Length: " << picture_string.size() << "\r\n";
        response << "request_body: " << request_body << "\r\n";
        response << "Connection: close\r\n";
        response << "\r\n";
        response << picture_string;
        

        //std::cout << "PICTURE SIZE = " << picture_string.size() << std::endl;
            
        // Send HTTP response to client
        send(curr_event->ident, response.str().c_str(), response.str().size(), 0);
            /*
            if (request_type == "GET")
            {
<<<<<<< HEAD
                std::cout << "GET request" << std::endl;
=======
                std::cerr << "accept error" << std::endl;
                throw Webserv_err("accept");
>>>>>>> origin
            }
            else if (request_type == "POST")
            {
                std::cout << "POST request" << std::endl;
            }
            else
            {
<<<<<<< HEAD
                std::cout << "Unknown request" << std::endl;
=======
                std::cerr << "setsockopt error" << std::endl;
                throw Webserv_err("setsockopt");
>>>>>>> origin
            }
            */
        
            /*
            std::stringstream ss(buf);
            std::string request_type, request_path, http_version;
            ss >> request_type >> request_path >> http_version;

            // Open video file
            std::ifstream video_file("slayer.mp4", std::ios::binary);

            // Construct HTTP response headers
            std::stringstream response;
            response << "HTTP/1.1 200 OK\r\n";
            response << "Content-Type: video/mp4\r\n";
            response << "Transfer-Encoding: chunked\r\n";
            response << "\r\n";

            // Send HTTP response headers to client
            send(curr_event->ident, response.str().c_str(), response.str().size(), 0);

            // Send video data to client in chunks
            char chunk_buffer[4096];
            while (!video_file.eof())
            {
<<<<<<< HEAD
                video_file.read(chunk_buffer, sizeof(chunk_buffer));
                int chunk_size = video_file.gcount();

                // Send chunk size as hexadecimal string
                std::stringstream chunk_size_ss;
                chunk_size_ss << std::hex << chunk_size << "\r\n";
                std::string chunk_size_str = chunk_size_ss.str();
                send(curr_event->ident, chunk_size_str.c_str(), chunk_size_str.size(), 0);

                // Send chunk data
                send(curr_event->ident, chunk_buffer, chunk_size, 0);

                // Send chunk terminator
                send(curr_event->ident, "\r\n", 2, 0);
            }

            // Send final chunk terminator
            send(curr_event->ident, "0\r\n\r\n", 5, 0);
            */
        
        free (temp);
        }
    }
}

void Webserv::webserv_evfilt_write(struct kevent *curr_event)
{
    if (this->clients.find(curr_event->ident) != this->clients.end())
    {
        if (this->clients[curr_event->ident] != "")
        {
            if (write(curr_event->ident, this->clients[curr_event->ident].c_str(), this->clients[curr_event->ident].size()) < 0)
            {
                std::cout << "write error" << std::endl;
                delete_event(curr_event->ident, EVFILT_WRITE);
                disconnect_client(curr_event->ident, this->clients);
                return ;  
=======
                throw Webserv_err("fnctl error");
>>>>>>> origin
            }
            else
                this->clients[curr_event->ident].clear();
        }
    }
}


void Webserv::event_check(struct kevent *event_list, int new_events, std::vector<int> & fds_s)
{
    for (int i = 0; i < new_events; i++)
    {
        if (event_list[i].flags & EV_ERROR)
            throw Webserv::Webserv_err("EV_ERROR");
        else if (event_list[i].filter == EVFILT_READ)
            webserv_evfilt_read(&event_list[i], fds_s);
        else if (event_list[i].filter == EVFILT_WRITE)
            webserv_evfilt_write(&event_list[i]);
        //else
            //std::cout << COLOR_RED << "EVENT ERROR" << i << " " << event_list[i].filter << COLOR_RESET << std::endl;
    }
}

<<<<<<< HEAD
void Webserv::run(std::vector<int> & fds_socket)
{
    int new_events;
    
=======
void Webserv::run()
{  
>>>>>>> origin
    std::cout << std::endl << COLOR_GREEN << std::setfill(' ') << 
    std::setw(40) << "Server is running" << COLOR_RESET << std::endl;
    while (1337)
    {
<<<<<<< HEAD
        new_events = kevent(this->kq, &this->change_list[0], this->change_list.size(), this->event_list, EVENT_LIST, &this->timeout);
        //std::cout << "KEVENt RETURN = " << new_events << std::endl;
        this->change_list.clear();
        if (new_events == -1)
            throw Webserv::Webserv_err("kevent failed");
        else
            event_check(event_list, new_events, fds_socket);
        //system("leaks Webserv");
=======
        //system("leaks Webserv");
        std::cout << std::endl << COLOR_BLUE << std::setw(40) << "MULTIPLEXING NEXT IS NEXT STEP" << COLOR_RESET << std::endl;
        sleep(1);
>>>>>>> origin
    }
}

Webserv::Webserv(char *config_file)
{
    // Parse config file and create a configurationSA object
    configurationSA config(config_file);
<<<<<<< HEAD
    this->timeout.tv_sec = 3;
=======
    
    this->timeout.tv_sec = 1;
>>>>>>> origin
    this->timeout.tv_nsec = 0;

    // Create a server object with the configurationSA object
    Servers         server(config);
    this->kq = server.kq;
<<<<<<< HEAD
    this->run(Servers::fd_vector, Servers::location );
=======

    //std::cout << std::endl << COLOR_BLUE << "-> KQ VAL IN WEBSERV CONSTRUCTOR = " << this->kq << COLOR_RESET << std::endl << std::endl;
>>>>>>> origin
}

// default destructor
Webserv::~Webserv(){}