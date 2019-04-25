#include <sysexits.h>
#include <iostream>
#include <string>
#include "rpc/server.h"
#include "rpc/this_handler.h"

using namespace std;

struct custom_type {
    int x;
    double y;
    std::string str;
    MSGPACK_DEFINE_MAP(x, y, str) // or MSGPACK_DEFINE_MAP
};

int main(){
    cout<<"hello"<<endl;
    rpc::server srv(8080);

    srv.bind("cool_function", [](custom_type const& c) {
        std::cout << "c = { " << c.x << ", " << c.y << ", " << c.str << "}" << std::endl;
    });

    srv.run(); // blocking call
    return 0;

}
    //srv.bind("function_name",[local_args](params){
        //function stuff
    //})