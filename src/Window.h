#ifndef WINDOW_H
#define WINDOW_H

#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>

class Window
{
  cv::Mat src;
  bool srcLoaded;
  std::vector<int> histogram;
  cv::Point topLeft;
  cv::Point bottomRight;

  bool insideSrc(int x, int y);
  bool insideSrc(cv::Point p);
  void createHistogram();

public:
  Window(cv::Point center = cv::Point(0, 0), int w = 0, int h = 0);
  Window(cv::Mat image, cv::Point center = cv::Point(0, 0), int w = 0, int h = 0);
  Window(cv::Point a, cv::Point b);
  Window(cv::Mat image, cv::Point a, cv::Point b);
  ~Window();

  std::vector<int> getHistogram();

  void setCenter(cv::Point center);

  void setInput(cv::Mat image);

  int height();
  int width();
  cv::Point center();
  cv::Point getTopLeft();
  cv::Point getBottomRight();
};

#endif
