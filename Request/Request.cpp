#include "../MainInc/main.hpp"


/********************************************************************/
/*                     Constructors  and Destructor                 */
/********************************************************************/
Request::Request()
{
    this->headers_done = false;
    this->first_line = false;
    this->is_chuncked = false;
    this->file_body_name = _TMP_FILE_ + gen_random();
    this->file = new std::ofstream(this->file_body_name, std::ios::binary);
}

Request::Request(Request const & ob)
{
    *this = ob;
}

Request::~Request(){};


/********************************************************************/




/********************************************************************/
/*                            Operator Overload                     */
/********************************************************************/


Request & Request::operator=(Request const &ob)
{
    this->headers_done = ob.headers_done;
    this->first_line = ob.first_line;
    this->is_chuncked = ob.is_chuncked;
    this->params = ob.params;
    this->fd_accept = ob.fd_accept;
    this->fd_server = ob.fd_server;
    this->method = ob.method;
    this->path = ob.path;
    this->version = ob.version;
    this->content_length = ob.content_length;
    this->body = ob.body;
    std::remove(this->file_body_name.c_str());
    this->file_body_name = ob.file_body_name;
    this->file = ob.file;
    return *this;
}



std::ostream & operator<<(std::ostream & o, Request const & ref)
{
    param para =  ref.params;
    std::cout << std::endl << COLOR_GREEN << "Request :" << COLOR_RESET << std::endl;
    std::cout << "Server fd :" << ref.fd_server <<  "  Client fd : " << ref.fd_accept << std::endl;
    for(it_param it = para.begin(); it != para.end(); it++ )
        std::cout << it->first << " : '" << it->second << "'" << std::endl;
    return o;
}

/********************************************************************/


/********************************************************************/
/*                          Public functions                        */
/********************************************************************/

/*
        Cette fonction génère un nom aléatoire pour un 
        fichier temporaire dans le dossier tmp
*/

std::string Request::gen_random() 
{
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    srand(time(0));

    tmp_s.reserve(14);
    for (int i = 0; i < 14; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return tmp_s;
}

void    Request::print_params()
{
    it_param it = this->params.begin();
    std::cout << COLOR_RED << "   Display Request params " << COLOR_RESET << std::endl;
    while (it++ != this->params.end())
        std::cout << COLOR_BLUE << it->first << " : " << COLOR_RESET << it->second << std::endl;
}
void    Request::reset_request()
{
    this->headers_done = false;
    this->first_line = false;
    this->is_chuncked = false;
    this->params.clear();
    if (this->file_body_name.size())
        std::remove(this->file_body_name.c_str());
    this->file_body_name = "";
    this->file = NULL;
}

void    Request::reset_file()
{
    if (this->file_body_name == "")
        return ;
    std::remove(this->file_body_name.c_str());
    this->file_body_name = "";
    this->file = NULL;
}

int Request::check_readed_bytes()
{
    if (this->params.find("Content-Length") != this->params.end())
    {
        if (std::stoi(this->params["Content-Length"]) != this->file->tellp())
        {
            std::cout << "check Content length: " << this->params["Content-Length"] << std::endl;
            std::cout << "_CONTENT_: " << this->file->tellp() << std::endl;
            this->is_chuncked = true;
            return _CHUNCKED_REQUEST;
        }
        else
        {
            this->file->close();
            this->is_chuncked = false;
        }
    }
    return _PARSE_REQUEST_DONE;
}

void Request::get_firstline(std::string line)
{
    std::string         str;
    std::stringstream   file(line);
    int                 i = 0;

    //std::cout << "Parsing First line " << std::endl;
    while (std::getline(file, str, ' '))
    {
        switch (i)
        {
            case 0 :
                this->params["Method"] = str;
                this->method = str;
            case 1 :
                this->params["Url"] = str;
                this->path = str;
            case 2 : 
                this->params["Protocol Version"] = str;
                this->version = str;
        }
        i++;
    }
    this->first_line = true;
}

void Request::get_other_lines(std::string line)
{
    size_t  indx;

    //std::cout << COLOR_GREEN << "Parsing Other lines '" << line << "'" << COLOR_RESET << std::endl;
    indx = line.find(": ");
    if (indx != std::string::npos)
        this->params[line.substr(0, indx)] = line.substr(indx + 2);
}

int Request::get_headers(std::string str)
{
    size_t line;
    std::string str1 = "";

    std::cout << "Parsing headers " << std::endl;
    if ((line = str.rfind("\r\n\r\n")) != std::string::npos)
    {
        str1 = str.substr(line + 4);
        str = str.substr(0 ,line);
    }
    while ((line = str.find("\r\n")) != std::string::npos)
    {
        if (!this->first_line)
            this->get_firstline(str.substr(0, line));
        else
            this->get_other_lines(str.substr(0, line));
        str = str.substr(line + 2);
    }
    this->headers_done = true;
    if (str1 != "")
    {
        std::cout << "Man headers le9a chumck messages " << std::endl;
        return (get_chuncked_msg(str1));
    }
    return (check_readed_bytes());
}

int Request::get_chuncked_msg(std::string str)
{
    size_t              line;
    std::string         tmp_str;
    std::stringstream   ss;
    int                 len;

    std::cout << COLOR_YELLOW << "Parsing chunck " << COLOR_RESET << std::endl;
    // std::cout << COLOR_RED << "----------------------------------" << std::endl;
    // std::cout << COLOR_GREEN << str << std::endl << std::endl;
    // std::cout << COLOR_RED << "----------------------------------" << std::endl;
    line = str.find("0\r\n\r\n");
    while (line != std::string::npos)
    {
        std::cout << COLOR_RED << "line loop" << COLOR_RESET<< std::endl;
        tmp_str = str.substr(0, line);
        *this->file << tmp_str;
        str = str.substr(line + 5);
        line = str.find("0\r\n\r\n");
    }
    /*if ((line = str.find("\r\n")) != std::string::npos)
    {
        try
        {
            ss << std::hex << str.substr(0, line);
            ss >> len;
            std::cout << "to read " << len << std::endl;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
        //std::cout << COLOR_RED << "before byte" << COLOR_RESET<< std::endl;
        str = str.substr(line + 2);
    }*/
    if (line == std::string::npos)
    {
        std::cout << COLOR_RED << "NO limits " << str.size()<< COLOR_RESET<< std::endl;
        *this->file << str;
    }
    return (check_readed_bytes());
}

int Request::parse_request(char * str)
{
    if (!str)
        return _ERR_PARSE_REQUEST;
    else if (!this->headers_done)
        return (this->get_headers(std::string(str)));
    else if (this->is_chuncked)
        return(this->get_chuncked_msg(std::string(str)));
    return 1;
}