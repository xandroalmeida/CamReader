#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "MjpegCapture.h"

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
    MjpegCapture cap("192.168.1.101", 8080, "/videofeed");
    cap.Open();
    cap.RequestHeaders();

    int line = 1;
    cout << line++ << ": " << cap.ReadLine();
    cout << line++ << ": " << cap.ReadLine();
    cout << line++ << ": " << cap.ReadLine();
    cout << line++ << ": " << cap.ReadLine();
    cout << line++ << ": " << cap.ReadLine();
    cout << line++ << ": " << cap.ReadLine();
    cout << line++ << ": " << cap.ReadLine();
    cout << line++ << ": " << cap.ReadLine();
    cout << line++ << ": " << cap.ReadLine();
    cout << line++ << ": " << cap.ReadLine();
    cout << line++ << ": " << cap.ReadLine();
    cout << line++ << ": " << cap.ReadLine();
    cout << line++ << ": " << cap.ReadLine();
    cout << line++ << ": " << cap.ReadLine();


    cap.Close();

    readParam();
#ifdef false
    VideoCapture cap;

    help();

    cap.open("http://192.168.1.101:8087/mjpeg");
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

    int threshold1  = 5;
    int threshold2 = 5;
    int d = 5;
    int sigmaColor = 0;
    int sigmaSpace = 0;
    Mat result;
    namedWindow("Controle", 0);
    createTrackbar("threshold1", "Controle", &threshold1, 30);
    createTrackbar("threshold2", "Controle", &threshold2, 20);
    createTrackbar("d", "Controle", &d, 20);
    createTrackbar("sigmaColor", "Controle", &sigmaColor, 100);
    createTrackbar("sigmaSpace", "Controle", &sigmaSpace, 100);

    Mat bilateral;
    for(;;)
    {
        Mat frame;
        cap >> frame;
        if( frame.empty() )
            break;

        imshow("Original", frame);

        bilateralFilter(frame, bilateral, d, sigmaColor/10, sigmaSpace/10);
        cvtColor(bilateral, frame, CV_RGB2GRAY);
        imshow("bilateral", frame);

        Canny(frame, frame, threshold1 + 20, threshold1);

        imshow("Saida", frame);

        int c = waitKey(30);
        if( c == 'q' || c == 'Q' || (c & 255) == 27 )
            break;

    }
#endif
    return 0;
}
