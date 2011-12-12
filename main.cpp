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

#define NEXT_CONTOUR        0
#define PREVIOUS_CONTOUR    1
#define CHILD_CONTOUR       2
#define PARENT_CONTOUR      3


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

void nn_256_5_3_1(double * inputs, double * outputs);
void nn_256_7_1(double * inputs, double * outputs);

double digitOCR(Mat& img)
{
    IplImage dst_img = img;

    IplImage scaled = preprocessing(&dst_img, 16,16);

    double* nn_inputs = new double[16*16];
    double* nn_outputs = new double[1];

    for (int i = 0; i < (16*16); i++)
    {
        nn_inputs[i] = scaled.imageData[i] == 0 ? 1.0 : 0.0;
        cout << (nn_inputs[i] == 0.0 ? " " : "X");
        if (i%16 == 0)
            cout << endl;
    }

    cout << endl;

    nn_256_5_3_1(nn_inputs, nn_outputs);
    double ret = nn_outputs[0];

    delete nn_inputs;
    delete nn_outputs;

    return ret;
}

int processa_ocr()
{
    const char* inputImage = "ocr.png";

    Mat original = imread(inputImage,0);
    Mat image = original.clone();

    if(image.empty())
    {
        std::cerr << "Cannot read image file: " << inputImage << std::endl;
        return -1;
    }
    //namedWindow( "image", 1 );
    namedWindow( "contours", 1 );
    namedWindow( "sel", 1 );
    namedWindow( "ocr", 1 );


    bool fgUpdate = true;

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //imshow( "image", image );

    vector<vector<Point> > contours0;

    findContours( image, contours0, hierarchy, CV_RETR_TREE, CHAIN_APPROX_SIMPLE);
    contours.resize(contours0.size());
    for( size_t k = 0; k < contours0.size(); k++ )
        approxPolyDP(Mat(contours0[k]), contours[k], 3, true);

    cout << "contours0.size() = " << contours.size() << endl;

    vector<int> digits_contours;

    for( size_t k = 0; k < hierarchy.size(); k++ )
    {
        Vec4i h = hierarchy[k];
        cout << k << " next: " << h[NEXT_CONTOUR] << " previous: "<< h[PREVIOUS_CONTOUR] << " child: " << h[CHILD_CONTOUR] << " parent: " << h[PARENT_CONTOUR] << endl;
    }

    for( size_t k = 0; k < hierarchy.size(); k++ )
    {
        Vec4i h = hierarchy[k];
        vector<Point> c = contours[k];
        cout << h[3] << " ";
        if (h[3] == 0 && contours[k].size() >= 4)
        {
            cout << "*";
            digits_contours.push_back(k);
        }

        Rect rect = boundingRect(Mat(contours[k]));

        cout << "contour " << k << " : x=" << rect.x << ", y=" << rect.y << endl;

    }

    int idx = 0;
    while (true)
    {
        if (fgUpdate)
        {

            Mat cnt_img = Mat::zeros(480, 640, CV_8UC3);

            drawContours( cnt_img, contours, digits_contours[idx], Scalar(128,255,255),1, CV_AA, hierarchy, 2 );

            Rect rect = boundingRect(Mat(contours[digits_contours[idx]]));
            rect.x = rect.x - 5;
            rect.y = rect.y - 5;
            rect.width = rect.width + 10;
            rect.height = rect.height + 10;

            Mat recImg(original, rect);


            imshow("contours", cnt_img);
            imshow("sel", recImg);
            cout << "OCR: " << digitOCR(recImg) << endl;

            cout << "Window updated with " << idx << endl;
            idx++;
            fgUpdate = false;
        }

        int c = waitKey(30);
        if( c == 'q' || c == 'Q' || (c & 255) == 27 )
            break;
        if( c == 'u' || c == 'U' )
            fgUpdate = true;

    }
    return 0;
}

int main( int argc, char** argv )
{
    help();
    readParam();

    return processa_ocr();

    //buildTrainData();
    //return 0;

    namedWindow("Controles");

    createTrackbar("blur_ksize", "Controles", &blur_ksize, 30, onChangeParam);
    createTrackbar("threshold_thresh", "Controles", &threshold_thresh, 200, onChangeParam);


    MjpegCapture cap("192.168.1.100", "8080", "/videofeed");
    cap.Open();

    int niters = 3;
    Mat recImg;
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

            findContours( temp, contours, hierarchy, CV_RETR_EXTERNAL , CV_CHAIN_APPROX_SIMPLE );

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
            imshow("sel", recImg);
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
