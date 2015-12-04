#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void focus(Mat ims, Mat imf)
{
    // Remove parts of the image not from the football field
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
    //Time to process
    double time = (double)getTickCount();

    //Load images
    Mat ims = imread(imsname);
    Mat new_ims = ims.clone();
    Mat imf = imread(imfield, CV_LOAD_IMAGE_GRAYSCALE);
    Mat imsG;

    imshow("ims",ims);

    // Brightness & contrast
    for( int y = 0; y < ims.rows; y++ )
        for( int x = 0; x < ims.cols; x++ )
            for( int c = 0; c < 3; c++ )
                new_ims.at<Vec3b>(y,x)[c] = saturate_cast<uchar>( 2 * ims.at<Vec3b>(y,x)[c] );

    imshow( "ims (contrast)", new_ims );
    //waitKey(0);


    //Convert main image to a gray scale
    cvtColor(ims, imsG, CV_BGR2GRAY);

    Mat hsv[3];
    {
        Mat hsv2;
        cvtColor(new_ims, hsv2, CV_BGR2HSV);
        split(hsv2, hsv);
    }


    //////////////////////////////////////////////////////////////////////////////////////
/*
    equalizeHist(hsv[0], hsv[0]);
    //hsv[0] = Mat::ones(ims.rows, ims.cols, CV_8UC1) * 255 - hsv[0];
    // Extract yellow-ish colors
    for( int y = 0; y < hsv[0].rows; y++ )
        for( int x = 0; x < hsv[0].cols; x++ )
                hsv[0].at<uchar>(y,x) =  (abs((int)hsv[0].at<uchar>(y,x) - 66) < 20) * 255;
*/

    imshow( "HSV Value1", hsv[2]);
    //waitKey(0);

    //bilateralFilter(hsv[2].clone(), hsv[2], -1, 30, 10);
    threshold(hsv[2], hsv[2], 225, 255, THRESH_BINARY);

    imshow( "HSV Value", hsv[2]);
    //waitKey(0);

    //////////////////////////////////////////////////////////////////////////////////////
    //equalizeHist(imsG,imsG);
    bilateralFilter(imsG.clone(), imsG, -1, 30, 10);
    imshow( "1.1 Filter", imsG );
    morphologyEx(imsG,imsG,CV_MOP_DILATE, getStructuringElement(MORPH_ELLIPSE, Size(8,8)));
    /*for( int y = 0; y < imsG.rows; y++ )
        for( int x = 0; x < imsG.cols; x++ )
                imsG.at<uchar>(y,x) = saturate_cast<uchar>( 2.5 * imsG.at<uchar>(y,x));*/
    equalizeHist(imsG,imsG);
    imshow( "1.15 Filter", imsG );
    //waitKey(0);

    adaptiveThreshold(imsG, imsG, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 0);

    //imshow( "1.2 Threshold", imsG );
    //waitKey(0);

    bilateralFilter(imsG.clone(), imsG, -1, 80, 10);

    adaptiveThreshold(imsG, imsG, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 0);
    //threshold(imsG,imsG,125,255,CV_THRESH_BINARY);
    Mat kern = getStructuringElement(MORPH_CROSS, Size(8,8));
    morphologyEx(imsG,imsG,CV_MOP_CLOSE, kern);
    //morphologyEx(imsG,imsG,CV_MOP_ERODE, getStructuringElement(MORPH_CROSS, Size(3,3)));

    //focus(imsG, imf);

    imshow( "1.3 Morpho", imsG );

    // Process and display execution time
    time = ((double)getTickCount() - time) / getTickFrequency();
    cout << "Execution time:" << time << endl;

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

