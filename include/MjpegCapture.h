#ifndef MJPEGCAPTURE_H
#define MJPEGCAPTURE_H

#include <winsock2.h>
#include <windows.h>

#include <string>

class MjpegCapture
{
public:
    MjpegCapture(const char* url, unsigned int port, const char* path);
    virtual ~MjpegCapture();

    bool RequestHeaders();
    std::string ReadLine();
    char ReadChar();

    bool Open();
    void Close();

protected:
private:
    unsigned long FindHostIP();
    void FillSockAddr(sockaddr_in *pSockAddr);

    char* m_tempBuffer;
    std::string m_url;
    unsigned int m_port;
    std::string m_path;
    const  int tempBufferMaxSize;
    int tempBufferSize;
    SOCKET m_Socket;
    int tempBufferPos;
};

#endif // MJPEGCAPTURE_H
