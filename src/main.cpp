#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

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


void process(char* imsfile, char* imfield, char* imball, char* imtheo, char*imres)
{
    vector<string> calibList;
    readStringList(imsfile, calibList);

    vector<string> balllist;
    readStringList(imball, balllist);

    vector<string> theolist;
    readStringList(imtheo, theolist);

    vector<string> fieldlist;
    readStringList(imfield, fieldlist);

    vector<string> reslist;
    readStringList(imres, reslist);

    int nframes = 0;
    if( !calibList.empty() ) {
        nframes = (int)calibList.size();
        cout<< nframes <<  " images" << endl;
    }
    else {
        cout <<  "No images" << endl;
        return;
    }

    for(int i = 0; i < nframes ;i++) {

        cout << "image "<< i << endl;

        //Time to process
        double time = (double)getTickCount();

        //Load images
        Mat ims = imread(calibList[i], 1);
        Mat imb = imread(balllist[i], CV_LOAD_IMAGE_GRAYSCALE);
        Mat imt = imread(theolist[i], CV_LOAD_IMAGE_GRAYSCALE);

        Mat imf = imread(fieldlist[i], CV_LOAD_IMAGE_GRAYSCALE);

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

        GaussianBlur(imsG,imsG,Size(9,9), 2);
        medianBlur(imsG,imsG,5);
        imshow( "Gaussian", imsG );

        adaptiveThreshold(imsG, imsG, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 0);

        imshow( "2", imsG );

        focus(imsG, imf, imb);
        imshow("ball", imb);
        //imshow( "Result1.0", imsG );
        morphologyEx(imsG,imsG,CV_MOP_CLOSE, getStructuringElement(MORPH_RECT, Size(3,3)));
        //imshow( "Result1.05", imsG );
        morphologyEx(imsG,imsG,CV_MOP_OPEN, getStructuringElement(MORPH_RECT, Size(3,3)));
        imshow( "Result1", imsG );

        vector<Vec2f> lines;
        HoughLines(imsG, lines, 1, CV_PI/180, 230, 0, 0 );


        /*Mat Mlines = Mat::zeros(sqrt((imsG.rows*imsG.rows)+(imsG.cols*imsG.cols)), 158, CV_8UC1);

        for( size_t i = 0; i < lines.size(); i++ )
        {
            float rho = lines[i][0], theta = lines[i][1];
            Mlines.at<uchar>(rho,theta*100) = 255;
        }
        imshow("HOUGH", Mlines);

        morphologyEx(Mlines, Mlines, CV_MOP_DILATE, getStructuringElement(MORPH_ELLIPSE, Size(3 ,3)));

        imshow("HOUGH2", Mlines);

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        findContours(Mlines, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        RNG rng(12345);
        Mat drawing = Mat::zeros( Mlines.size(), CV_8UC3 );
        for( int i = 0; i< contours.size(); i++ )
        {
            Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            drawContours( drawing, contours, i, color, 2, 8,hierarchy ,0, Point() );
        }
        imshow("draw", drawing);

        lines.clear();
        for(size_t i = 0; i < contours.size(); ++i){
            Vec2f average(0,0);
            for(size_t j = 0; j < contours[i].size(); ++j){
                average[0] += contours[i][j].x;
                average[1] += contours[i][j].y;
            }
            average[0] /= contours[i].size();
            average[1] /= contours[i].size();
            lines.push_back(average);
        }*/

        for( size_t i = 0; i < lines.size(); i++ )
        {
            float rho = lines[i][0] , theta = lines[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            line( ims, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
        }

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

        imwrite(reslist[i], ims);
        waitKey(0);

    }
}


#define param 5
int main(int argc, char** argv)
{
    if(argc != param + 1)
        cout<<"Usage: imsfile imfield imball imtheo"<<endl;
    else
    {
        process(argv[1], argv[2], argv[3], argv[4], argv[5]);
    }
    return 0;
}

