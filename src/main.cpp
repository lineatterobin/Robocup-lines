#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void focus(Mat ims, Mat imf, Mat imb)
{
    // Remove parts of the image not from the football field and remove the ball.
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
            if(imb.at<uchar>(i,j) == 255)
                if(ims.type() == CV_8UC3)
                    ims.at<Vec3b>(i,j) = V;
                else
                    ims.at<uchar>(i,j) = 0;
        }
    }
}

bool isWhite(Mat img)
{
    for (int i = 0; i < img.rows ; i++) {
        for (int j = 0; j < img.cols ; j++) {
            if(img.at<uchar>(i,j) == 0)
                return false;
        }
    }
    return true;
}

void process(char* imsname, char* imfield, char* imball, char* imtheo)
{
    //Time to process
    double time = (double)getTickCount();

    //Load images
    Mat ims = imread(imsname);
    Mat imb = imread(imball);
    Mat imtheo = imread(imtheo);

    Mat imf = imread(imfield, CV_LOAD_IMAGE_GRAYSCALE);

    Mat new_ims = ims.clone();

    Mat imsG;

    // Brightness & contrast
    for( int y = 0; y < ims.rows; y++ )
        for( int x = 0; x < ims.cols; x++ )
            for( int c = 0; c < 3; c++ )
                new_ims.at<Vec3b>(y,x)[c] = saturate_cast<uchar>( 2 * ims.at<Vec3b>(y,x)[c] );


    //Convert main image to a gray scale
    cvtColor(ims, imsG, CV_BGR2GRAY);

    Mat hsv[3];
    {
        Mat hsv2;
        cvtColor(new_ims, hsv2, CV_BGR2HSV);
        split(hsv2, hsv);
    }


    //////////////////////////////////////////////////////////////////////////////////////

    equalizeHist(hsv[0], hsv[0]);
    // Extract yellow-ish colors
    for( int y = 0; y < hsv[0].rows; y++ )
        for( int x = 0; x < hsv[0].cols; x++ )
                hsv[0].at<uchar>(y,x) =  (abs((int)hsv[0].at<uchar>(y,x) - 66) < 20) * 255;

    threshold(hsv[2], hsv[2], 225, 255, THRESH_BINARY);

    imshow( "1", hsv[2]);
    //////////////////////////////////////////////////////////////////////////////////////
    bilateralFilter(imsG.clone(), imsG, -1, 20, 10);

    morphologyEx(imsG,imsG,CV_MOP_DILATE, getStructuringElement(MORPH_ELLIPSE, Size(8,8)));

    adaptiveThreshold(imsG, imsG, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 0);

    bilateralFilter(imsG.clone(), imsG, -1, 80, 10);

    threshold(imsG,imsG,125,255,CV_THRESH_BINARY);

    imshow( "2", imsG );

    bitwise_or(hsv[2], imsG, imsG);
    //focus(imsG, imf, imb);
    if(isWhite(imf))
        morphologyEx(imsG,imsG,CV_MOP_OPEN, getStructuringElement(MORPH_RECT, Size(3,3)));
    imshow( "Result1", imsG );

    for (int i = 0; i < imsG.rows ; i++) {
        for (int j = 0; j < imsG.cols ; j++) {
            if(imsG.at<uchar>(i,j) == 255) {
                ims.at<Vec3b>(i,j)[0] = 0;
                ims.at<Vec3b>(i,j)[1] = 0;
                ims.at<Vec3b>(i,j)[2] = 255;
            }
        }
    }

    imshow( "Result", ims );

    // Process and display execution time
    time = ((double)getTickCount() - time) / getTickFrequency();
    cout << "Execution time:" << time << endl;

    waitKey(0);

}

#define param 4
int main(int argc, char** argv)
{
    if(argc != param + 1)
        cout<<"Usage: imsname imfield imball imtheo"<<endl;
    else
    {
        process(argv[1], argv[2], argv[3], argv[4]);
    }
    return 0;
}

