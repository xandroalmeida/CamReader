#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <ctype.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

#define winname "CamReaderTest"

void help()
{
	cout << "Este software tem apenas o proposito de demonstracao" << endl;
}

int alpha = 3;
int blockSize  = 50;
int thresholdC = 100;

int main( int argc, char** argv )
{
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

    namedWindow( winname, 0 );
    createTrackbar( "Alpha", winname, &alpha, 15, 0 );
    createTrackbar("blockSize Threshold", winname, &blockSize, 200);
    createTrackbar("thresholdC Threshold", winname, &thresholdC, 200);

    Mat result, gray,threshold;
    for(;;)
    {
        Mat frame;
        cap >> frame;
        if( frame.empty() )
            break;

        int ksize = (alpha*5)|1;
        convertScaleAbs(frame, result, (alpha+1)*0.25);
        cvtColor(result, gray, CV_BGR2GRAY);
        adaptiveThreshold(gray, threshold, 255,ADAPTIVE_THRESH_GAUSSIAN_C , CV_THRESH_BINARY, (blockSize % 2 == 0 ? blockSize +1 : blockSize),thresholdC-100);

        imshow(winname, threshold);

        int c = waitKey(30);
        if( c == 'q' || c == 'Q' || (c & 255) == 27 )
            break;
    }

    return 0;
}
