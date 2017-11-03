#include <iostream> 
#include <signal.h>
#include <unistd.h> // unlink()
#include <map>

#include "socket.h"

using namespace std;

const char* socket_name_ = "/tmp/cppMap-socket";
Server * initalize();
void setUpSignalHandler();
void onExit(int);

int main()
{
    Server * server = initalize();
    map<int,string> tree;

    while(1)
    {
        while(server->Accept() > 0)
        {
            Server::request request_;
            while(server->Listen(request_))
            {
                char * command = request_.command;
                int & id = request_.id;
                char * data = request_.data;

                if (strcmp("add   ",command) == 0)
                {
                     cout << "inside add if" << endl;
                     cout << "id: " << id << " data " << data << endl;
                     tree.insert(pair<int,string>(id, data));
                     server->Reply("Added!");
                }
                else if (strcmp(command,"search") == 0)
                {
                     cout << "inside search if" << endl;

                     string output;
                     try 
                     {
                         output = "found ";
                         output += tree.at(id);
                     } 
                     catch (const std::out_of_range& oor)
                     {
                         output = "not found";
                     }

                     server->Reply(output);
                     cout << output << endl;
                }
                else if (strcmp(command,"delete") == 0)
                {
                     cout << "inside delete if" << endl;
                     tree.erase(id);
                }
                else if (strcmp(command,"draw  ") == 0)
                {
                     cout << "inside draw if" << endl;
                     for(map<int,string>::iterator it=tree.begin(); it != tree.end(); ++it)
                     {
                         cout << it->first << " " << it->second << endl;
                     }
                }
                else 
                {
                 cerr << "invalid option"<< endl;
                 onExit(0);
                 exit (33);
                }
            }
        }
    }
}


Server* initalize() 
{
    setUpSignalHandler();
    Server * server = new Server(socket_name_);
    return server;
}

void setUpSignalHandler()
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = onExit;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
}

void onExit(int) 
{
    unlink(socket_name_);
    exit(2);
}
