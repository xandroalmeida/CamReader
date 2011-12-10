#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "MjpegCapture.h"

#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
//#include <ofstream>

#include <boost/filesystem.hpp>

#include "preprocessing.h"

using namespace boost::filesystem;

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

#define TESTE

void buildTrainData()
{
    int size = 16;
    IplImage* src_image;
    IplImage prs_image;
    path basePath("C:\\Documents and Settings\\saoadalm\\My Documents\\Downloads\\OCR");
    try
    {
        if (exists(basePath) && is_directory(basePath))      // is p a directory?
        {
            cout << basePath << " is a directory containing:\n";

            directory_iterator numDirIterator (basePath);
            directory_iterator end;

            ofstream fout;
            path dataPath("ocr_train.csv");
            dataPath = basePath.parent_path()/dataPath;

            fout.open(dataPath.string().c_str(), ios::out);

            while (numDirIterator != end)
            {
                directory_entry numberDirEntry = *numDirIterator++;
                directory_iterator numFileIteraator (numberDirEntry);
                string number = numberDirEntry.path().filename().string();
                while (numFileIteraator != end)
                {
                    directory_entry numberFileEntry = *numFileIteraator++;
                    string fileName = numberFileEntry.path().string();

                    src_image = cvLoadImage(fileName.c_str(),0);
                    prs_image = preprocessing(src_image, size, size);

                    for (int i = 0; i < (size*size); i++) {
                        fout << (prs_image.imageData[i] != -1 ? "1" : "0") << ",";
                        //cout << (prs_image.imageData[i] != -1 ? "X" : " ");
                        //if (((i+1)%size) == 0)
                        //    cout << endl;
                    }
                    //string a;
                    //cin >> a;
                    fout << "0." << number << endl;
                    //cout << endl;
                }

            }
            fout.close();

            //cout << dir << endl;
        }
        else
            cout << basePath << " does not exist or not is a directory\n";
    }

    catch (const filesystem_error& ex)
    {
        cout << ex.what() << '\n';
    }
}

int main( int argc, char** argv )
{
#ifdef TESTE

    buildTrainData();
    return 0;


    MjpegCapture cap("localhost", "8080", "/HP.PhoenixOMC");
    cap.Open();
    cap.SendRequest();

    int line = 1;
    cout << line++ << ": " << cap.ReadLine() << endl;
    cout << line++ << ": " << cap.ReadLine() << endl;
    cout << line++ << ": " << cap.ReadLine() << endl;
    cout << line++ << ": " << cap.ReadLine() << endl;
    cout << line++ << ": " << cap.ReadLine() << endl;
    cout << line++ << ": " << cap.ReadLine() << endl;
    cout << line++ << ": " << cap.ReadLine() << endl;
    cout << line++ << ": " << cap.ReadLine() << endl;
    cout << line++ << ": " << cap.ReadLine() << endl;
    cout << line++ << ": " << cap.ReadLine() << endl;
    cout << line++ << ": " << cap.ReadLine() << endl;
    cout << line++ << ": " << cap.ReadLine() << endl;
    cout << line++ << ": " << cap.ReadLine() << endl;
    cout << line++ << ": " << cap.ReadLine() << endl;


    cap.Close();

#else
    readParam();
    VideoCapture cap;

    help();

    cap.open(0);
    if( cap.isOpened() )
    {
        cout << "Video " << 0 <<
             ": width=" << cap.get(CV_CAP_PROP_FRAME_WIDTH) <<
             ", height=" << cap.get(CV_CAP_PROP_FRAME_HEIGHT) <<
             ", nframes=" << cap.get(CV_CAP_PROP_FRAME_COUNT) <<
             ", fps=" << cap.get(CV_CAP_PROP_FPS) <<
             ", format=" << cap.get(CV_CAP_PROP_FORMAT) <<
             ", mode=" << cap.get(CV_CAP_PROP_MODE) <<
             ", ratio=" << cap.get(CV_CAP_PROP_POS_AVI_RATIO) <<

             endl;
    }
    else
    {
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

        //imshow("Original", frame);

        bilateralFilter(frame, bilateral, d, sigmaColor/10, sigmaSpace/10);
        cvtColor(bilateral, frame, CV_RGB2GRAY);
        imshow("bilateral", frame);

//       Canny(frame, frame, threshold1 + 20, threshold1);

        //     imshow("Saida", frame);

        int c = waitKey(30);
        if( c == 'q' || c == 'Q' || (c & 255) == 27 )
            break;

    }
#endif
    return 0;
}
