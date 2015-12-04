#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void focus(Mat ims, Mat imf)
{
    Vec3b V(0,0,0);
    for(int i=0; i<imf.rows; ++i)
    {
        for(int j=0; j<imf.cols; ++j)
        {
            if(imf.at<uchar>(i,j) == 0)
                if(ims.type() == CV_8UC3)
                    ims.at<Vec3b>(i,j) = V;
                else
                    ims.at<uchar>(i,j) = 0;
        }
    }
}


void process(char* imsname, char* imfield)
{
    Mat ims = imread(imsname);
    Mat imf = imread(imfield, CV_LOAD_IMAGE_GRAYSCALE);
    Mat imsG;
    cvtColor(ims, imsG, CV_BGR2GRAY);

    Mat hsv[3];
    {
        Mat hsv2;
        cvtColor(ims, hsv2, CV_BGR2HSV);
        split(hsv2, hsv);
    }


    //////////////////////////////////////////////////////////////////////////////////////
    equalizeHist(hsv[0], hsv[0]);
    hsv[0] = Mat::ones(ims.rows, ims.cols, CV_8UC1) * 255 - hsv[0];


    namedWindow("Source");
    imshow( "Source", hsv[0] );
    waitKey(0);
    //////////////////////////////////////////////////////////////////////////////////////
    //equalizeHist(imsG,imsG);
    bilateralFilter(imsG.clone(), imsG, -1, 30, 10);

    namedWindow("Source");
    imshow( "Source", imsG );
    waitKey(0);

    adaptiveThreshold(imsG, imsG, 200, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 0);

    //focus(imsG, imf);
    namedWindow("Source");
    imshow( "Source", imsG );
    waitKey(0);

    bilateralFilter(imsG.clone(), imsG, -1, 80, 10);
    adaptiveThreshold(imsG, imsG, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 0);
    //threshold(imsG,imsG,125,255,CV_THRESH_BINARY);
    Mat kern = getStructuringElement(MORPH_CROSS, Size(8,8));
    morphologyEx(imsG,imsG,CV_MOP_CLOSE, kern);
    morphologyEx(imsG,imsG,CV_MOP_ERODE, getStructuringElement(MORPH_CROSS, Size(3,3)));


    namedWindow("Source");
    imshow( "Source", imsG );
    imshow("ims",ims);
    waitKey(0);

    waitKey(0);
}

#define param 2
int main(int argc, char** argv)
{
    if(argc != param + 1)
        cout<<"Usage: imsname imfield"<<endl;
    else
    {
        process(argv[1], argv[2]);
    }
    return 0;
}

