#include <stdio.h>
#include <fstream>
#include <opencv2/opencv.hpp>
//#include "BirdsEyeView.h"
//#include "Polynomial.h"

#define ASCII_0 48

using namespace cv;
using namespace std;


int main(int argc, char** argv )
{
    if(argc < 3)
    {
        cout<<"Input not specified"<<endl;
        return -1;
    }
    ifstream file(argv[1]);
    if(!file.is_open())
    {
        cout<<"Failed to open config file"<<endl;
        return -1;
    }
    int inputNum = argv[2][0] - ASCII_0;
    vector<Point> roi;
    Point a;
    string inputName;
    for(int i = 0; i < inputNum; ++i)
    {
        roi.clear();
        file>>inputName;
        for(int j = 0; j < 4; ++j)
        {
            file>>a.x>>a.y;
            roi.push_back(a);
        }
    }
    file.close();

    VideoCapture vid;
    vid.open(inputName);

    if(!vid.isOpened())
    {
        cout<<"Input not opened"<<endl;
        return -1;
    }

    Mat frame;

    //namedWindow("Frame", WINDOW_NORMAL);//causes leaks
    //resizeWindow("Frame", 1080, 720);//no point using without a window

    //BirdsEyeView bird;
    vid >> frame;
    //bird.setInput(frame);
    //bird.setRoi(roi);
    int frame_id = 0;
    while(!frame.empty())
    {
        //imshow("Frame", frame);//causes leaks
        //bird.setInput(frame);

        //Polynomial <int> w;


        //char c = (char)waitKey(1);
        //if(c == 27)//break when user presses ESC
          //  break;

        vid >> frame;
        cout<<"Frame id: "<<frame_id++<<endl;
        if(frame_id == 100) break;
    }
    return 0;
}
