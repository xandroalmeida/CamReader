#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

#define winname "CamReaderTest"
int alpha = 3;
int blockSize  = 50;
int thresholdC = 10;
int ksize = 15;

void readParam()
{
    ifstream fin("data.bin", ios::in | ios::binary);
    if (fin) {
        fin.read((char*)&alpha, sizeof(alpha));
        fin.read((char*)&blockSize, sizeof(blockSize));
        fin.read((char*)&thresholdC, sizeof(thresholdC));
        fin.read((char*)&ksize, sizeof(ksize));
        fin.close();
    }
}

void onChangeParam(int, void*)
{
    ofstream fout;
    fout.open("data.bin", ios::out | ios::binary);
    fout.write((char *)(&alpha), sizeof(alpha));
    fout.write((char *)(&blockSize), sizeof(blockSize));
    fout.write((char *)(&thresholdC), sizeof(thresholdC));
    fout.write((char *)(&ksize), sizeof(ksize));

    fout.close();

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

    if( argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
        cap.open(argc == 2 ? argv[1][0] - '0' : 0);
    else if( argc >= 2 )
    {
        cap.open(argv[1]);
        if( cap.isOpened() )
            cout << "Video " << argv[1] <<
                ": width=" << cap.get(CV_CAP_PROP_FRAME_WIDTH) <<
                ", height=" << cap.get(CV_CAP_PROP_FRAME_HEIGHT) <<
                ", nframes=" << cap.get(CV_CAP_PROP_FRAME_COUNT) << endl;
        if( argc > 2 && isdigit(argv[2][0]) )
        {
            int pos;
            sscanf(argv[2], "%d", &pos);
            cout << "seeking to frame #" << pos << endl;
            cap.set(CV_CAP_PROP_POS_FRAMES, pos);
        }
    }

    if( !cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return -1;
    }

    namedWindow( "Controls", 0 );
    createTrackbar( "Alpha", "Controls", &alpha, 15, onChangeParam );
    createTrackbar("blockSize Threshold", "Controls", &blockSize, 200, onChangeParam);
    createTrackbar("thresholdC Threshold", "Controls", &thresholdC, 50, onChangeParam);
    createTrackbar("ksize", "Controls", &ksize, 50, onChangeParam);

    Mat result, gray,threshold, smooth;
    for(;;)
    {
        Mat frame;
        cap >> frame;
        if( frame.empty() )
            break;

        convertScaleAbs(frame, result, (alpha+1)*0.25);
        medianBlur(result, smooth, ksize|1);

        cvtColor(smooth, gray, CV_BGR2GRAY);
        adaptiveThreshold(gray, threshold, 255,ADAPTIVE_THRESH_GAUSSIAN_C , CV_THRESH_BINARY, (blockSize+2)|1 ,thresholdC);

        imshow(winname, threshold);
        imshow("Original", frame);
        imshow("Scaled", result);
        imshow("Gray", gray);
        imshow("Smooth", smooth);

        int c = waitKey(30);
        if( c == 'q' || c == 'Q' || (c & 255) == 27 )
            break;
    }

    return 0;
}
