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
    memset(request_.command, 0, sizeof(request_.command));
    char buf_[7];
    string request = "";
    bool command = true;
    int length = 0;
    // read until we get a newline
    /* while (request.find("\n") == string::npos) */ 

// 1) read command:
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

    cout << "Command " << request_.command<< endl;

// 2) read id:
    nread = recv(client,&buf_,7,0);
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

    buf_[6] = ' ';
    cout << "buf_ before conversion to int : " << buf_ << endl;
    stringstream ss;
    ss << buf_;
    ss >> request_.id;
    cout << "buf_ after conversion to int " << request_.id << endl;

// 3) length of data
    char len[3];
    int dataLength;
    nread = recv(client,&len,2,0);
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

    ss.str("");
    ss << len;
    ss >> dataLength;

    cout << "dataLength: " << dataLength << endl;

// 4) read data:
    nread = recv(client,&request_.data,dataLength,0);
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

    request_.data[dataLength] = '\0';
    cout << "Data: " << request_.data << endl;

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
