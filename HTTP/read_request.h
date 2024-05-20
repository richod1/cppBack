#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <string>


std::string read_request(int socket){
    char buffer[1024]={0};
    read(socket,buffer,1024);
    return std::string(buffer);
}