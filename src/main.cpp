#include <stdio.h>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "BirdsEyeView.h"
#include "FourPoints.h"
#include "Polynomial.h"
#include "Window.h"
#include "PolyFit.h"
#include "LaneDetect.h"

#define ASCII_0 48

using namespace cv;
using namespace std;

void testBirdsEyeView();
void testFourPoints();
void testWindow();
void testPolynomial();
void testPolyFit();
void testLaneDetect();
void performTests();


int main(int argc, char** argv )
{
    performTests();

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

    //namedWindow("Histogram", WINDOW_NORMAL);

    BirdsEyeView bird;
    vid >> frame;
    bird.setInput(frame);
    bird.setRoi(roi);
    birdView = bird.getResult();

    cout<<"Frame resolution: "<<frame.cols<<" "<<frame.rows<<endl;

    imshow("Frame", frame);//causes leaks
    bird.setInput(frame);
    birdView = bird.getResult();

    LaneDetect lanes;

    lanes.setInput(birdView);
    lanes.initLines();


    //Window initialWindow(birdView);
    //initialWindow.push_back(Point(0,0), Point(frame.cols-1, 0), Point(frame.cols-1, frame.rows-1), Point(0, frame.rows-1));

    int frame_id = 0;
    while(!frame.empty())
    {
        imshow("Frame", frame);//causes leaks
        bird.setInput(frame);
        birdView = bird.getResult();

        lanes.setInput(birdView);
        lanes.updateLines();

        /*initialWindow.setInput(birdView);
        initialWindow.createHistograms();
        histogram = initialWindow.histToImg();
        imshow("Histogram", histogram);*/

        char c = (char)waitKey(30);//causes leaks
        if(c == 27)//break when user presses ESC
            break;

        vid >> frame;
        cout<<"Frame id: "<<frame_id++<<endl;
        //if(frame_id == 100) break;
    }
    return 0;
}



void performTests()
{
  testBirdsEyeView();
  cout<<"BirdsEyeView"<<endl;
  testFourPoints();
  //testWindow();
  testPolynomial();
  cout<<"Polynomial"<<endl;
  testPolyFit();
  cout<<"Polyfit"<<endl;
  //testLaneDetect();
}

void testBirdsEyeView()
{
  BirdsEyeView bird;
  Mat mat;
  mat = bird.getTransformed();
  mat = bird.getResult();
  vector<Point> a;
  for(int i = 0; i < 4; ++i)
    a.push_back(Point(0,0));
  bird.setRoi(a);

  mat = Mat::zeros(100, 100, CV_8UC3);
  bird.setInput(mat);

  BirdsEyeView bird2(mat);
  mat = bird2.getResult();
}

void testFourPoints()
{

}

void testPolynomial()
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
  v[1] = w[3];

  Polynomial <double> u = v;
  if(u[1] != 23)
    errors++;

  assert(errors == 0);
}

void testPolyFit()
{
  PolyFit polyfit;
  vector<Point> a;
  a.push_back(Point(10, 160));
  a.push_back(Point(150, 190));
  a.push_back(Point(300, 250));
  a.push_back(Point(450, 350));
  a.push_back(Point(550, 250));

  polyfit.setInput(a);
  Polynomial<double> result = polyfit.solve();
  cout<<result[0]<<" "<<result[1]<<" "<<result[2]<<endl;

  Mat polyMat(480, 640, CV_8UC3, Scalar(0, 0, 0));

  for(int i = 0; i < a.size(); ++i)
  {
    circle(polyMat, a[i], 3, Scalar(255, 0, 0), -1);
  }
  for(int i = 0; i < polyMat.cols; ++i)
  {
    int y = result.value(i);
    if(y >= 0 && y < polyMat.rows)
      polyMat.at<Vec3b>(Point(i, y)) = Vec3b(0, 0, 255);
  }
  namedWindow("PolyFit", WINDOW_NORMAL);
  imshow("PolyFit", polyMat);
  waitKey(0);
}
