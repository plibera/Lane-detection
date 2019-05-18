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
      a.push_back(Point(currentCentre.x-WINDOW_WIDTH/2, 430-windowNum*WINDOW_HEIGHT), Point(currentCentre.x+WINDOW_WIDTH/2, 430-windowNum*WINDOW_HEIGHT),
                  Point(currentCentre.x+WINDOW_WIDTH/2, 480-windowNum*WINDOW_HEIGHT), Point(currentCentre.x-WINDOW_WIDTH/2, 480-windowNum*WINDOW_HEIGHT));
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

      /*if(i==1 && windowNum == 5)
      {
        for(int j = 0; j < histogram.size(); ++j)
        {
          cout<<histogram[j]<<" ";
        }
        cout<<endl;
        for(int j = 0; j < histPrefixSum.size(); ++j)
        {
          cout<<histPrefixSum[j]<<" ";
        }
        cout<<endl;
        cout<<"Lookup: "<<lookupVal<<endl;
        cout<<"middle: "<<middleIndex<<endl;
        cout<<"centre x: "<<currentCentre.x<<endl;
      }*/
    }
    roadLines.push_back(a);
  }


  Mat debug;
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
  imshow("Windows", debug);
}

void updateLines();
