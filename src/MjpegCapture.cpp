#include "MjpegCapture.h"
#include <iostream>
#include <fstream>
#include <string>

#include <boost/algorithm/string/predicate.hpp>



// http://www.daniweb.com/software-development/cpp/threads/313842

using namespace std;

static ofstream myfile;

static const int  REQ_WINSOCK_VER   = 2;

//------------------------------------------------------------

class HRException
{
public:
    HRException() :
        m_pMessage("") {}
    virtual ~HRException() {}
    HRException(const char *pMessage) :
        m_pMessage(pMessage) {}
    const char * what()
    {
        return m_pMessage;
    }
private:
    const char *m_pMessage;
};


MjpegCapture::MjpegCapture(const char* host, unsigned int port, const char* path):
    m_host(host), m_port(port), m_path(path), tempBufferMaxSize(256)
{
    this->tempBufferPos = tempBufferMaxSize;
    this->tempBufferSize = this->tempBufferPos;

    this->m_Socket = INVALID_SOCKET;
    this->m_tempBuffer = (char*)malloc(tempBufferMaxSize);
}

MjpegCapture::~MjpegCapture()
{
    Close();
    free(this->m_tempBuffer);
}

unsigned long MjpegCapture::FindHostIP()
{
    HOSTENT *pHostent;

    // Get hostent structure for hostname:
    if (!(pHostent = gethostbyname(m_host.c_str())))
        throw HRException("could not resolve hostname.");

    // Extract primary IP address from hostent structure:
    if (pHostent->h_addr_list && pHostent->h_addr_list[0])
        return *reinterpret_cast<unsigned long*>(pHostent->h_addr_list[0]);

    return 0;
}

void MjpegCapture::FillSockAddr(sockaddr_in *pSockAddr)
{
    // Set family, port and find IP
    pSockAddr->sin_family = AF_INET;
    pSockAddr->sin_port = htons(m_port);
    pSockAddr->sin_addr.S_un.S_addr = FindHostIP();
}

void MjpegCapture::sendString(const char* str)
{
    if (send(this->m_Socket, str, lstrlen(str), 0)==SOCKET_ERROR)
        throw HRException("failed to send data.");

}

void MjpegCapture::sendString(std::string &str)
{
    sendString(str.c_str());
}

bool MjpegCapture::SendRequest()
{

    cout << "Sending request... ";
    try
    {
        sendString("GET ");
        sendString("/");
        sendString(" HTTP/1.1\r\nHost: ");
        sendString(this->m_host);
        sendString("\r\nAccept: text/html;text/plain;");
        sendString(m_host);
        sendString("\r\nUser-agent: Myserver\r\nConnection: close\r\n\r\n");

        cout << "request sent.\n";
    }
    catch (HRException e)
    {
        cout << e.what() << endl;
        return false;
    }
    return true;
}


void MjpegCapture::Close()
{
    if (this->m_Socket != INVALID_SOCKET)
    {
        closesocket(this->m_Socket);
    }

}

char MjpegCapture::ReadChar()
{

    if (this->tempBufferPos == EOF)
        return EOF;

    if (this->tempBufferPos == this->tempBufferSize)
    {
        this->tempBufferSize = recv(this->m_Socket, this->m_tempBuffer, sizeof(this->tempBufferSize), 0);
        if (this->tempBufferSize == 0)
            this->tempBufferPos = EOF;

        this->tempBufferPos = 0;
    }

    return this->m_tempBuffer[this->tempBufferPos++];
}

string MjpegCapture::ReadLine()
{
    string ret;
    char c;

    while (true)
    {
        c = ReadChar();
        if (c == EOF)
        {
            break;
        }

        if (c == '\n')
        {
            break;
        }

        ret.append(1, c);
    }

    if (boost::algorithm::ends_with(ret, "\r"))
    {
        ret = ret.substr(0, ret.size()-1);
    }

    return ret;
}

bool MjpegCapture::Open()
{
    WSADATA wsaData;
    bool hRet = false;
    if (WSAStartup(MAKEWORD(REQ_WINSOCK_VER,0), &wsaData)==0)
    {
        if (LOBYTE(wsaData.wVersion) >= REQ_WINSOCK_VER)
        {

            cout << "initialized.\n";
            sockaddr_in	sockAddr = {0};

            try
            {
                cout << "Looking up hostname " << m_host << "... ";
                FillSockAddr(&sockAddr);
                cout << "found.\n";

                cout << "Creating socket... ";
                if ((this->m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
                    throw HRException("could not create socket.");
                cout << "created.\n";

                cout << "Attempting to connect to " << inet_ntoa(sockAddr.sin_addr)
                     << ":" << m_port <<  "... ";

                if (connect(this->m_Socket, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr))!=0)
                    throw HRException("could not connect.");
                cout << "connected.\n";

                hRet = true;

            }
            catch (HRException e)
            {
                cout << e.what() << endl;
                Close();
            }
        }
        else
        {
            cerr << "LOBYTE failed!\n";
        }
    }
    else
    {
        cerr << "WSAStartup failed!\n";

    }

    return  hRet;
}

