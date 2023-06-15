#include "../../MainInc/main.hpp"
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
      resp.serveERROR(_CS_404, _CS_404_m);
   if (access(resp.resourceFullPath.c_str(), R_OK) != 0)
      resp.serveERROR(_CS_403, _CS_403_m);
   resp.fs = new std::ifstream(resp.resourceFullPath.c_str(), std::ios::binary);
   if (resp.fs->good()) {
      resp.fs->seekg(0, std::ios::end);
      std::streampos length = resp.fs->tellg();
      resp.resourceSize = static_cast<size_t>(length);
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

int getFileSize(std::string &path) {
   struct stat st;
   stat(path.c_str(), &st);
   return st.st_size;
}

std::string getLastModified(std::string &path) {
   struct stat st;
   stat(path.c_str(), &st);
   std::string last_modified = "<td>" + std::string(ctime(&st.st_mtime)) + "</td>";
   return last_modified;
}

void Response::serveDirectory(Response &resp)
{
   if (this->kwargs["auto_index"][0] == "off")
      resp.serveERROR(_CS_403, _CS_403_m);
   else
   {
      std::vector<std::string> list_of_files =
          resp.listing_directory(resp.resourceFullPath);
      resp.headers["Content-Type"] = "text/html";
      resp.body += "<link rel=\"stylesheet\" href=\"../../font-awesome-4.7.0/css/font-awesome.min.css\">";
      resp.body += "<link rel=\"stylesheet\" href=\"../../font-awesome-4.7.0/css/index.css\">";
      resp.body += "<h1> Index of " + resp.resourceFullPath + "</h1> ";

      // List directories on browser with some css
      //resp.body += "<style> table, th, td { border: 1px solid black; border-collapse: collapse; } </style>";
      resp.body += "<table style=\"width:100%\"> <tr> <th>Name</th> <th>Type</th> <th>Size</th> <th>Last Modified</th> </tr>";
      for (std::vector<std::string>::iterator it = list_of_files.begin();
           it != list_of_files.end(); ++it)
      {
         std::string file_name = *it;
         std::string icon = "<i class=\"fa fa-folder\" aria-hidden=\"true\"></i>";
         if (file_name.find(".") == std::string::npos)
            file_name += "/";
         else
         {
            icon = "<i class=\"fa fa-file-o\" aria-hidden=\"true\"></i>";
            file_name += " ";
         }
         std::string file_path = resp.resourceFullPath + "/" + file_name;
         std::string file_type = getContentType(file_path);
         std::string file_size = std::to_string(getFileSize(file_path)) + " bytes";
         std::string last_modified = getLastModified(file_path);

         resp.body += "<tr> <td><a href=\"" + file_name + "\">" + icon + file_name + "</a></td> <td>" + file_type + "</td> <td>" + file_size + "</td> " + last_modified + "</tr>";
      }
   }
   resp.sendResponse(FULL);
}

void Response::serveGET() {
   try
   {
      // std::cout << this->resourceFullPath << std::endl;
      if (this->isCGI)
         this->serveCGI();
      else if (this->resourceType == FILE)
         serveFile(*this);
      else if (this->resourceType == DIRECTORY)
         serveDirectory(*this);
   }
   catch (const std::exception &e)
   {
      throw Response_err(e.what());
   }
}