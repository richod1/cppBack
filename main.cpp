
#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include "httpServer/http_server.h"

#define PORT 3000

int main() {
    HttpServer server(PORT);

    // Add middleware to log requests
    server.use([](int socket, const std::map<std::string, std::string>& headers, const std::string& body) {
        std::cout << "Middleware: Received request with headers:\n";
        for (const auto& header : headers) {
            std::cout << header.first << ": " << header.second << "\n";
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
    server.post("/data", [](int socket, const std::map<std::string, std::string>& headers, const std::string& body) {
        std::string response = "HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: " + std::to_string(body.length()) + "\n\n" + body;
        send(socket, response.c_str(), response.length(), 0);
    });

    // Start the file watcher and reload mechanism
    server.watch_and_reload("main.cpp");

    // Start the server
    server.start();

    return 0;
}
