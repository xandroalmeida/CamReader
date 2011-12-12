#ifndef NUMERICOCR_H
#define NUMERICOCR_H

#include "opencv2/imgproc/imgproc.hpp"
#include <vector>


class NumericOCR
{
public:
    NumericOCR(cv::Mat image);
    virtual ~NumericOCR();
    cv::Mat drawContours();

protected:
private:
    cv::Mat image;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

};

#endif // NUMERICOCR_H
