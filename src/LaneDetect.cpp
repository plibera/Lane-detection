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
    for(int j = 0; j < roadLines[i].size(); ++j)
    {
      roadLines[i][j].setInput(input);
    }
  }
  inputSet = 1;
}

vector<vector<double> > LaneDetect::getLineFits()
{
  return lineFits;
}

void LaneDetect::initLines()
{
  if(!inputSet)
    return;

  roadLines.clear();
  lineFits.clear();
  Window roi(input, Point(0, input.rows/2), Point(input.rows-1, input.cols-1));
  vector<int> histogram = roi.getHistogram();
  vector<int> newcenters;

  int sumThreshold = 1000;
  //for(int x = WINDOW_WIDTH/2; x < input.cols - WINDOW_WIDTH/2; ++x)
  for(int x = 2*WINDOW_WIDTH; x < input.cols - WINDOW_WIDTH/2; ++x)
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
      newcenters.push_back(x);
    }
    x += WINDOW_WIDTH-1;
  }

  vector<Point> lineStart;
  for(int i = 0; i < newcenters.size(); ++i)
  {
    Window w(input, Point(newcenters[i], input.rows-WINDOW_HEIGHT/2), WINDOW_WIDTH, WINDOW_HEIGHT);
    histogram = w.getHistogram();
    for(int j = 0; j < histogram.size(); ++j)
    {
      if(histogram[j] > 0)
      {
        lineStart.push_back(Point(newcenters[i], input.rows-WINDOW_HEIGHT/2));
        break;
      }
    }
  }

  vector<int> histPrefixSum;
  Point currentcenter;
  int lookupVal;
  int middleIndex;
  vector<Window> currentLine;
  for(int i = 0; i < lineStart.size(); ++i)
  {
    currentLine.clear();
    Window a(input, Point(0, 0), WINDOW_WIDTH, WINDOW_HEIGHT);
    currentcenter = lineStart[i];
    for(int windowNum = 0; windowNum < WINDOWS; ++windowNum)
    {
      a.setCenter(currentcenter);
      histogram = a.getHistogram();

      histPrefixSum.clear();
      histPrefixSum.push_back(histogram[0]);
      for(int j = 1; j < histogram.size(); ++j)
      {
        histPrefixSum.push_back(histPrefixSum[j-1] + histogram[j]);
      }
      lookupVal = histPrefixSum[histPrefixSum.size()-1] / 2;
      middleIndex = histPrefixSum.size()-1;
      for(int j = 0; j < histPrefixSum.size(); ++j)
      {
        if(histPrefixSum[j] > lookupVal)
        {
          middleIndex = j;
          break;
        }
      }
      if(histPrefixSum[histPrefixSum.size()-1] == 0)
        middleIndex = WINDOW_WIDTH/2;
      currentcenter.x = currentcenter.x - WINDOW_WIDTH/2 + middleIndex;
      a.setCenter(currentcenter);
      currentLine.push_back(a);
      currentcenter.y -= WINDOW_HEIGHT;
    }
    roadLines.push_back(currentLine);
  }


  /*Mat debug;
  cvtColor(input, debug, COLOR_GRAY2BGR);
  for(int i = 0; i < roadLines.size(); ++i)
  {
    //rectangle(debug, Point(newcenters[i]-WINDOW_WIDTH/2, 430), Point(newcenters[i]+WINDOW_WIDTH/2, 480), Scalar(255, 0, 0), 5);
    //rectangle(debug, Point(lineStart[i].x-WINDOW_WIDTH/2, lineStart[i].y-WINDOW_HEIGHT/2), Point(lineStart[i].x+WINDOW_WIDTH/2, lineStart[i].y+WINDOW_HEIGHT/2),
      //        Scalar(255, 0, 0), 5);
    for(int j = 0; j < roadLines[i].size(); ++j)
    {
      Point a, b, c;
      c = roadLines[i][j].center();
      a.x = c.x - WINDOW_WIDTH/2;
      a.y = c.y - WINDOW_HEIGHT/2;
      b.x = c.x + WINDOW_WIDTH/2;
      b.y = c.y + WINDOW_HEIGHT/2;
      rectangle(debug, a, b, Scalar(255, 0, 0), 5);
    }
  }
  namedWindow("Windows", WINDOW_NORMAL);
  imshow("Windows", debug);*/
}

