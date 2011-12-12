#include "NumericOCR.h"

using namespace cv;

NumericOCR::NumericOCR(Mat img)
{
    this->image = img.clone();
    vector<vector<Point> > contours0;

    findContours(this->image, contours0, this->hierarchy, CV_RETR_TREE, CHAIN_APPROX_SIMPLE);
    this->contours.resize(contours0.size());
    for( size_t k = 0; k < contours0.size(); k++ )
        approxPolyDP(Mat(contours0[k]), this->contours[k], 3, true);
}

NumericOCR::~NumericOCR()
{
}

Mat NumericOCR::drawContours()
{
    Mat cnt_img = Mat::zeros(480, 640, CV_8UC3);
    cv::drawContours( cnt_img, this->contours, 0, Scalar(128,255,255),1, CV_AA, this->hierarchy, 99 );
    return cnt_img;
}


