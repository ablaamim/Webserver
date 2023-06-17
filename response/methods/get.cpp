#include "../../MainInc/main.hpp"
#include "../Response.hpp"

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
         openFile(resp);
         resp.headers["Content-Type"] = getContentType(resp.resourceFullPath);
         resp.headers["Content-Length"] = std::to_string(this->resourceSize);
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


void Response::serveGET() {
   try
   {
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