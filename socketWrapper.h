#ifndef HTTPSERVER_SOCKETWRAPPER_H_
#define HTTPSERVER_SOCKETWRAPPER_H_

#include <string>
#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>
#include <unordered_map>
#include <sstream> 

class SocketWrapper{
    public:
        SocketWrapper();
        SocketWrapper(int socket, addrinfo info, bool bound, bool connected);
        ~SocketWrapper();

        void bind(int port);
        void connect(std::string addr, int port);
        void listen();
        std::shared_ptr<SocketWrapper> accept();
        void send(const char *data, unsigned int length, int flags);
        int receive(char* msg, int len, int flags);
        void close();

    private:
        void setInfo(std::string adress, int port);
        void openSocket(addrinfo *info);
        addrinfo *mInfo;
        WSADATA mWsaData;
        int mSetup = 1;
        int mSock = -1;
        bool mSockCreated = false;
        bool mBound = false;
        bool mConnected = false;
        bool mClosed = false;
};

#endif