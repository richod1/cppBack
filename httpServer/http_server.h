#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <functional>
#include <map>
#include <vector>
#include <thread>
#include <sys/wait.h>
#include <csignal>
#include "json.hpp" 

#define PORT 3000

using json = nlohmann::json;

class HttpServer {
public:
    using Handler = std::function<void(int, const std::map<std::string, std::string>&, const json&)>;

    HttpServer(int port) : port(port), server_fd(-1) {}

    void use(Handler middleware) {
        middlewares.push_back(middleware);
    }

    void get(const std::string& path, Handler handler) {
        getRoutes[path] = handler;
    }

    void post(const std::string& path, Handler handler) {
        postRoutes[path] = handler;
    }

    void start() {
        running = true;
        server_thread = std::thread(&HttpServer::run_server, this);
        server_thread.join();
    }

    void stop() {
        running = false;
        if (server_fd != -1) {
            close(server_fd);
            server_fd = -1;
        }
    }

    void watch_and_reload(const std::string &source_file) {
        std::thread([this, source_file]() {
            while (true) {
                std::string command = "inotifywait -e modify " + source_file;
                system(command.c_str());
                std::cout << "Detected change in " + source_file + ". Recompiling and restarting server..." << std::endl;

                stop();
                
                // Recompile the server
                std::string compile_command = "g++ -o http_server " + source_file + " -std=c++11 -lpthread";
                system(compile_command.c_str());

                // Restart the server
                start();
            }
        }).detach();
    }

private:
    int port;
    int server_fd;
    std::vector<Handler> middlewares;
    std::map<std::string, Handler> getRoutes;
    std::map<std::string, Handler> postRoutes;
    std::thread server_thread;
    bool running;

    void run_server() {
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);

        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            perror("setsockopt");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("bind failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, 3) < 0) {
            perror("listen");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        std::cout << "Server listening on port " << port << std::endl;

        while (running) {
            int new_socket;
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                if (!running) break; // Exit the loop if the server is stopped
                perror("accept");
                close(server_fd);
                exit(EXIT_FAILURE);
            }

            std::string request = read_request(new_socket);
            std::map<std::string, std::string> headers = parse_headers(request);
            std::string method, path;
            parse_request_line(request, method, path);
            json body;
            if (headers["Content-Type"] == "application/json") {
                try {
                    body = json::parse(get_request_body(request));
                } catch (json::parse_error& e) {
                    std::cerr << "JSON parse error: " << e.what() << std::endl;
                    std::string response = "HTTP/1.1 400 Bad Request\nContent-Type: application/json\nContent-Length: 24\n\n{\"error\": \"Invalid JSON\"}";
                    send(new_socket, response.c_str(), response.length(), 0);
                    close(new_socket);
                    continue;
                }
            }

            for (const auto &middleware : middlewares) {
                middleware(new_socket, headers, body);
            }

            if (method == "GET" && getRoutes.find(path) != getRoutes.end()) {
                getRoutes[path](new_socket, headers, body);
            } else if (method == "POST" && postRoutes.find(path) != postRoutes.end()) {
                postRoutes[path](new_socket, headers, body);
            } else {
                std::string response = "HTTP/1.1 404 NOT FOUND\nContent-Type: text/plain\nContent-Length: 9\n\nNot Found";
                send(new_socket, response.c_str(), response.length(), 0);
                close(new_socket);
            }
        }

        close(server_fd);
        std::cout << "Server stopped" << std::endl;
    }

    std::string read_request(int socket) {
        char buffer[2048] = {0};
        read(socket, buffer, 2048);
        return std::string(buffer);
    }

    std::map<std::string, std::string> parse_headers(const std::string &request) {
        std::map<std::string, std::string> headers;
        std::istringstream request_stream(request);
        std::string line;
        while (std::getline(request_stream, line) && line != "\r") {
            size_t pos = line.find(": ");
            if (pos != std::string::npos) {
                headers[line.substr(0, pos)] = line.substr(pos + 2, line.length() - pos - 3);
            }
        }
        return headers;
    }

    void parse_request_line(const std::string &request, std::string &method, std::string &path) {
        std::istringstream request_stream(request);
        request_stream >> method >> path;
    }

    std::string get_request_body(const std::string &request) {
        size_t pos = request.find("\r\n\r\n");
        if (pos != std::string::npos) {
            return request.substr(pos + 4);
        }
        return "";
    }
};