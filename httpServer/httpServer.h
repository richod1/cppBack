#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <functional>
#include <vector>
#include <map>
#include <netinet/in.h>


class HttpServer{
public:
    using Handler=std::function<void(int,const std::map<std::string,std::string>&,const std::string&)>;
    HttpServer(int port) :port(port){}
    void use(Handler middlware){
        middlewares.push_back(middlware);
    }

    void get(const std::string& path,Handler handler){
        getRoutes[path]=handler;

    }
    void post(const std::string& path,Handler handler){
        postRoutes[path]=handler;

    }
    // server listner
    void listen(){
        int server_fd,new_socket;
        struct sockaddr_in address;
        int opt=1;
        int addrlen=sizeof(address);

        if((server_fd=socket(AF_INET,SOCK_STREAM,0))==0){
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
        if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))){
            perror("setsockopt");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        address.sin_family=AF_INET;
        address.sin_addr.s_addr=INADDR_ANY;
        address.sin_port=htons(port);


        // checking binding
          if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("bind failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        if (::listen(server_fd, 3) < 0) {
            perror("listen");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        std::cout<<"Server is listening port :"<<port<<std::endl;


    while(true){
        if((new_socket=accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen)) < 0){
            perror("accept");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
    }

    std::string request=read_request(new_socket);
    }

    private:
        int port;
        std::vector<Handler> middlewares;
        std::map<std::string, Handler>  getRoutes;
        std::map<std::string, Handler> postRoutes;


        // function for reading request
        std::string read_request(int socket){
            char buffer[2048]={0};
            read(socket,buffer,2048);
            return std::string(buffer);
        }

        std::map<std::string,std::string> parse_headers(const std::string &request){
            std::map<std::string,std::string> headers;
            std::istringstream request_stream(request);
            std::string line;

            while(std::getline(request_stream,line) && line != "\r"){
                size_t pos=line.find(": ");
                if(pos != std::string::npos){
                    headers[line.substr(0,pos)]=line.substr(pos + 2,line.length()-pos-3);
                }
            }
            return headers;

        }
};