#ifndef MJPEGCAPTURE_H
#define MJPEGCAPTURE_H

#include <boost/asio.hpp>

#include <string>

class MjpegCapture
{
public:
    MjpegCapture(const char* host, unsigned int port, const char* path);
    virtual ~MjpegCapture();

    bool SendRequest();
    std::string ReadLine();

    bool Open();
    void Close();

protected:
private:
    void sendString(const char* str);
    void sendString(std::string &str);

    std::string m_host;
    unsigned int m_port;
    std::string m_path;

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver;
    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf buffer;

};

#endif // MJPEGCAPTURE_H
