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
    roi.push_back(Point(newCentres[i]-WINDOW_WIDTH/2, input.rows-WINDOW_HEIGHT), Point(newCentres[i]+WINDOW_WIDTH/2, input.rows-WINDOW_HEIGHT),
                  Point(newCentres[i]+WINDOW_WIDTH/2, input.rows), Point(newCentres[i]-WINDOW_WIDTH/2, input.rows));
    roi.createHistograms();
    histogram = roi.getHistogram(0);
    for(int j = 0; j < histogram.size(); ++j)
    {
      if(histogram[j] > 0)
      {
        lineStart.push_back(Point(newCentres[i], input.rows-WINDOW_HEIGHT/2));
        break;
      }
    }
  }

  vector<int> histPrefixSum;
  Point currentCentre;
  int lookupVal;
  int middleIndex;
  for(int i = 0; i < lineStart.size(); ++i)
  {
    Window a(input);
    currentCentre = lineStart[i];
    for(int windowNum = 0; windowNum < WINDOWS; ++windowNum)
    {
      a.push_back(Point(currentCentre.x-WINDOW_WIDTH/2, input.rows-(windowNum+1)*WINDOW_HEIGHT), Point(currentCentre.x+WINDOW_WIDTH/2, input.rows-(windowNum+1)*WINDOW_HEIGHT),
                  Point(currentCentre.x+WINDOW_WIDTH/2, input.rows-windowNum*WINDOW_HEIGHT), Point(currentCentre.x-WINDOW_WIDTH/2, input.rows-windowNum*WINDOW_HEIGHT));
      a.createHistograms();
      histogram = a.getHistogram(windowNum);

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
      currentCentre.x = currentCentre.x - WINDOW_WIDTH/2 + middleIndex;
      a.setWindowCentre(windowNum, currentCentre);
      currentCentre.y -= WINDOW_HEIGHT;
    }
    roadLines.push_back(a);
  }


  /*Mat debug;
  cvtColor(input, debug, COLOR_GRAY2BGR);
  for(int i = 0; i < roadLines.size(); ++i)
  {
    //rectangle(debug, Point(newCentres[i]-WINDOW_WIDTH/2, 430), Point(newCentres[i]+WINDOW_WIDTH/2, 480), Scalar(255, 0, 0), 5);
    //rectangle(debug, Point(lineStart[i].x-WINDOW_WIDTH/2, lineStart[i].y-WINDOW_HEIGHT/2), Point(lineStart[i].x+WINDOW_WIDTH/2, lineStart[i].y+WINDOW_HEIGHT/2),
      //        Scalar(255, 0, 0), 5);
    for(int j = 0; j < roadLines[i].size(); ++j)
    {
      Point a, b, c;
      c = roadLines[i].getWindowCentre(j);
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
  Point currentCentre;
  int lookupVal;
  int middleIndex;

  lineFits.clear();
  for(int i = 0; i < roadLines.size(); ++i)
  {
    roadLines[i].createHistograms();
    blackWindowIdx = WINDOWS;
    histogram.clear();
    histPrefixSum.clear();
    for(int windowNum = 0; windowNum < roadLines[i].size(); ++windowNum)
    {
      histogram = roadLines[i].getHistogram(windowNum);
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
      currentCentre = roadLines[i].getWindowCentre(windowNum);
      int old = currentCentre.x;
      currentCentre.x = currentCentre.x - WINDOW_WIDTH/2 + middleIndex;

      if(roadLines.size() == 2)
      {
        Point border;
        if(i==0)
        {
          border = roadLines[1].getWindowCentre(windowNum);
          currentCentre.x = min(currentCentre.x, border.x-WINDOW_WIDTH/2);
        }
        if(i==1)
        {
          border = roadLines[0].getWindowCentre(windowNum);
          currentCentre.x = max(currentCentre.x, border.x+WINDOW_WIDTH/2);
        }
        currentCentre.x = min(input.cols-1-WINDOW_WIDTH/2, max(WINDOW_WIDTH/2, currentCentre.x));
      }
      if(currentCentre.x < old - WINDOW_WIDTH/8) currentCentre.x = old-WINDOW_WIDTH/8;
      if(currentCentre.x > old + WINDOW_WIDTH/8) currentCentre.x = old+WINDOW_WIDTH/8;

      //if(windowNum <= 1)
        roadLines[i].setWindowCentre(windowNum, currentCentre);
      /*else
      {
        vector<Point> tempPoints;
        for(int j = 0; j < windowNum; ++j)
        {
          tempPoints.push_back(roadLines[i].getWindowCentre(j));
        }
        PolyFit tempFit(tempPoints);
        Polynomial<double> tempPoly = tempFit.solve();
        int x = tempPoly.value(currentCentre.y);
        if(x < 0 || x > input.cols)
        {
          blackWindowIdx = windowNum;
          break;
        }
        else
        {
          roadLines[i].setWindowCentre(windowNum, currentCentre);
        }
      }*/
    }

    vector<Point> lanePoints;
    for(int j = 0; j < blackWindowIdx; ++j)
    {
      Point a = roadLines[i].getWindowCentre(j);
      swap(a.x, a.y);
      lanePoints.push_back(a);
    }

    PolyFit polyfit(lanePoints);
    Polynomial<double> laneSolution = polyfit.solve();

  //  int outOfFrameIdx = roadLines[i].size();

    for(int j = blackWindowIdx; j < roadLines[i].size(); ++j)
    {
      Point a = roadLines[i].getWindowCentre(j);
      a.x = laneSolution.value(a.y);
    //  if(a.x >= 0 && a.x < input.cols)
        roadLines[i].setWindowCentre(j, a);
    /*  else
      {
        outOfFrameIdx = j;
        break;
      }*/
    }
    lineFits.push_back(laneSolution);

    /*while(roadLines[i].size() != outOfFrameIdx)
      roadLines[i].pop_back();

    Point newCentre;
    for(int j = roadLines[i].size(); j < WINDOWS; ++j)
    {
      newCentre.y = input.rows - j*WINDOW_HEIGHT + WINDOW_HEIGHT/2;
      newCentre.x = laneSolution.value(newCentre.y);
      if(newCentre.x >= 0 && newCentre.x < input.cols)
      {
        roadLines[i].push_back(Point(0, 0), Point(WINDOW_WIDTH, 0), Point(WINDOW_WIDTH, WINDOW_HEIGHT), Point(0, WINDOW_HEIGHT));
        roadLines[i].setWindowCentre(j, newCentre);
      }
      else
       break;
    }*/

    for(int y = 0; y < input.rows; ++y)
    {
      double x = laneSolution.value(y);
      if(x >= 0 && x < input.cols) debug.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 255);
    }
    for(int j = 0; j < roadLines[i].size(); ++j)
    {
      Point a, b, c;
      c = roadLines[i].getWindowCentre(j);
      a.x = c.x - WINDOW_WIDTH/2;
      a.y = c.y - WINDOW_HEIGHT/2;
      b.x = c.x + WINDOW_WIDTH/2;
      b.y = c.y + WINDOW_HEIGHT/2;
      rectangle(debug, a, b, Scalar(255, 0, 0), 5);
    }
  }
  namedWindow("PolyFitted", WINDOW_NORMAL);
  resizeWindow("PolyFitted", 1080, 720);
  imshow("PolyFitted", debug);
}
