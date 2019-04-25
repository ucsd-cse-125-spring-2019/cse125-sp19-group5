#include "rpc/client.h"

struct custom_type {
    int x;
    double y;
    std::string str;
    MSGPACK_DEFINE_MAP(x, y, str) // or MSGPACK_DEFINE_MAP
};

int main() {
    rpc::client client_conn("127.0.0.1", 8080);
    std::string msg = "hello";
    custom_type c;
    c.x = 2; 
    c.y = 7.0; 
    c.str = msg;        

    // client initiates async connection upon creation

    // call blocks until:
    // - connection is established
    // - result is read
    client_conn.call("cool_function", c);

    return 0;
}