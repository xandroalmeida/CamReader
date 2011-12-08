#include "MjpegCapture.h"
#include <iostream>
#include <fstream>
#include <string>



// http://www.daniweb.com/software-development/cpp/threads/313842

using namespace std;

static ofstream myfile;

static const int  REQ_WINSOCK_VER   = 2;
static const char HEAD_REQUEST_PART1[] =
{
    //Path to ip camera cgi file
    //"Get /videofeed HTTP/1.1\r\n"
    "GET /videofeed HTTP/1.1\r\n"
    "Host: 192.168.1.101\r\n"// Specify host name used
    "Accept: text/html;text/plain;"
};
//------------------------------------------------------------
static const char HEAD_REQUEST_PART2[] =
{
    "\r\n"							// End hostname header from part1
    "User-agent: Myserver\r\n"      // Specify user agent
    "Connection: close\r\n" 		// Close connection after response
    "\r\n"							// Empty line indicating end of request
};

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


MjpegCapture::MjpegCapture(const char* url, unsigned int port, const char* path):
    m_url(url), m_port(port), m_path(path), tempBufferMaxSize(256)
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
    if (!(pHostent = gethostbyname(m_url.c_str())))
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

bool MjpegCapture::RequestHeaders()
{

    cout << "Sending request... ";
    try
    {
        if (send(this->m_Socket, HEAD_REQUEST_PART1, sizeof(HEAD_REQUEST_PART1)-1, 0)==SOCKET_ERROR)
            throw HRException("failed to send data.");
        if (send(this->m_Socket, m_url.c_str(), lstrlen(m_url.c_str()), 0)==SOCKET_ERROR)
            throw HRException("failed to send data.");

        if (send(this->m_Socket, HEAD_REQUEST_PART2, sizeof(HEAD_REQUEST_PART2)-1, 0)==SOCKET_ERROR)
            throw HRException("failed to send data.");
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

    while (true) {
        c = ReadChar();
        if (c == EOF) {
            break;
        }

        ret.append(1, c);

        if (c == '\n') {
            break;
        }
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
                cout << "Looking up hostname " << m_url << "... ";
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

