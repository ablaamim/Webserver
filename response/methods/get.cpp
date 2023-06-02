#include "methods.hpp"

void Response::list_directories_recursive(std::string& path, std::vector<std::string>& directoryList)
{
    DIR* dir;
    struct dirent* ent;

    if ((dir = opendir(path.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                std::string entryName = ent->d_name;
                directoryList.push_back(entryName);
            }
            //this->resourceFullPath = path;
        }
        closedir(dir);
    }
}

std::vector<std::string> Response::listing_directory(std::string& path)
{
    std::vector<std::string> directoryList;
    list_directories_recursive(path, directoryList);
    return directoryList;
}

void print_vector_of_strings(std::vector<std::string> list_of_files)
{
    for (std::vector<std::string>::iterator it = list_of_files.begin(); it != list_of_files.end(); ++it)
        std::cout << *it << std::endl;
}

void    openFile(Response& resp)
{       
    if (access(resp.resourceFullPath.c_str(), F_OK) != 0)
        resp.serveERROR("404", "Not Found");
    if (access(resp.resourceFullPath.c_str(), R_OK) != 0)
        resp.serveERROR("403", "Forbidden");
    resp.fs = new std::ifstream(resp.resourceFullPath.c_str(), std::ios::binary);
    if (resp.fs->good())
    {
        resp.fs->seekg(0, std::ios::end);
        std::streampos length = resp.fs->tellg();
        resp.resourceSize = static_cast<size_t>(length);
    }
}

void    Response::serveFile(Response& resp)
{
    try
    {
        char buf[CHUNCK_SIZE];
        if (resp.currentSize == 0) /* first time we open the file, no need to re open it when the response its chuncked */
        {
            resp.headers["Content-Type"] = getContentType(resp.resourceFullPath);
            openFile(resp);
        }
        resp.fs->seekg(resp.currentSize, std::ios::beg);
        resp.fs->read(buf, CHUNCK_SIZE);
        resp.lastChunkSize = resp.fs->gcount();
        resp.currentSize += resp.lastChunkSize;
        if (resp.currentSize >= resp.resourceSize)
            resp.isCompleted = true;
        resp.body.append(buf, resp.lastChunkSize);
        resp.sendResponse(FULL);
    }
    catch(const std::exception& e)
    {
        throw Response_err(e.what());
    }
}

void    Response::serveDirectory(Response& resp)
{
    // std::cout << "serveDirectory" << std::endl;
    // resp.headers["Content-Type"] = "text/html";
    // resp.body = "<h1> Directory </h1> ";
    // if (this->kwargs["auto_index"] == std::vector<std::string>({"off"}))
    // {
    //     resp.status = std::make_pair("403", "Forbidden");
    //     resp.sendResponse(HEADERS_ONLY);
    //     return ;
    // }
    // else
    // {
        std::vector<std::string> list_of_files = resp.listing_directory(resp.resourceFullPath);
        resp.headers["Content-Type"] = "text/html";
        resp.body = "<h1> Index of " + resp.resourceFullPath + "</h1> ";
        for (std::vector<std::string>::iterator it = list_of_files.begin(); it != list_of_files.end(); ++it)
        {
            if (resp._req.path == "/")
                resp._req.path = "";
            std::string path = resp._req.path + "/" + *it;
            resp.body += "<a href=\"";
            resp.body += path;
            resp.body += "\">";
            resp.body += *it;
            resp.body += "</a><br>";
        }
        //print_vector_of_strings(list_of_files);
    //}
    resp.sendResponse(FULL);
}

void    serveCGI(Response& resp)
{
    (void)resp;
    std::cout << "serveCGI" << std::endl;
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
        //this->print_kwargs();
        //std::cout << this->resourceType << std::endl;
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