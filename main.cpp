#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "MjpegCapture.h"
#include "NumericOCR.h"

#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

//#include <ofstream>

#include <boost/filesystem.hpp>

#include <preprocessing.h>

#define NEXT_CONTOUR        0
#define PREVIOUS_CONTOUR    1
#define CHILD_CONTOUR       2
#define PARENT_CONTOUR      3


using namespace boost::filesystem;

using namespace cv;
using namespace std;
int blur_ksize = 1;
int threshold_thresh = 1;
int roi_size = 10;



void readParam()
{
    ifstream fin("data.bin", ios::in | ios::binary);
    if (fin)
    {
        fin.read((char*)&blur_ksize, sizeof(blur_ksize));
        fin.read((char*)&threshold_thresh, sizeof(threshold_thresh));
        fin.read((char*)&roi_size, sizeof(roi_size));

        fin.close();
    }
}


void onChangeParam(int, void*)
{
    ofstream fout;
    fout.open("data.bin", ios::out | ios::binary);
    fout.write((char *)(&blur_ksize), sizeof(blur_ksize));
    fout.write((char *)(&threshold_thresh), sizeof(threshold_thresh));
    fout.write((char *)(&roi_size), sizeof(roi_size));

    fout.close();
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
    path basePath("C:\\Users\\Alexandro\\Downloads\\basicOCR\\OCR");
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

                    for (int i = 0; i < (size*size); i++)
                    {
                        fout << (prs_image.imageData[i] != -1 ? "1" : "0") << ";";
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
    help();
    readParam();

    namedWindow("Controles",1);
    namedWindow("Original", 1);
    namedWindow("Contornos", 1);
    namedWindow("Bordas", 1);

    createTrackbar("blur_ksize", "Controles", &blur_ksize, 30, onChangeParam);
    createTrackbar("threshold_thresh", "Controles", &threshold_thresh, 200, onChangeParam);
    createTrackbar("roi_size", "Controles", &roi_size, 400, onChangeParam);


    //MjpegCapture cap("192.168.1.100", "8080", "/videofeed");
    //cap.Open();

    bool updateImage =true;
    VideoCapture cap;
    cap.open(0);
    Mat thImg;
    Mat img;
    while (true)
    {
        //Mat original = imread("identification-03-large.PNG");
        Mat original;
        cap >> original;

        if (roi_size < 5) roi_size = 5;
        if (roi_size > min(original.cols, original.rows)) roi_size = min(original.cols, original.rows);

        Point p1((original.cols / 2) - (roi_size/2), (original.rows / 2) - (roi_size/3));
        Point p2((original.cols / 2) + (roi_size/2), (original.rows / 2) + (roi_size/3));

        Rect crop(p1, p2);
        rectangle(original, p1, p2, Scalar(255,0,0));
        imshow("Original", original);

        Mat img(original);

        Mat borders(original);

        blur(img, img, Size(blur_ksize+1, blur_ksize+1));
        cvtColor(img, img, CV_BGR2GRAY);
        threshold(img, thImg,threshold_thresh,255,THRESH_BINARY) ;

        Canny(img, borders, 30,40);
        NumericOCR ocr(thImg);
        if (updateImage)
        {
            imshow("threshold", thImg);
            imshow("Contornos", ocr.drawContours());
            imshow("Bordas", borders);

            //updateImage = false;
        }


        int c = waitKey(30);
        if( c == 'q' || c == 'Q' || (c & 255) == 27 )
            break;

    }

    //cap.Close();

    return 0;
}
