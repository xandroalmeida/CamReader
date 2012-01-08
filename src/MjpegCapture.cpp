#include "MjpegCapture.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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


MjpegCapture::MjpegCapture(const char* host, std::string port, const char* path):
    m_host(host), m_port(port), m_path(path), io_service(), resolver(io_service), socket(io_service)
{

}

MjpegCapture::~MjpegCapture()
{
    Close();
}


void MjpegCapture::sendString(const char* str)
{
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << str;
    // Send the request.
    boost::asio::write(socket, request);

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
        sendString(m_path);
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
    socket.close();
}

string MjpegCapture::ReadUntil(const char* str)
{
    string ret;
    boost::asio::read_until(socket, buffer, str);

    while (!boost::algorithm::ends_with(ret, str))
    {
        ret.append(1, (char)buffer.sbumpc());
    }

    return ret;
}

string MjpegCapture::ReadUntil(string &str)
{
    return ReadUntil(str.c_str());
}

string MjpegCapture::ReadLine()
{
    boost::asio::read_until(socket, buffer, "\r\n");
    std::istream is(&buffer);
    std::string line;
    std::getline(is, line);
    return line;
}

MjpegCapture& MjpegCapture::operator >> (cv::Mat& image)
{
    char* buffer;
    string sbuffer;
    sbuffer = GetFrame();
    while (sbuffer.size() < 1000)
    {
        sbuffer = GetFrame();
    }

    buffer = new char[sbuffer.length()];
    memcpy(buffer, sbuffer.data(), sbuffer.length());
    cv::Mat mat = cv::imdecode(cv::Mat(1, sbuffer.length(), CV_8UC1, buffer), CV_LOAD_IMAGE_UNCHANGED);

    delete buffer;
    image = mat;
    return *this;
}

string MjpegCapture::GetFrame()
{
    string frame = ReadUntil(boundary);
    int iend = frame.find(boundary);
    frame = frame.substr(0, iend);
    ReadUntil("\r\n\r\n");
    return frame;
}

bool MjpegCapture::Open()
{
    bool hRet = false;
    boost::asio::ip::tcp::resolver::query query(m_host, m_port);
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::connect(socket, endpoint_iterator);
    SendRequest();

    string response = ReadLine();
    cout << "response : " << response << endl;

    response = ReadUntil(";boundary=");
    boundary = ReadLine();

    hRet = true;
    return  hRet;
}
