#include "../MainInc/main.hpp"


/********************************************************************/
/*                     Constructors  and Destructor                 */
/********************************************************************/

Request::Request()
{
    this->headers_done = false;
    this->content_length = 0;
    this->error = 0;
    this->first_line = false;
    this->is_chuncked = false;
    this->file_body_name = "";
    this->erro_msg = "";
    this->file = NULL;
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
    this->fd_server = ob.fd_server;
    this->method = ob.method;
    this->path = ob.path;
    this->version = ob.version;
    this->content_length = ob.content_length;
    if (this->file_body_name != "")
        std::remove(this->file_body_name.c_str());
    this->file_body_name = ob.file_body_name;
    this->file = ob.file;
    this->erro_msg = ob.erro_msg;
    this->error = ob.error;
    return *this;
}



std::ostream & operator<<(std::ostream & o, Request const & ref)
{
    param para =  ref.params;
    std::cout << std::endl << COLOR_GREEN << "Request :" << COLOR_RESET << std::endl;
    std::cout << "Server fd :" << ref.fd_server << std::endl;
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
    while (it != this->params.end())
    {
        std::cout << COLOR_BLUE << it->first << " : " << COLOR_RESET << it->second << std::endl;
        ++it;
    }
    std::cout << COLOR_BLUE <<  " file to upload : " << COLOR_RESET << this->file_body_name << std::endl;
}
void    Request::reset_request()
{
    this->headers_done = false;
    this->first_line = false;
    this->is_chuncked = false;
    this->error = 0;
    this->erro_msg = "";
    this->params.clear();
    std::remove(this->file_body_name.c_str());
    if (this->file)
        delete(this->file);
    this->file_body_name = "";
    this->file = NULL;
}

int Request::check_readed_bytes()
{
    if (this->params.find("Content-Length") != this->params.end())
    {
        if (this->file && std::stoi(this->params["Content-Length"]) != this->file->tellp())
        {
            this->is_chuncked = true;
            return _CHUNCKED_REQUEST;
        }
        else
        {
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
    int  indx;

    indx = line.find(": ");
    if (indx != -1)
        this->params[line.substr(0, indx)] = line.substr(indx + 2);
}

int Request::open_file_for_reponse(std::string str)
{
    this->file_body_name = _TMP_FILE_ + this->gen_random();
    this->file = new std::ofstream(this->file_body_name, std::ios::binary);
    return (get_chuncked_msg(str));
}

void Request::parse_headers(std::string str)
{
    int line;

    while ((line = str.find("\r\n")) != -1)
    {
        if (!this->first_line)
            this->get_firstline(str.substr(0, line));
        else
            this->get_other_lines(str.substr(0, line));
        str = str.substr(line + 2);
    }
    if (str != "")
        this->get_other_lines(str);
}

void Request::get_content_extension(void)
{
    int      line;

    if (this->params.find("Content-Type") != this->params.end())
    {
        this->params["Content-Extension"] = this->params["Content-Type"];
        this->content_type = this->params["Content-Type"];
        if ((line = this->params["Content-Type"].rfind("/")) != -1)
            this->params["Content-Extension"] = this->params["Content-Type"].substr(line + 1);
    }
}

int Request::get_headers(std::string str)
{
    int line;
    std::string str1 = "";

    if ((line = str.rfind("\r\n\r\n")) != -1 && str.rfind("\r\n") != std::string::npos)
    {
        str1 = str.substr(line + 4);
        str = str.substr(0 ,line);
    }
    else
    {
        this->error = std::stoi(_CS_400);
        this->erro_msg = _CS_400_m;
        return _UKNOWN_PROTOCOL;
    }
    this->parse_headers(str);
    this->get_content_extension();
    this->headers_done = true;
    if (this->method != POST)
        return _PARSE_REQUEST_DONE;
    if (str1 != "")
        return (open_file_for_reponse(str1));
    return (check_readed_bytes());
}

int Request::get_chuncked_msg(std::string str)
{
    int              line;
    std::string         tmp_str;
    std::stringstream   ss;
    int                 len;

    line = str.find("\r\n\r\n");
    while (line != -1)
    {
        tmp_str = str.substr(0, line);
        if ((line = tmp_str.find("\r\n")) != -1)
        {
            tmp_str = str.substr(line + 2);
            try
            {
                ss << std::hex << str.substr(0, line);
                ss >> len;
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        *this->file << tmp_str;
        str = str.substr(line + 5);
        line = str.find("0\r\n\r\n");
    }
    if (line == -1)
    {
        *this->file << str;
    }
    return (check_readed_bytes());
}

int Request::parse_request(std::string str)
{
    if (!str.size())
        return _ERR_PARSE_REQUEST;
    else if (!this->headers_done)
        return (this->get_headers(str));
    else if (this->is_chuncked)
        return(this->get_chuncked_msg(str));
    return 1;
}