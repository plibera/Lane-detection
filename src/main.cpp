#include <stdio.h>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


int main(int argc, char** argv )
{
    VideoCapture vid;
    if(argc > 1)
        vid.open(argv[1]);
    else
        vid.open("/home/piotr/Videos/road1.mp4");

    if(!vid.isOpened())
    {
        cout<<"File not opened"<<endl;
        return -1;
    }

    Mat frame;
    namedWindow("Frame", WINDOW_NORMAL);
    resizeWindow("Frame", 1080, 720);

    vid >> frame;
    while(!frame.empty())
    {
        imshow("Frame", frame);
        char c = (char)waitKey(30);
        if(c == 27)//break when user presses ESC
            break;

        vid >> frame;
    }
    return 0;
}
