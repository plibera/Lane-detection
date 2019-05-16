#include <stdio.h>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "BirdsEyeView.h"
#include "FourPoints.h"
#include "Polynomial.h"
#include "Window.h"

#define ASCII_0 48

using namespace cv;
using namespace std;


void testPolynomials();//tests for Polynomial.h


int main(int argc, char** argv )
{
    testPolynomials();

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

    Mat frame, birdView, histogram;

    namedWindow("Frame", WINDOW_NORMAL);//causes leaks
    resizeWindow("Frame", 1080, 720);//no point using without a window

    namedWindow("Histogram", WINDOW_NORMAL);

    BirdsEyeView bird;
    vid >> frame;
    bird.setInput(frame);
    bird.setRoi(roi);
    birdView = bird.getResult();

    cout<<frame.cols<<" "<<frame.rows<<endl;

    Window initialWindow(birdView);
    initialWindow.push_back(Point(0,0), Point(frame.cols-1, 0), Point(frame.cols-1, frame.rows-1), Point(0, frame.rows-1));
    int frame_id = 0;
    while(!frame.empty())
    {
        imshow("Frame", frame);//causes leaks
        bird.setInput(frame);
        birdView = bird.getResult();


        initialWindow.setInput(birdView);
        initialWindow.createHistograms();
        histogram = initialWindow.histToImg();
        imshow("Histogram", histogram);

        char c = (char)waitKey(10);//causes leaks
        if(c == 27)//break when user presses ESC
            break;

        vid >> frame;
        cout<<"Frame id: "<<frame_id++<<endl;
        //if(frame_id == 100) break;
    }
    return 0;
}


void testPolynomials()
{
  int errors = 0;

  Polynomial <int> w (3);
  w[0] = -1;
  w[1] = 2;
  w[2] = 4.5;
  w[3] = 23;
  if(w[0] != -1)
  {
    errors++;
  }
  w[0] = w.degree();

  Polynomial<double> v;
  w[0] = v[0];
  v[0] = w[1];
  if(v[0] != 2)
  {
    errors++;
  }
  assert(errors == 0);
}
