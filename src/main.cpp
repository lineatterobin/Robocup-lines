#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void process(char* imsname)
{

}

#define param 1
int main(int argc, char** argv)
{
    if(argc != param + 1)
        cout<<"Usage: imsname"<<endl;
    else
    {
        process(argv[1]);
    }
    return 0;
}

