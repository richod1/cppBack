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

    private:
        int port;
        std::vector<Handler> middlewares;
        std::map<std::string, Handler>  getRoutes;
        std::map<std::string, Handler> postRoutes;
};