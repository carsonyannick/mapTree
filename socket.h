#include <netinet/in.h>
#include <iostream> 
#include <string.h>
#include <sstream>
#include <sys/un.h>

using namespace std;

class Server
{
public:
    struct request
    {
        char command[7];
        int id;
        char data[30];
    };
    Server(const char * socketAddress); // :  socketAddress_(socketAddress);
    int Accept();
    bool Listen(request & request_);
    /* bool Reply(int client, string response); */
    bool Reply(string response);

private:
    int server_;
    int client;
};
