#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sstream>


std::string parse_request(const std::string &request){
    std::istringstream request_stream(request);
    std::string method,path,version;
    request_stream>>method>>path>>version;
    return path;

}