#include "../MainInc/main.hpp"
#include "Response.hpp"

void Response::list_directories_recursive(std::string &path, std::vector<std::string> &directoryList)
{
   DIR *dir;
   struct dirent *ent;

   if ((dir = opendir(path.c_str())) != NULL)
   {
      while ((ent = readdir(dir)) != NULL)
      {
         if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
         {
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

std::string getFileSize(std::string &path) {
   struct stat st;
   std::string size = "-";

   stat(path.c_str(), &st);
   if ((st.st_size / 1000000000) > 0)
      size = std::to_string(st.st_size / 10000000000) + " Gb";
   else if ((st.st_size / 1000000) > 0)
      size = std::to_string(st.st_size / 1000000) + " Mb";
   else if ((st.st_size / 1000) > 0)
      size = std::to_string(st.st_size / 1000) + " Kb";
   else
      size = std::to_string(st.st_size) + " Bytes";
   return size;
}

std::string getLastModified(std::string &path) {
   struct stat st;
   stat(path.c_str(), &st);
   std::string last_modified = "<td>" + std::string(ctime(&st.st_mtime)) + "</td>";
   return last_modified;
}

std::string get_content_type(std::string file_path)
{

   if (file_path[file_path.length() - 1] == '/')
      return "Directory";
   std::string file_extension = file_path.substr(file_path.find_last_of(".") + 1);
   for (std::string::iterator it = file_extension.begin(); it != file_extension.end(); ++it)
   {
      if (*it >= 'A' && *it <= 'Z')
         *it += 32;
      if (*it == ' ')
         *it = '_';
      if (*it >= 'a' && *it <= 'z')
         return file_extension;
   }
   return "text/html";
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
      resp.body = "<style>@import url('https://fonts.googleapis.com/css?family=Poppins:400,500,600,700&display=swap');\
                     *{box-sizing: border-box;font-family: 'Poppins', Arial, Helvetica, sans-serif;margin: 0;padding: 0;}\
                     body{padding: 3%;background: #d1d1d1;}\
                     table{background: #fff;max-width: 1366px;margin: 0 auto;border:none;border-collapse:collapse;table-layout: fixed;}\
                     table th {font-size: 18px;color: #fff;line-height: 1.4;text-transform: uppercase;background-color: #36304a;padding-top: 24px;padding-bottom: 20px;padding-left: 20px;text-align: left;font-weight: 600;}\
                     table td {padding-top: 18px;padding-bottom: 14px;padding-left: 15px;}\
                     table tr:nth-child(even) {background-color: #f3f3f3;}\
                     h1{text-align: center;padding-bottom: 50px;}\
                     table a{color: #36304a;font-size: 16px;text-decoration: none;}\
                     table a i{padding-right: 5px;}\
                     table tr:nth-child(even) a{color: #000;}\
                     table a:hover, table tr:nth-child(even) a:hover{color: #6c7ae0; font-weight: 500;}\
                     table a:hover i.fa, table a:hover i.fa:before{background-color: #6c7ae0}\
                     table a:hover i.fa-file-o, table a:hover i.fa-file-o:before{background-color: #ffffff; border-color: #6c7ae0}\
                     table tr:hover{background-color: #6c7ae02e; cursor: pointer;}\
                     i.fa.fa-folder:before {content: '';width: 50%;height: 2px;border-radius: 0 10px 0 0;background-color: #36304a;position: absolute;top: -2px;left: 0px;}\
                     i.fa.fa-folder {width: 20px;height: 14px;margin: 0 10px -2px 0;position: relative;background-color: #36304a;border-radius: 0 3px 3px 3px;display: inline-block;}\
                     i.fa.fa-file-o {display: inline-block;width: 15px;height: 20px;background: #ffffff;border-radius: 2px;margin: 0 10px -4px 0;border-top-right-radius: 7px;border:1px solid #36304a}\
                     </style>";
      resp.body += "<h1> Index of " + resp.resourceFullPath + "</h1> ";

      resp.body += "<table style=\"width:100%\"> <tr> <th>Name</th> <th>Type</th> <th>Size</th> <th>Last Modified</th> </tr>";
      for (std::vector<std::string>::iterator it = list_of_files.begin();
           it != list_of_files.end(); ++it)
      {
         std::string file_name = *it;
         if (file_name == "Webserv")
            continue;
         std::string icon = "<i class=\"fa fa-folder\" aria-hidden=\"true\"></i>";
         if (file_name.find(".") == std::string::npos)
            file_name += "/";
         else
         {
            icon = "<i class=\"fa fa-file-o\" aria-hidden=\"true\"></i>";
            file_name += " ";
         }
         std::string file_path = resp.resourceFullPath + file_name;
         std::string file_type = get_content_type(file_path);
         std::string file_size = getFileSize(file_path);
         std::string last_modified = getLastModified(file_path);
         resp.body += "<tr> <td><a href=\"" + file_name + "\">" + icon + file_name + "</a></td> <td>" + file_type + "</td> <td>" + file_size + "</td> " + last_modified + "</tr>";
      }
   }
   resp.sendResponse(FULL);
}
