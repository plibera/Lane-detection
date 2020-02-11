#include "Window.h"

using namespace std;
using namespace cv;

Window::Window(Mat image, Point center, int w, int h)
{
    src = image;
    srcLoaded = true;
    w = max(0, w);
    h = max(0, h);
    topLeft = Point(center.x - w/2, center.y - h/2);
    bottomRight = Point(topLeft.x + w, topLeft.y + h);
    createHistogram();
}

Window::Window(Point center, int w, int h)
{
    srcLoaded = false;
    w = max(0, w);
    h = max(0, h);
    topLeft = Point(center.x - w/2, center.y - h/2);
    bottomRight = Point(topLeft.x + w, topLeft.y + h);
    createHistogram();
}

Window::Window(Mat image, Point a, Point b)
{
    src = image;
    srcLoaded = true;
    topLeft = Point(min(a.x, b.x), min(a.y, b.y));
    bottomRight = Point(max(a.x, b.x), max(a.y, b.y));
    createHistogram();
}

Window::Window(Point a, Point b)
{
    srcLoaded = false;
    topLeft = Point(min(a.x, b.x), min(a.y, b.y));
    bottomRight = Point(max(a.x, b.x), max(a.y, b.y));
    createHistogram();
}

Window::~Window()
{}

bool Window::insideSrc(int x, int y)
{
  if(!srcLoaded)
    return false;
  return x >= 0 && x < src.cols && y >= 0 && y < src.rows;
}

bool Window::insideSrc(Point p)
{
  if(!srcLoaded)
    return false;
  return p.x >= 0 && p.x < src.cols && p.y >= 0 && p.y < src.rows;
}

void Window::createHistogram()
{
  if(!srcLoaded)
    return;
  int count = 0;
  histogram.clear();
  for(int x = topLeft.x; x <= bottomRight.x; ++x)
  {
    count = 0;
    for(int y = topLeft.y; y <= bottomRight.y; ++y)
    {
      if(!insideSrc(x, y))
        continue;
      if(src.at<uchar>(Point(x, y)) == 255)
      {
        count++;
      }
    }
    histogram.push_back(count);
  }
}

vector<int> Window::getHistogram()
{
  return histogram;
}

void Window::setCenter(cv::Point center)
{
  int h = height();
  int w = width();
  topLeft = Point(center.x - w/2, center.y - h/2);
  bottomRight = Point(topLeft.x + w, topLeft.y + h);
  createHistogram();
}

void Window::setInput(Mat image)
{
  src = image;
  srcLoaded = true;
  createHistogram();
}

int Window::height()
{
  return bottomRight.y - topLeft.y;
}

int Window::width()
{
  return bottomRight.x - topLeft.x;
}

cv::Point Window::center()
{
  return Point((bottomRight.x+topLeft.x)/2, (bottomRight.y+topLeft.y)/2);
}

cv::Point Window::getTopLeft()
{
  return topLeft;
}

cv::Point Window::getBottomRight()
{
  return bottomRight;
}
