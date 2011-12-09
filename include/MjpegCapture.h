#ifndef MJPEGCAPTURE_H
#define MJPEGCAPTURE_H

#include <winsock2.h>
#include <windows.h>

#include <string>

class MjpegCapture
{
public:
    MjpegCapture(const char* host, unsigned int port, const char* path);
    virtual ~MjpegCapture();

    bool SendRequest();
    std::string ReadLine();
    char ReadChar();

    bool Open();
    void Close();

protected:
private:
    unsigned long FindHostIP();
    void FillSockAddr(sockaddr_in *pSockAddr);
    void sendString(const char* str);
    void sendString(std::string &str);

    char* m_tempBuffer;
    std::string m_host;
    unsigned int m_port;
    std::string m_path;
    const  int tempBufferMaxSize;
    int tempBufferSize;
    SOCKET m_Socket;
    int tempBufferPos;
};

#endif // MJPEGCAPTURE_H
