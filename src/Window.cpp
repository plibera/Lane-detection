#include "Window.h"

using namespace std;
using namespace cv;

Window::Window(Mat image)
{
    src = image;
    FourPoints(Point(src.cols, src.rows));
    hist = Mat::zeros(src.rows, src.cols, src.type());
}

Window::Window(Mat image, vector<Point> input)
{
    src = image;
    hist = Mat::zeros(src.rows, src.cols, src.type());
    FourPoints(input, Point(src.rows, src.cols));
}

Window::Window(Mat image, Point a, Point b, Point c, Point d)
{
    src = image;
    hist = Mat::zeros(src.rows, src.cols, src.type());
    FourPoints(a, b, c, d, Point(src.rows, src.cols));
}

Window::~Window()
{
  //cout<<"Window destructor"<<endl;
}

Point Window::polygonCentre(int index)
{
  index = max(0, min((int)points.size()-1, index));
  Point result(0,0);
  result.x = (points[index]->point[0].x + points[index]->point[2].x) / 2;
  result.y = (points[index]->point[0].y + points[index]->point[2].y) / 2;

  return result;
}

void Window::createHistograms()
{
  vector <int> v;
  histograms.clear();
  for(unsigned i = 0; i < points.size(); ++i)
  {
    histograms.push_back(v);
    createHistogram(i);
  }
}

void Window::createHistogram(int index)
{
  if(index < 0 || index >= histograms.size())
    return;

  int count = 0;
  for(int x = 0; x < src.cols; ++x)
  {
    count = 0;
    for(int y = 0; y < src.rows; ++y)
    {
      if(src.at<uchar>(Point(x, y)) == 255)
        count++;
    }
    histograms[index].push_back(count);
  }
}

vector<int> Window::getHistogram(int index)
{
  index = max(0, min((int)histograms.size()-1, index));
  vector<int> v = histograms[index];
  return v;
}

vector<vector<int> > Window::getHistograms()
{
  vector<vector<int> > v = histograms;
  return v;
}

Mat Window::histToImg(int index)
{
  index = max(0, min((int)histograms.size()-1, index));

  int maxVal = *max_element(histograms[index].begin(), histograms[index].end());
  maxVal = max(1, maxVal);
  int limit = hist.rows;

  vector<int> valuesToShow;
  for(unsigned int i = 0; i < histograms[index].size(); ++i)
  {
    valuesToShow.push_back(histograms[index][i]*limit / maxVal);
    valuesToShow[i] = limit - valuesToShow[i];
  }
  hist = Mat::zeros(limit, histograms[index].size(), src.type());
  for(unsigned int x = 0; x < hist.cols; ++x)
  {
    for(unsigned int y = 0; y < hist.rows; ++y)
    {
      if(y <= valuesToShow[x])
        hist.at<uchar>(Point(x, y)) = 0;
      else
        hist.at<uchar>(Point(x, y)) = 255;
    }
  }

  return hist;
}

void Window::setWindowCentres(vector<cv::Point> centres)
{
  for(int i = 0; i < min(centres.size(), points.size()); ++i)
  {
    setWindowCentre(i, centres[i]);
  }
}

void Window::setWindowCentre(int index, cv::Point centre)
{
  if(index < 0 || index >= points.size())
    return;

  int width = points[index]->point[2].x - points[index]->point[0].x;
  int height = points[index]->point[2].y - points[index]->point[0].y;

  if(centre.x - width/2 < 0) centre.x = width/2;
  if(centre.x + width/2 >= src.cols) centre.x = src.cols-1-width/2;
  if(centre.y - height/2 < 0) centre.y = height/2;
  if(centre.y + height/2 >= src.rows) centre.y = src.rows-1-height/2;

  points[index]->point[0].x = centre.x - width/2;
  points[index]->point[1].x = centre.x + width/2;
  points[index]->point[2].x = centre.x + width/2;
  points[index]->point[3].x = centre.x - width/2;
  points[index]->point[0].y = centre.y - height/2;
  points[index]->point[1].y = centre.y - height/2;
  points[index]->point[2].y = centre.y + height/2;
  points[index]->point[3].y = centre.y + height/2;
}

vector<Point> Window::getWindowCentres()
{
  vector<Point> v;
  for(unsigned int i = 0; i < points.size(); ++i)
  {
    v.push_back(this->polygonCentre(i));
  }
}

Point Window::getWindowCentre(int index)
{
  index = max(0, min((int)histograms.size()-1, index));
  return this->polygonCentre(index);
}

void Window::setInput(Mat image)
{
  src = image;
  hist = Mat::zeros(src.rows, src.cols, src.type());
  setBoundaries(Point(src.cols, src.rows));
}
