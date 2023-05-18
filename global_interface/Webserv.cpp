
#include "../MainInc/main.hpp"
#include "../response/Response.hpp"

void Webserv::change_events(uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    if (kevent(this->kq, &temp_event, 1, NULL, 0, NULL) == -1)
        throw Webserv::Webserv_err("kevent error change event");
}

void Webserv::disconnect_client(int client_fd, std::map<int, std::string>& clients, std::string str)
{
    std::cout << str << " : client disconnected: " << client_fd << std::endl;
    close(client_fd);
    clients.erase(client_fd);
}

void Webserv::delete_event(int fd, int16_t filter, std::string str)
{
    struct kevent temp_event;

    if (this->clients.find(fd) == this->clients.end())
        return ;
    EV_SET(&temp_event, fd, filter, EV_DELETE, 0, 0, NULL);
    if (kevent(this->kq, &temp_event, 1, NULL, 0, NULL) == -1)
        throw Webserv::Webserv_err(str + "kevent error delete event");
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
        change_events(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        this->clients[client_socket] = "";
    }
    else if (this->clients.find(curr_event->ident)!= this->clients.end())
    {
        n = read(curr_event->ident, buf, BUFFER_SIZE - 1);
        this->clients[curr_event->ident] = "";
        if (n <= 0)
        {
            delete_event(curr_event->ident, EVFILT_READ, "read evfil");
            disconnect_client(curr_event->ident, this->clients, "read");
        }
        
        while(n > 0)
        {
            buf[n] = '\0';
            this->clients[curr_event->ident] += buf;
            
            std::cout << "received data from " << curr_event->ident << ": " 
            << std::endl << this->clients[curr_event->ident] << std::endl;
            
            n = read(curr_event->ident, buf, BUFFER_SIZE - 1);

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
            char chunk_buffer[1024];
            while (!video_file.eof())
            {
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
        }   
    }
}

void Webserv::webserv_evfilt_write(struct kevent *curr_event)
{
    if (this->clients.find(curr_event->ident) != this->clients.end())
    {
        if (this->clients[curr_event->ident] != "")
        {
            delete_event(curr_event->ident, EVFILT_WRITE, "write evfil");
            /*
                so here we need to add client to the reponseList
            */
            if (write(curr_event->ident, this->clients[curr_event->ident].c_str(), this->clients[curr_event->ident].size()) < 0)
            {
                std::cout << "write error" << std::endl;
                disconnect_client(curr_event->ident, this->clients, "write");
            }
            else
                this->clients[curr_event->ident].clear();
        }
    }
}

void Webserv::event_check(int new_events, std::vector<int> & fds_s)
{
    for (int i = 0; i < new_events; i++)
    {
        if (this->event_list[i].flags & EV_ERROR)
            disconnect_client(this->event_list[i].ident, this->clients, "EV_ERROR");
        else if (this->event_list[i].flags & EV_EOF)
        {
            delete_event(this->event_list[i].ident, EVFILT_READ, "read eof");
            disconnect_client(this->event_list[i].ident, this->clients, "EV_EOF");
        }
        else if (this->event_list[i].filter == EVFILT_READ)
            webserv_evfilt_read(&this->event_list[i], fds_s);
        else if (this->event_list[i].filter == EVFILT_WRITE)
            webserv_evfilt_write(&this->event_list[i]);
        else
            std::cout << COLOR_RED << "EVENT ERROR" << i << " " << this->event_list[i].filter << COLOR_RESET << std::endl;
    }
}

void Webserv::run(std::vector<int> & fds_socket, configurationSA &config)
{
    int new_events;
    
    std::cout << std::endl << COLOR_GREEN << std::setfill(' ') << 
    std::setw(40) << "Server is running size " << fds_socket.size() << COLOR_RESET << std::endl;
    signal(SIGPIPE, SIG_IGN);
    while (1337)
    {
        new_events = kevent(this->kq, NULL, 0, this->event_list, fds_socket.size(), NULL);
        if (new_events == -1)
            throw Webserv::Webserv_err("kevent failed");
        else
            event_check(new_events, fds_socket);
        Response response(config);
    }
    //std::cout << "Wesaaal lehenaa " << std::endl;
}

Webserv::Webserv(configurationSA &config)
{
    // Create a server object with the configurationSA object
    Servers         server(config);
    this->kq = server.kq;
    this->event_list = new struct kevent [Servers::fd_vector.size()];
    this->run(Servers::fd_vector, config);
}

// default destructor
Webserv::~Webserv(){delete [] event_list;}