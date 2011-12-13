#include "NumericOCR.h"

using namespace cv;

NumericOCR::NumericOCR(Mat img)
{
    this->image = img.clone();
    vector<vector<Point> > contours0;

    findContours(this->image, contours0, this->hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_L1);
    this->contours.resize(contours0.size());
    for( size_t k = 0; k < contours0.size(); k++ )
        approxPolyDP(Mat(contours0[k]), this->contours[k], 3, true);
}

NumericOCR::~NumericOCR()
{
}

Mat NumericOCR::drawContours()
{
    Mat cnt_img = Mat::zeros(image.rows, image.cols, CV_8UC3);
    cv::drawContours( cnt_img, this->contours, -1, Scalar(128,255,255),1, CV_AA, this->hierarchy, 99 );
    return cnt_img;
}


