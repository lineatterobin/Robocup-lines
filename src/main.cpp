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

void process(char* imsname, char* imfield)
{
    //Time to process
    double time = (double)getTickCount();

    //Load images
    Mat ims = imread(imsname);

    Mat imf = imread(imfield, CV_LOAD_IMAGE_GRAYSCALE);

    Mat new_ims = ims.clone();

    Mat imsG;

    //imshow("ims",ims);

    // Brightness & contrast
    for( int y = 0; y < ims.rows; y++ )
        for( int x = 0; x < ims.cols; x++ )
            for( int c = 0; c < 3; c++ )
                new_ims.at<Vec3b>(y,x)[c] = saturate_cast<uchar>( 2 * ims.at<Vec3b>(y,x)[c] );

    //imshow( "ims (contrast)", new_ims );
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

    equalizeHist(hsv[0], hsv[0]);
    //hsv[0] = Mat::ones(ims.rows, ims.cols, CV_8UC1) * 255 - hsv[0];
    // Extract yellow-ish colors
    for( int y = 0; y < hsv[0].rows; y++ )
        for( int x = 0; x < hsv[0].cols; x++ )
                hsv[0].at<uchar>(y,x) =  (abs((int)hsv[0].at<uchar>(y,x) - 66) < 20) * 255;


    //imshow( "HSV Value1", hsv[2]);
    //waitKey(0);

    //bilateralFilter(hsv[2].clone(), hsv[2], -1, 30, 10);
    threshold(hsv[2], hsv[2], 225, 255, THRESH_BINARY);

    imshow( "1", hsv[2]);
    //waitKey(0);

    //////////////////////////////////////////////////////////////////////////////////////
    //equalizeHist(imsG,imsG);
    bilateralFilter(imsG.clone(), imsG, -1, 20, 10);
    //imshow( "1.1 Filter", imsG );
    morphologyEx(imsG,imsG,CV_MOP_DILATE, getStructuringElement(MORPH_ELLIPSE, Size(8,8)));
    /*for( int y = 0; y < imsG.rows; y++ )
        for( int x = 0; x < imsG.cols; x++ )
                imsG.at<uchar>(y,x) = saturate_cast<uchar>( 2.5 * imsG.at<uchar>(y,x));*/
    //equalizeHist(imsG,imsG);
    //imshow( "1.15 Filter", imsG );
    //waitKey(0);

    adaptiveThreshold(imsG, imsG, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 0);

    //imshow( "1.2 Threshold", imsG );
    //waitKey(0);

    bilateralFilter(imsG.clone(), imsG, -1, 80, 10);

    //adaptiveThreshold(imsG, imsG, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 0);
    threshold(imsG,imsG,125,255,CV_THRESH_BINARY);
    //Mat kern = getStructuringElement(MORPH_CROSS, Size(8,8));
    //morphologyEx(imsG,imsG,CV_MOP_CLOSE, kern);
    //morphologyEx(imsG,imsG,CV_MOP_ERODE, getStructuringElement(MORPH_CROSS, Size(3,3)));
    imshow( "2", imsG );

    bitwise_or(hsv[2], imsG, imsG);
    //focus(imsG, imf);
    if(isWhite(imf))
        morphologyEx(imsG,imsG,CV_MOP_OPEN, getStructuringElement(MORPH_RECT, Size(3,3)));
    imshow( "Result1", imsG );
    ////////////HOUGHLINES
/*
    Mat dst, cdst;
    Canny(imsG, dst, 50, 200, 3);
    cvtColor(dst, cdst, CV_GRAY2BGR);

    /*vector<Vec2f> lines;
    HoughLines(dst, lines, 1, CV_PI/180, 100, 0, 0 );

    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        line( ims, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
    }
    vector<Vec4i> lines;
        HoughLinesP( dst, lines, 1, CV_PI/180, 80, 30, 10 );
        for( size_t i = 0; i < lines.size(); i++ )
        {
            line( ims, Point(lines[i][0], lines[i][1]),
                Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
        }

    */
    //////////////

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

    /*//////////////

    Mat imsRes = Mat::zeros(imsG.rows, imsG.cols, CV_8UC1);
    int countW = 0;
    int countB = 0;
    for (int i = 0; i < imsG.rows ; i++) {
        for (int j = 0; j < imsG.cols ; j++) {
            if(imsG.at<uchar>(i,j) == 255) {
                countW += 1;
            }else if(imsG.at<uchar>(i,j) == 0 && countW > 0){
                if (countB > 0 && countW/2 > 0){
                    imsRes.at<uchar>(i,j-2-countW/2) = 255;
                    countB = 0;
                    countW = 0;
                }
                else if(countB > 0 && countW/2 ==0){
                    countB = 0;
                    countW = 0;
                }
                else{
                    countB++;
                }
            }
        }
    }
    countW = 0;
    countB = 0;
    for (int i = 0; i < imsG.cols ; i++) {
        for (int j = 0; j < imsG.rows ; j++) {
            if(imsG.at<uchar>(j,i) == 255) {
                countW += 1;
            }else if(imsG.at<uchar>(j,i) == 0 && countW > 0){
                if (countB > 0 && countW/2 > 0){
                    imsRes.at<uchar>(j-2-countW/2,i) = 255;
                    countB = 0;
                    countW = 0;
                }
                else if(countB > 0 && countW/2 ==0){
                    countB = 0;
                    countW = 0;
                }
                else{
                    countB++;
                }
            }
        }
    }
    imshow( "imsResult", imsRes );

    //////////////*/

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

