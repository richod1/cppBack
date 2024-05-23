
#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include "httpServer/http_server.h"

#define PORT 3000

int main() {
    HttpServer server(PORT);

    // Add middleware to log requests
    server.use([](int socket, const std::map<std::string, std::string>& headers, const json& body) {
        std::cout << "Middleware: Received request with headers:\n";
        for (const auto& header : headers) {
            std::cout << header.first << ": " << header.second << "\n";
        }
        if(!body.is_null()){
            std::cout<<"Body "<<body.dump() <<std::endl;
        }
    });

    // Define a GET route for "/"
    server.get("/", [](int socket, const std::map<std::string, std::string>& headers, const std::string& body) {
        std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
        send(socket, response.c_str(), response.length(), 0);
        close(socket);
    });



    // Define a GET route for "/hello"
    server.get("/hello", [](int socket, const std::map<std::string, std::string>& headers, const std::string& body) {
        std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 5\n\nHello";
        send(socket, response.c_str(), response.length(), 0);
        close(socket);
    });

    // Define a POST route for "/data"
    server.post("/data", [](int socket, const std::map<std::string, std::string>& headers, const json& body) {
        std::string response_json = body.dump();
        std::string response = "HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: " + std::to_string(response_json.length()) + "\n\n" + response_json;
        send(socket, response.c_str(), response.length(), 0);
        close(socket);
    });

    // sending json data
    server.get("/api",[](int socket,const std::map<std::string, std::string>& headers,const json& body){
        json response_json={{"name","degraft frimpong"}};
        std::string response="HTTP/1.1 OK\nContent-Type: application/json\nContent-Length: " +std::to_string(response_json.dump().length())+ "\n\n" +response_json.dump();
        send(socket,response.c_str(),response.length(),0);
        close(socket); 
    });

    // Start the file watcher and reload mechanism
    server.watch_and_reload("main.cpp");

    // Start the server
    server.start();

    return 0;
}
