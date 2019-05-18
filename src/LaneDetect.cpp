#include "LaneDetect.h"

using namespace std;
using namespace cv;


LaneDetect::LaneDetect()
{
  inputSet = 0;
}

LaneDetect::LaneDetect(Mat in)
{
  input = in;
  inputSet = 1;
}

void LaneDetect::setInput(Mat in)
{
  input = in;
  for(int i = 0; i < roadLines.size(); ++i)
  {
    roadLines[i].setInput(input);
  }
  inputSet = 1;
}

vector<Polynomial<double> > LaneDetect::getLineFits()
{
  return lineFits;
}

void LaneDetect::initLines()
{
  if(!inputSet)
    return;

  roadLines.clear();
  lineFits.clear();
  Window roi(input, Point(0, input.rows/2), Point(input.cols-1, input.rows/2), Point(input.cols-1, input.rows-1), Point(0, input.rows-1));
  roi.createHistograms();
  vector<int> histogram = roi.getHistogram(0);
  vector<int> newCentres;

  int sumThreshold = 100;

  for(int x = WINDOW_WIDTH/2; x < input.cols - WINDOW_WIDTH/2; ++x)
  {
    int xp = x;
    for(int j = 0; j < WINDOW_WIDTH/2; ++j)
    {
      if(histogram[x] < histogram[x+j])
        x = xp + j;
    }
    if(x != xp)
    {
      x--;
      continue;
    }
    int sum = 0;
    for(int j = x - WINDOW_WIDTH/2; j < x + WINDOW_WIDTH/2; ++j)
    {
      sum += histogram[j];
    }
    if(sum >= sumThreshold)
    {
      newCentres.push_back(x);
    }
    x += WINDOW_WIDTH-1;
  }

  vector<Point> lineStart;
  for(int i = 0; i < newCentres.size(); ++i)
  {
    roi.clear();
    roi.push_back(Point(newCentres[i]-WINDOW_WIDTH/2, 430), Point(newCentres[i]+WINDOW_WIDTH/2, 430),
                  Point(newCentres[i]+WINDOW_WIDTH/2, 480), Point(newCentres[i]-WINDOW_WIDTH/2, 480));
    roi.createHistograms();
    histogram = roi.getHistogram(0);
    for(int j = 0; j < histogram.size(); ++j)
    {
      if(histogram[j] > 0)
      {
        lineStart.push_back(Point(newCentres[i], 455));
        break;
      }
    }
  }


  Mat debug;
  cvtColor(input, debug, COLOR_GRAY2BGR);
  for(int i = 0; i < lineStart.size(); ++i)
  {
    //rectangle(debug, Point(newCentres[i]-WINDOW_WIDTH/2, 430), Point(newCentres[i]+WINDOW_WIDTH/2, 480), Scalar(255, 0, 0), 5);
    rectangle(debug, Point(lineStart[i].x-WINDOW_WIDTH/2, lineStart[i].y-WINDOW_HEIGHT/2), Point(lineStart[i].x+WINDOW_WIDTH/2, lineStart[i].y+WINDOW_HEIGHT/2),
              Scalar(255, 0, 0), 5);
  }
  namedWindow("Windows", WINDOW_NORMAL);
  imshow("Windows", debug);
  waitKey(30);
}

void updateLines();
