#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <netinet/in.h>


void handle_request(int new_socket,const std::string &path){
std::string response;

if(path=="/"){
    response="HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length 12\n\nHello world";
}else if(path=="/hello"){
    response="HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 5\n\nHello";
}else{
    response="HTTP/1.1 404\nContent-Type: text/plain\nContent-Length: 9\n\nNot Found";
}


send(new_socket,response.c_str(),response.length(),0);
close(new_socket);
}