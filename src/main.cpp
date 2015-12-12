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
            if(imf.at<uchar>(i,j) == 0){
                if(ims.type() == CV_8UC3)
                    ims.at<Vec3b>(i,j) = V;
                else
                    ims.at<uchar>(i,j) = 0;
            }
            if(imb.at<uchar>(i,j) > 0){
                if(ims.type() == CV_8UC3)
                    ims.at<Vec3b>(i,j) = V;
                else
                    ims.at<uchar>(i,j) = 0;
            }
        }
    }
}

/*bool isWhite(Mat img)
{
    for (int i = 0; i < img.rows ; i++) {
        for (int j = 0; j < img.cols ; j++) {
            if(img.at<uchar>(i,j) == 0)
                return false;
        }
    }
    return true;
}*/

void process(char* imsfile, char* imfield, char* imball, char* imtheo)
{
    vector<string> calibList;
    readStringList(imsfile, calibList);

    int nframes = 0;
    if( !calibList.empty() ) {
        nframes = (int)calibList.size();
        cout<< nframes <<  " images" << endl;
    }
    else {
        cout <<  "No images" << endl;
        return 1;
    }

    for(int i = 0; i < nframes ;i++) {

        cout << "image "<< i << endl;

        //-----  If no more image, or got enough, then stop calibration and show result -------------
        if(!im.empty())
        {
            //Time to process
            double time = (double)getTickCount();

            //Load images
            Mat ims = imread(calibList[i], 1);
            Mat imb = imread(imball, CV_LOAD_IMAGE_GRAYSCALE);
            Mat imt = imread(imtheo, CV_LOAD_IMAGE_GRAYSCALE);

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

            /*//////////////////////////////////////////////////////////////////////////////////////

            equalizeHist(hsv[0], hsv[0]);
            // Extract yellow-ish colors
            for( int y = 0; y < hsv[0].rows; y++ )
                for( int x = 0; x < hsv[0].cols; x++ )
                    hsv[0].at<uchar>(y,x) =  (abs((int)hsv[0].at<uchar>(y,x) - 66) < 20) * 255;

            threshold(hsv[2], hsv[2], 225, 255, THRESH_BINARY);

            //imshow( "1", hsv[2]);
            //////////////////////////////////////////////////////////////////////////////////////*/


            //bilateralFilter(imsG.clone(), imsG, -1, 20, 10);
            GaussianBlur(imsG,imsG,Size(9,9), 2);
            medianBlur(imsG,imsG,5);

            //morphologyEx(imsG,imsG,CV_MOP_DILATE, getStructuringElement(MORPH_ELLIPSE, Size(8,8)));

            adaptiveThreshold(imsG, imsG, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 0);

            //bilateralFilter(imsG.clone(), imsG, -1, 80, 10);

            //threshold(imsG,imsG,125,255,CV_THRESH_BINARY);

            imshow( "2", imsG );

            //bitwise_or(hsv[2], imsG, imsG);
            focus(imsG, imf, imb);
            imshow("ball", imb);
            imshow( "Result1.0", imsG );
            morphologyEx(imsG,imsG,CV_MOP_CLOSE, getStructuringElement(MORPH_RECT, Size(3,3)));
            imshow( "Result1.05", imsG );
            morphologyEx(imsG,imsG,CV_MOP_OPEN, getStructuringElement(MORPH_RECT, Size(3,3)));
            imshow( "Result1", imsG );

            vector<Vec2f> lines;
            HoughLines(imsG, lines, 1, CV_PI/180, 200, 0, 0 );

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

            /*for (int i = 0; i < imsG.rows ; i++) {
        for (int j = 0; j < imsG.cols ; j++) {
            if(imsG.at<uchar>(i,j) == 255) {
                ims.at<Vec3b>(i,j)[0] = 0;
                ims.at<Vec3b>(i,j)[1] = 0;
                ims.at<Vec3b>(i,j)[2] = 255;
            }
            /*if(imt.at<uchar>(i,j) == 0) {
                ims.at<Vec3b>(i,j)[0] = 0;
                ims.at<Vec3b>(i,j)[1] = 0;
                ims.at<Vec3b>(i,j)[2] = 0;
            }
        }
    }*/

            imshow( "Result", ims );
            //imshow( "imt", imt);
            int false_positive = 0;
            int false_negative = 0;
            for (int i = 0; i < imsG.rows ; i ++) {
                for (int j = 0; j < imsG.cols ; j++) {
                    if(imsG.at<uchar>(i,j) == 255 && imt.at<uchar>(i,j) != 0)
                        false_positive++;
                    else if (imsG.at<uchar>(i,j) != 255 && imt.at<uchar>(i,j) == 0)
                        false_negative++;
                }
            }

            // Process and display execution time
            time = ((double)getTickCount() - time) / getTickFrequency();
            cout << "Execution time:" << time << endl;
            cout << "false_po " << false_positive << " false_neg " << false_negative << endl;

            waitKey(0);
        }
    }
}

static bool readStringList( const string& filename, vector<string>& l )
{
    l.resize(0);
    FileStorage fs(filename, FileStorage::READ);
    if( !fs.isOpened() )
        return false;
    FileNode n = fs.getFirstTopLevelNode();
    if( n.type() != FileNode::SEQ )
        return false;
    FileNodeIterator it = n.begin(), it_end = n.end();
    for( ; it != it_end; ++it )
        l.push_back((string)*it);
    return true;
}

#define param 4
int main(int argc, char** argv)
{
    if(argc != param + 1)
        cout<<"Usage: imsfile imfield imball imtheo"<<endl;
    else
    {
        process(argv[1], argv[2], argv[3], argv[4]);
    }
    return 0;
}

