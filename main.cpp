#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

void readParam()
{
/*
    ifstream fin("data.bin", ios::in | ios::binary);
    if (fin) {
        fin.read((char*)&value, sizeof(value));
        fin.close();
    }
*/
}

void onChangeParam(int, void*)
{
    /*
    ofstream fout;
    fout.open("data.bin", ios::out | ios::binary);
    fout.write((char *)(&value), sizeof(value));
    fout.close();
*/
}

void help()
{
	cout << "Este software tem apenas o proposito de demonstracao" << endl;
}

int main( int argc, char** argv )
{
    readParam();
    VideoCapture cap;

    help();

    cap.open(0);
    if( cap.isOpened() ) {
        cout << "Video " << 0 <<
                ": width=" << cap.get(CV_CAP_PROP_FRAME_WIDTH) <<
                ", height=" << cap.get(CV_CAP_PROP_FRAME_HEIGHT) <<
                ", nframes=" << cap.get(CV_CAP_PROP_FRAME_COUNT) <<
                ", fps=" << cap.get(CV_CAP_PROP_FPS) <<
                ", format=" << cap.get(CV_CAP_PROP_FORMAT) <<
                ", mode=" << cap.get(CV_CAP_PROP_MODE) <<
                ", ratio=" << cap.get(CV_CAP_PROP_POS_AVI_RATIO) <<

                endl;
    } else {
        cout << "Could not initialize capturing...\n";
        return -1;
    }

    Mat result;
    for(;;)
    {
        Mat frame;
        cap >> frame;
        if( frame.empty() )
            break;

        cvtColor(frame, frame, CV_BGR2GRAY);
        medianBlur(frame, frame, 3);

        Scalar mean;
        Scalar stddev;

        meanStdDev(frame, mean, stddev);
        threshold(frame, frame, (int)mean[0]-7*(int)(stddev[0]/8), 255, CV_THRESH_BINARY_INV);

        imshow("Original", frame);

        int c = waitKey(30);
        if( c == 'q' || c == 'Q' || (c & 255) == 27 )
            break;

    }

    return 0;
}
