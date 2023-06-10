#include "../Response.hpp"

void Response::list_directories_recursive(
    std::string &path, std::vector<std::string> &directoryList) {
   DIR *dir;
   struct dirent *ent;

   if ((dir = opendir(path.c_str())) != NULL) {
      while ((ent = readdir(dir)) != NULL) {
         if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
            std::string entryName = ent->d_name;
            directoryList.push_back(entryName);
         }
      }
      closedir(dir);
   }
}

std::vector<std::string> Response::listing_directory(std::string &path) {
   std::vector<std::string> directoryList;
   list_directories_recursive(path, directoryList);
   return directoryList;
}

void print_vector_of_strings(std::vector<std::string> list_of_files) {
   for (std::vector<std::string>::iterator it = list_of_files.begin();
        it != list_of_files.end(); ++it)
      std::cout << *it << std::endl;
}

void openFile(Response &resp) {
   if (access(resp.resourceFullPath.c_str(), F_OK) != 0)
      resp.serveERROR("404", "Not Found");
   if (access(resp.resourceFullPath.c_str(), R_OK) != 0)
      resp.serveERROR("403", "Forbidden");
   resp.fs = new std::ifstream(resp.resourceFullPath.c_str(), std::ios::binary);
   if (resp.fs->good()) {
      resp.fs->seekg(0, std::ios::end);
      std::streampos length = resp.fs->tellg();
      resp.resourceSize = static_cast<size_t>(length);
      std::cout << "resourceSize: " << resp.resourceSize << std::endl;
   }
}

void Response::serveFile(Response &resp) {
   try {
      char buf[CHUNCK_SIZE];
      if (resp.currentSize == 0) 
      {
         resp.headers["Content-Type"] = getContentType(resp.resourceFullPath);
         openFile(resp);
      }
      resp.fs->seekg(resp.currentSize, std::ios::beg);
      resp.fs->read(buf, CHUNCK_SIZE);
      resp.lastChunkSize = resp.fs->gcount();
      resp.currentSize += resp.lastChunkSize;
      if (resp.currentSize >= resp.resourceSize) resp.isCompleted = true;
      resp.body.append(buf, resp.lastChunkSize);
      resp.sendResponse(FULL);
   } catch (const std::exception &e) {
      throw Response_err(e.what());
   }
}

void Response::serveDirectory(Response &resp) {
   if (this->kwargs["auto_index"][0] == "off") {
      resp.serveERROR("403", "Forbidden");
   } else {
      std::vector<std::string> list_of_files =
          resp.listing_directory(resp.resourceFullPath);
      resp.headers["Content-Type"] = "text/html";
      resp.body = "<h1> Index of " + resp.resourceFullPath + "</h1> ";
      for (std::vector<std::string>::iterator it = list_of_files.begin();
           it != list_of_files.end(); ++it) {
         if (resp._req.path == "/") resp._req.path = "";
         std::string path = resp._req.path + "/" + *it;
         resp.body += "<a href=\"";
         resp.body += path;
         resp.body += "\">";
         resp.body += *it;
         resp.body += "</a><br>";
      }
   }
   resp.sendResponse(FULL);
}

void Response::serveGET() {
   try {
      if (this->isCGI)
         this->serveCGI();
      else if (this->resourceType == FILE)
         serveFile(*this);
      else if (this->resourceType == DIRECTORY)
         serveDirectory(*this);
   } catch (const std::exception &e) {
      std::cout << "exception 2 " << e.what() << std::endl;
      throw Response_err(e.what());
   }
}