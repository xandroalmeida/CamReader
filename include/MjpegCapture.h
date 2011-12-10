#ifndef MJPEGCAPTURE_H
#define MJPEGCAPTURE_H
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <boost/asio.hpp>

#include <string>

class MjpegCapture
{
public:
    MjpegCapture(const char* host, std::string port, const char* path);
    virtual ~MjpegCapture();
    bool Open();
    void Close();
    virtual MjpegCapture& operator >> (cv::Mat& image);

protected:
private:
    bool SendRequest();
    std::string ReadUntil(std::string &str);
    std::string ReadUntil(const char *str);
    std::string ReadLine();
    void sendString(const char* str);
    void sendString(std::string &str);
    std::string GetFrame();

    std::string m_host;
    std::string m_port;
    std::string m_path;
    std::string boundary;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver;
    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf buffer;

};

#endif // MJPEGCAPTURE_H
