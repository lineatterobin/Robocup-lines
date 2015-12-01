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
                ims.at<Vec3b>(i,j) = V;
        }
    }
}


void process(char* imsname, char* imfield)
{
	//Time to process
	double time = (double)getTickCount();

	//Load images
    Mat ims = imread(imsname);
    Mat imf = imread(imfield, CV_LOAD_IMAGE_GRAYSCALE);
    Mat imsG;

	//Convert main image to a gray scale
    cvtColor(ims, imsG, CV_BGR2GRAY);

	//Equalize histogram
	Mat eqIms;
	equalizeHist(ims, eqIms);

	//Apply HoughLinesP function
    Mat dst, color_dst;
    Mat result(ims.rows, ims.cols, CV_8UC3);
    Canny( imsG, dst, 30, 200, 3 );
    cvtColor( dst, color_dst, CV_GRAY2BGR );
    vector<Vec4i> lines;
    HoughLinesP( dst, lines, 1, 1*CV_PI/180, 20, 30, 10 );
    ims.copyTo(result);
    for( size_t i = 0; i < lines.size(); i++ ) {
        line( result, Point(lines[i][0], lines[i][1]),
                Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
    }

	// Remove parts of the image not from the football field
    focus(result, imf);

	// Process and display execution time
	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "Execution time:" << time << endl;

    namedWindow("Source");
    imshow( "Source", imsG );

    namedWindow( "Detected Lines");
    imshow( "Detected Lines", result );

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

