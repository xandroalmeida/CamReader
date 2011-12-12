#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "MjpegCapture.h"

#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

//#include <ofstream>

#include <boost/filesystem.hpp>

#include <preprocessing.h>

using namespace boost::filesystem;

using namespace cv;
using namespace std;
int blur_ksize = 1;
int threshold_thresh = 1;


void readParam()
{
    ifstream fin("data.bin", ios::in | ios::binary);
    if (fin)
    {
        fin.read((char*)&blur_ksize, sizeof(blur_ksize));
        fin.read((char*)&threshold_thresh, sizeof(threshold_thresh));

        fin.close();
    }
}


void onChangeParam(int, void*)
{
    ofstream fout;
    fout.open("data.bin", ios::out | ios::binary);
    fout.write((char *)(&blur_ksize), sizeof(blur_ksize));
    fout.write((char *)(&threshold_thresh), sizeof(threshold_thresh));

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

    //buildTrainData();
    //return 0;

    namedWindow("Controles");

    createTrackbar("blur_ksize", "Controles", &blur_ksize, 30, onChangeParam);
    createTrackbar("threshold_thresh", "Controles", &threshold_thresh, 200, onChangeParam);


    MjpegCapture cap("192.168.1.100", "8080", "/videofeed");
    cap.Open();

    int niters = 3;
    while (true)
    {
        Mat frame;
        cap >> frame;

        if (!frame.empty())
        {
            cvtColor(frame, frame, CV_RGB2GRAY);
            blur(frame, frame, Size(blur_ksize+1,blur_ksize+1));

            threshold(frame, frame, threshold_thresh, 255, THRESH_BINARY_INV);

            vector<vector<Point> > contours;
            vector<Vec4i> hierarchy;
            Mat temp = frame.clone();

            findContours( temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

            int idx = 0, largestComp = 0;
            double maxArea = 0;
            if( contours.size() > 0 )
            {
                cout << "Contornos: " << contours.size() << endl;
                for( ; idx >= 0; idx = hierarchy[idx][0] )
                {
                    const vector<Point>& c = contours[idx];
                    double area = fabs(contourArea(Mat(c)));
                    if( area > maxArea )
                    {
                        maxArea = area;
                        largestComp = idx;
                    }

                }
                Scalar color( 0, 0, 255 );
                drawContours( frame, contours, largestComp, color, CV_FILLED, 8, hierarchy );
            }
            else
            {
                cout << "Sem contornos" << endl;
            }

            imshow("bilateral", frame);



            //findContours(frame, contours, hierarchy, CV_RETR_EXTERNAL , CV_CHAIN_APPROX_SIMPLE);
            //approxPolyDP(contours, )
            //imshow("erode", frame);
        }


        int c = waitKey(30);
        if( c == 'q' || c == 'Q' || (c & 255) == 27 )
            break;

    }

    cap.Close();

    return 0;
}
