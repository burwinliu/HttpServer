#include "socketWrapper.h"

//todo buffer management.
//todo Exception handling

// Constructs/Destructors
SocketWrapper::SocketWrapper(int pPortNumber){
    int res;
    mPortNumber = std::to_string(pPortNumber);
    res = WSAStartup(MAKEWORD(2,2), &mWsaData);

    if (res != 0) {
        printf("WSAStartup failed with error: %d\n", res);
        mSetupSuccess = 0;
        return;
    }
    ZeroMemory(&mHints, sizeof(mHints));
    mHints.ai_family = AF_INET;
    mHints.ai_socktype = SOCK_STREAM;
    mHints.ai_protocol = IPPROTO_TCP;
    mHints.ai_flags = AI_PASSIVE;
    mSetupSuccess = 1;
}

SocketWrapper::~SocketWrapper(){
    // cleanup
    // No longer need server socket
    if (mListenSocket != INVALID_SOCKET){
        closesocket(mListenSocket);
    }
    WSACleanup();
}


// Methods and functions
void SocketWrapper::Listen(){
    
    int res;

    struct addrinfo *result = NULL;
    SOCKET ListenSocket = INVALID_SOCKET;
    int iResult;



    // Resolve the server address and port
    iResult = getaddrinfo(NULL, mPortNumber.c_str(), &mHints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        mListenSocket = INVALID_SOCKET;
        return;
    }

    // Create a SOCKET for connecting to server
    mListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (mListenSocket == INVALID_SOCKET) {
        printf("socket in BindSocket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        mListenSocket = INVALID_SOCKET;
        return;
    }

    // Setup the TCP listening socket
    iResult = bind( mListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind in BindSocket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(mListenSocket);
        mListenSocket = INVALID_SOCKET;
        return;
    }

    freeaddrinfo(result);

    iResult = listen(mListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen in BindSocket in BindSocket failed with error: %d\n", WSAGetLastError());
        closesocket(mListenSocket);
        mListenSocket = INVALID_SOCKET;
        return;
    }
    mSocketBound = 1;
}
SOCKET SocketWrapper::Accept(sockaddr *addr, int *addrlen){
    SOCKET clientSocket = INVALID_SOCKET;

    clientSocket = accept(mListenSocket, addr, addrlen);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "accept in Accept failed with error: " << WSAGetLastError() << std::endl;
        std::cout << "mListenSocket is " << mListenSocket << std::endl;
        closesocket(mListenSocket);
    }
    return clientSocket;
}

int SocketWrapper::Send(std::string message, SOCKET clientSocket){
    printf("SENDING %s WITH %d\n", message.c_str(),message.length()+1);
    int iSendResult = send( clientSocket, message.c_str(), message.length()+1, 0 );
    if (iSendResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(clientSocket);
    }
    return iSendResult;
}

std::string SocketWrapper::Receive(SOCKET clientSocket){
    std::string buf;
    char recvbuf[16384];
    int iSendResult;
    int recvbuflen = 16384;
    int iResult;
        
    iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
    
    if (iResult > 0) {
        // KEEP APPENDING TO RESULT STR AND THEN RETURN  (TEST AND CHECK)
        if (iResult < recvbuflen){
            recvbuf[iResult] = '\0';
            buf = recvbuf; // char*
            printf("# RECIEVED %s----\n", recvbuf);
            throw SocketError(1);
            return buf;
        }
        else{
            throw SocketError(1);
        }
        
    }
    else if (iResult == 0){
        printf("Client Exited\n");
        return "";
    }
    else {
        printf("recv in Receive failed with error: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        throw SocketError(2);
    }

    

}

void SocketWrapper::Cleanup(SOCKET socket){
    int result = shutdown(socket, SD_SEND);
    if (result == SOCKET_ERROR) {
        printf("shutdown in Cleanup failed with error: %d\n", WSAGetLastError());
        closesocket(socket);
        return;
    }
}

int SocketWrapper::getSetupSuccess(){
    return mSetupSuccess;
}

SocketError::SocketError(int err){
    mErrorCode = err;
}

int SocketError::ErrorCode(){
    return mErrorCode;
}

