#include "socket.h"

Server::Server(const char * socketAddress)
{
    /* setUpSignalHandler(); */
    struct sockaddr_un server_addr;

    // setup socket address structure
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path,socketAddress,sizeof(server_addr.sun_path) - 1);

    // create socket
    server_ = socket(PF_UNIX,SOCK_STREAM,0);
    if (!server_) 
    {
        perror("socket");
        exit(-1);
    }

    // call bind to associate the socket with the UNIX file system
    if (bind(server_,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) 
    {
        perror("bind");
        exit(-1);
    }

    // convert the socket to listen for incoming connections
    if (listen(server_,SOMAXCONN) < 0) 
    {
        perror("listen");
        exit(-1);
    }
}

int Server::Accept()
{
    // setup client
    /* int client; */
    struct sockaddr_in client_addr;
    socklen_t clientlen = sizeof(client_addr);

    // accept clients
    client = accept(server_,(struct sockaddr *)&client_addr,&clientlen); 
    return client;
}


bool Server::Listen(request & request_)
{
    /* memset(request_.command, 0, sizeof(request_.command)); */
    cout << "command before " << request_.command<< endl;
    memset(request_.command, 0, sizeof(request_.command));
    cout << "command after " << request_.command<< endl;
    cout << "buf_" << request_.command << endl;
    char buf_[10];
    string request = "";
    bool command = true;
    int length = 0;
    // read until we get a newline
    /* while (request.find("\n") == string::npos) */ 

    // read command:
    int nread = recv(client,&request_.command,7,0);
    if (nread < 0) 
    {
        if (errno == EINTR)
        {
            // the socket call was interrupted -- try again
            /* continue; */
        }
        else
        {
            // an error occurred, so break out
            cout << "else " << nread << " " << errno <<  endl;
            return false;
        }
    } 
    else if (nread == 0)
    {
        // the socket is closed
        cout << "socket closed" << endl;
        return false;
    }
request_.command[6] = '\0';

    /* strcpy(request_.command,buf_); */
    /* /1* request_.command[6] = '\n'; *1/ */
    cout << "command " << request_.command<< endl;
    /* command = false; */



    /* nread = recv(client,&buf_,1,0); */
    // read argument:
    nread = recv(client,&buf_,10,0);
    if (nread < 0) 
    {
        if (errno == EINTR)
        {
            // the socket call was interrupted -- try again
            /* continue; */
        }
        else
        {
            // an error occurred, so break out
            cout << "else " << nread << " " << errno <<  endl;
            return false;
        }
    } 
    else if (nread == 0)
    {
        // the socket is closed
        cout << "socket closed" << endl;
        return false;
    }

    buf_[9] = ' ';
    cout << "argu buf_ : " << buf_ << endl;
    /* buf_ = buf_+1; */
    /* buf_ = buf_+1; */
    stringstream ss;
    cout << "argu buf_ : " << buf_ << endl;
    ss << buf_;
    ss >> request_.argument;
    cout << "arg to int " << request_.argument << endl;

    /*
    int i = 0;
    while (i != 2) 
    {
        ++i;
        if(command)
        {
            length = 6;
        }
        else
        {
            length = 10;
        }
        int nread = recv(client,&buf_,length,0);
        if (nread < 0) 
        {
            if (errno == EINTR)
            {
                // the socket call was interrupted -- try again
                continue;
            }
            else
            {
                // an error occurred, so break out
                cout << "else " << nread << " " << errno <<  endl;
                return false;
            }
        } 
        else if (nread == 0)
        {
            // the socket is closed
            cout << "socket closed" << endl;
            return false;
        }

        if (command)
        {
            strcpy(request_.command,buf_);
            cout << "command " << request_.command<< endl;
            command = false;
        }
        else
        {
            stringstream ss;
            cout << "argu buf_ : " << buf_ << endl;
            ss << buf_;
            ss >> request_.argument;
            cout << "arg " << request_.argument << endl;
        }
        // be sure to use append in case we have binary data
        request.append(buf_,nread);
    }

*/
    // a better server would cut off anything after the newline and
    // save it in a cache
    return true;
}

/* bool Server::Reply(int client, string response) */
bool Server::Reply(string response)
{
    // prepare to send response
    const char* ptr = response.c_str();
    int nleft = response.length();
    int nwritten;
    // loop to be sure it is all sent
    while (nleft) 
    {
        if ((nwritten = send(client, ptr, nleft, 0)) < 0) 
        {
            if (errno == EINTR) 
            {
                // the socket call was interrupted -- try again
                /* continue; */
            } 
            else 
            {
                // an error occurred, so break out
                perror("write");
                return false;
            }
        } 
        else if (nwritten == 0) 
        {
            // the socket is closed
            return false;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return true;
}
