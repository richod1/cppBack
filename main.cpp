// Example usage of httpServer
#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include "httpServer/httpServer.h"

#define PORT 8080

int main(){
    HttpServer server(PORT);

    // middlware for loging
    server.use([](int socket,const std::map<std::string,std::string>& headers,const std::string& body){
        std::cout<<"Middleware: Received request with headers: \n";
        for(const auto& header:headers){
            std::cout<<header.first<<": "<<header.second<<"\n";
        }
    });

    // defining the get route
    server.get("/",[](int socket,const std::map<std::string,std::string>& headers,const std::string& body){
        std::string response="HTTP/1.1 200 OK \nContent-Type: text/plain\nContent-Length: 12\n\nHello World";
        send(socket,response.c_str(),response.length(),0);
        close(socket);
    });

    // having a specific route
    server.get("/api",[](int socket,const std::map<std::string,std::string>& headers,const std::string& body){
        std::string response="HTTP/1.1 200 OK \nContent-Type: text/plain\nContent-Length: 5\n\nHello Api";
        send(socket,response.c_str(),response.length(),0);
        close(socket);
    });

    server.post("/data",[](int socket,const std::map<std::string,std::string>& headers,const std::string& body){
        std::string response="HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: "+ std::to_string(body.length()) + "\n\n" + body;
        send(socket,response.c_str(),response.length(),0);
        close(socket);
    });
    server.watch_and_reload("main.cpp");
    server.start();

    return 0;
}