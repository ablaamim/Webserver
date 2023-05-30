#include "methods.hpp"


std::vector<std::string>	listing_directoty(std::string path)
{
	DIR							*d;
	struct dirent				*dir;
	std::vector<std::string>	list_of_files;
	
	d = opendir(path.c_str());
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
			list_of_files.push_back(dir->d_name);
		closedir(d);
	}
	return (list_of_files);
}

void print_vector_of_strings(std::vector<std::string> list_of_files)
{
    for (std::vector<std::string>::iterator it = list_of_files.begin(); it != list_of_files.end(); ++it)
        std::cout << *it << std::endl;
}

void    openFile(Response& resp)
{
    /* check if you have file permissions and set the appropriate status code (404, 403) */
    std::cout << "openFile" << std::endl;
    resp.fs = std::ifstream(resp.resourceFullPath.c_str(), std::ios::binary);
    if (resp.fs.good())
    {
        std::cout << "good" << std::endl;
        resp.fs.seekg(0, std::ios::end);
        std::streampos length = resp.fs.tellg();
        resp.resourceSize = static_cast<size_t>(length);
    }
    else
        resp.status = std::make_pair("404", "Not Found");
    
    /* if anything goes wrong, throw an exeception, NOTE: this will be catched in Webserv.cpp */
    if (resp.status.first != "200")
        throw std::runtime_error(resp.status.second);
}

void    serveFile(Response& resp)
{
    try
    {
        char buf[CHUNCK_SIZE];
        if (resp.currentSize == 0)
        {
            resp.headers["Content-Type"] = getContentType(resp.resourceFullPath);
            openFile(resp);
        }
        resp.fs.seekg(resp.currentSize, std::ios::beg);
        resp.fs.read(buf, CHUNCK_SIZE);
        resp.lastChunkSize = resp.fs.gcount();
        resp.currentSize += resp.lastChunkSize;
        if (resp.currentSize >= resp.resourceSize)
            resp.isCompleted = true;
        resp.body.append(buf, resp.lastChunkSize);
        resp.sendResponse(FULL);
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error(e.what());
    }
}

void    Response::serveDirectory(Response& resp)
{
    // std::cout << "serveDirectory" << std::endl;
    // resp.headers["Content-Type"] = "text/html";
    // resp.body = "<h1> Directory </h1> ";
    if (this->kwargs["auto_index"] == std::vector<std::string>({"off"}))
    {
        resp.status = std::make_pair("403", "Forbidden");
        resp.sendResponse(HEADERS_ONLY);
        return ;
    }
    else
    {
        std::vector<std::string> list_of_files = listing_directoty(resp.resourceFullPath);
        //serve list of directories as html
    
        resp.headers["Content-Type"] = "text/html";
        resp.body = "<h1> Listing Directory Content : </h1> ";
        for (std::vector<std::string>::iterator it = list_of_files.begin(); it != list_of_files.end(); ++it)
        {
            resp.body += "<a href=\"";
            resp.body += *it;
            resp.body += "\">";
            resp.body += *it;
            resp.body += "</a><br>";
        }
    }
    //print_vector_of_strings(list_of_files);
    resp.sendResponse(FULL);
}

void    serveCGI(Response& resp)
{
}

void    serveRedirect(Response& resp)
{
    resp.headers["Location"] = resp.resourceFullPath;
    resp.status = std::make_pair("301", "Moved Permanently");
    resp.sendResponse(HEADERS_ONLY);
}

void    Response::serveGET()
{
    try
    {
        if(this->resourceType == FILE)
            serveFile(*this);
        else if (this->resourceType == DIRECTORY)
            serveDirectory(*this);
        else if (this->resourceType == CGI)
            serveCGI(*this);
        else if (this->resourceType == REDIRECT)
            serveRedirect(*this);
    }
    catch(const std::exception& e)
    {
        throw Response_err(e.what());
    }
}