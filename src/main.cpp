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
    //performTests();

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

    namedWindow("Frame", WINDOW_NORMAL);
    resizeWindow("Frame", 1080, 720);

    BirdsEyeView bird;
    vid >> frame;
    bird.setInput(frame);
    bird.setRoi(roi);
    birdView = bird.getResult();

    cout<<"Frame resolution: "<<frame.cols<<" "<<frame.rows<<endl;

    imshow("Frame", frame);
    bird.setInput(frame);
    birdView = bird.getResult();

    LaneDetect lanes;

    lanes.setInput(birdView);
    lanes.initLines();

    int frame_id = 0;
    vector<Polynomial<double> > polyLines;
    Mat result, output, fin;
    result = Mat::zeros(frame.rows, frame.rows, CV_8UC3);
    output = Mat::zeros(frame.rows, frame.rows, CV_8UC3);
    while(!frame.empty())
    {
        imshow("Frame", frame);//causes leaks
        bird.setInput(frame);
        birdView = bird.getResult();

        lanes.setInput(birdView);
        lanes.updateLines();

        polyLines = lanes.getLineFits();

        if(polyLines[0][0] == 0 && polyLines[0][1] == 0 && polyLines[0][2] == 0)
          lanes.initLines();

        result = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
        int x;
        for(int y = 0; y < result.rows; ++y)
        {
          for(int i = 0; i < polyLines.size(); ++i)
          {
            x = polyLines[i].value(y);
            if(x >= 0 && x < result.cols)
              result.at<Vec3b>(Point(x, y)) = Vec3b(100, 100, 100);
          }
        }

        bird.setInput(result);
        result = bird.getUnwarped();
        cvtColor(result, output, CV_GRAY2BGR);
        fin = frame;
        for(int x = 0; x < output.cols; ++x)
        {
          for(int y = 0; y < output.rows; ++y)
          {
            if(y > frame.rows - WINDOWS*WINDOW_HEIGHT)
            {
              Vec3b pixelVal = output.at<Vec3b>(Point(x, y));
              if(pixelVal.val[0] > 0)
                frame.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 255);
            }
          }
        }
        imshow("Final", fin);

        char c = (char)waitKey(30);
        if(c == 27)//break when user presses ESC
            break;

        vid >> frame;
        cout<<"Frame id: "<<frame_id++<<endl;
    }
    return 0;
}



void performTests()
{
  testBirdsEyeView();
  cout<<"BirdsEyeView OK"<<endl;
  testFourPoints();
  cout<<"FourPoints OK"<<endl;
  testWindow();
  cout<<"Window OK"<<endl;
  testPolynomial();
  cout<<"Polynomial OK"<<endl;
  testPolyFit();
  cout<<"Polyfit OK"<<endl;
  testLaneDetect();
  cout<<"LaneDetect OK"<<endl;
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
  vector<Point> pvec;
  FourPoints a;
  FourPoints b(Point(10, 10));
  FourPoints c(pvec, Point(10, 10));
  pvec.push_back(Point(0,0));
  pvec.push_back(Point(20,0));
  pvec.push_back(Point(0,20));
  pvec.push_back(Point(20,20));
  FourPoints d(pvec, Point(10, 10));
  FourPoints e(Point(0, 0), Point(10, 0), Point(10, 10), Point(0, 10), Point(10, 10));
  c.push_back(pvec);
  pvec = a.getPoints(0);
  pvec = b.getPoints(13);
  pvec = d.getPoints(0);
  vector<Point> pvec2 = c.getPoints(0);
  assert(pvec == pvec2);
  e.setBoundaries(Point(5, 5));
  a.setBoundaries(-1, -1);
  assert(!a.withinBoundaries(Point(20, 20)));
  c.clear();
  c.size();
  d.size();
  a.size();
  d.polygonCentre(0);
  a.polygonCentre(0);
  e.polygonCentre(30);

  FourPoints s = a;
  cout<<d<<endl<<e<<endl;
}

void testWindow()
{
  Mat mata = Mat::zeros(100, 100, CV_8UC1);
  Window a(mata);
  Mat matb;
  Window b(matb);
  vector<Point> pvec;
  pvec.push_back(Point(0,0));
  pvec.push_back(Point(20,0));
  pvec.push_back(Point(0,200));
  pvec.push_back(Point(20,200));
  Window c(mata, pvec);
  Window d(mata, Point(0,0), Point(10, 0), Point(10, 10), Point(0, 10));
  Point point = d.polygonCentre(0);

  a.createHistograms();
  b.createHistograms();
  c.createHistograms();
  d.getHistograms();
  vector<int> hists = c.getHistogram(0);
  matb = c.histToImg();
  c.setWindowCentre(0, Point(5, 5));
  c.getWindowCentres();
  a.setInput(matb);
  Window e = d;
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

void testLaneDetect()
{
  Mat mata = Mat::zeros(100, 100, CV_8UC1);
  LaneDetect a;
  LaneDetect b(mata);

  a.initLines();
  a.updateLines();
  LaneDetect c = a;
  c.updateLines();
  LaneDetect d = a;
  d.getLineFits();
  b.updateLines();
  b.initLines();
  b.updateLines();
}