void LaneDetect::updateLines()
{

  Mat debug;
  cvtColor(input, debug, COLOR_GRAY2BGR);

  int blackWindowIdx = WINDOWS;
  vector<int> histogram;
  vector<int>histPrefixSum;
  Point currentcenter;
  int lookupVal;
  int middleIndex;

  lineFits.clear();
  for(int i = 0; i < roadLines.size(); ++i)
  {
    blackWindowIdx = WINDOWS;
    histogram.clear();
    histPrefixSum.clear();
    for(int windowNum = 0; windowNum < roadLines[i].size(); ++windowNum)
    {
      histogram = roadLines[i][windowNum].getHistogram();
      histPrefixSum.clear();
      histPrefixSum.push_back(histogram[0]);
      for(int j = 1; j < histogram.size(); ++j)
      {
        histPrefixSum.push_back(histPrefixSum[j-1] + histogram[j]);
      }

      if(histPrefixSum[histPrefixSum.size()-1] == 0)
      {
        blackWindowIdx = windowNum;
        break;
      }

      lookupVal = histPrefixSum[histPrefixSum.size()-1] / 2;
      middleIndex = histPrefixSum.size()-1;
      for(int j = 0; j < histPrefixSum.size(); ++j)
      {
        if(histPrefixSum[j] > lookupVal)
        {
          middleIndex = j;
          break;
        }
      }
      currentcenter = roadLines[i][windowNum].center();
      int old = currentcenter.x;
      currentcenter.x = currentcenter.x - WINDOW_WIDTH/2 + middleIndex;

      /*if(roadLines.size() == 2)
      {
        Point border;
        if(i==0)
        {
          border = roadLines[1][windowNum].center();
          currentcenter.x = min(currentcenter.x, border.x-WINDOW_WIDTH/2);
        }
        if(i==1)
        {
          border = roadLines[0][windowNum].center();
          currentcenter.x = max(currentcenter.x, border.x+WINDOW_WIDTH/2);
        }
        //currentcenter.x = min(input.cols-1-WINDOW_WIDTH/2, max(WINDOW_WIDTH/2, currentcenter.x));
      }*/
      if(currentcenter.x < old - WINDOW_WIDTH/8) currentcenter.x = old-WINDOW_WIDTH/8;
      if(currentcenter.x > old + WINDOW_WIDTH/8) currentcenter.x = old+WINDOW_WIDTH/8;

      roadLines[i][windowNum].setCenter(currentcenter);
    }

    vector<Point> lanePoints;
    for(int j = 0; j < blackWindowIdx; ++j)
    {
      Point a = roadLines[i][j].center();
      swap(a.x, a.y);
      lanePoints.push_back(a);
    }

    PolyFit polyfit(lanePoints);
    vector<double> laneSolution = polyfit.solve();


    for(int j = blackWindowIdx; j < roadLines[i].size(); ++j)
    {
      Point a = roadLines[i][j].center();
      a.x = value(a.y, laneSolution);
      roadLines[i][j].setCenter(a);
    }
    lineFits.push_back(laneSolution);

    for(int y = 0; y < input.rows; ++y)
    {
      double x = value(y, laneSolution);
      if(x >= 0 && x < input.cols) debug.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 255);
    }
    for(int j = 0; j < roadLines[i].size(); ++j)
    {
      Point a, b, c;
      c = roadLines[i][j].center();
      a.x = c.x - WINDOW_WIDTH/2;
      a.y = c.y - WINDOW_HEIGHT/2;
      b.x = c.x + WINDOW_WIDTH/2;
      b.y = c.y + WINDOW_HEIGHT/2;
      rectangle(debug, a, b, Scalar(255, 0, 0), 5);
    }
  }
  namedWindow("PolyFitted", WINDOW_NORMAL);
  resizeWindow("PolyFitted", 1080, 720);
  if(!debug.empty()) imshow("PolyFitted", debug);
}
