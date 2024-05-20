#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <netinet/in.h>
#include "HTTP/handle_request.h"
#include "HTTP/parse_request.h"
#include "HTTP/read_request.h"

#define PORT 8080
int main(){

    int server_fd,new_socket;

    struct sockaddr_in address;

    int opt=1;
    int addrlen=sizeof(address);

    if((server_fd=socket(AF_INET,SOCK_STREAM,0))==0){
        perror("sockt failed");
        exit(EXIT_FAILURE);
    }

    if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }


    address.sin_family=AF_INET;
    address.sin_addr.s_addr=INADDR_ANY;
    address.sin_port=htons(PORT);

    // binding port
    if(bind(server_fd,(struct sockaddr *)&address,sizeof(address))<0){
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    // listen to poat
    if(listen(server_fd,3)<0){
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);

    }

    std::cout<<"Server listening on port "<<PORT<<std::endl;


    while(true){
        if(new_socket=accept(server_fd,(struct sockaddr *)&address,(socklen_t*)&addrlen)){
            perror("accept");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        std::string request=read_request(new_socket);
        std::string path=parse_request(request);
        handle_request(new_socket,path);
    }

    close(server_fd);
    return 0;

}