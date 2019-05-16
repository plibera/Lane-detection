#ifndef WINDOW_H
#define WINDOW_H

#include "FourPoints.h"


class Window : public FourPoints
{
  cv::Mat src;
  cv::Mat hist;//Mat to show a resulting image
  std::vector< std::vector<int> > histograms;//vector storing histograms for every window

public:
  Window(cv::Mat image);
  Window(cv::Mat image, std::vector<cv::Point> input);
  Window(cv::Mat image, cv::Point a, cv::Point b, cv::Point c, cv::Point d);
  ~Window();

  virtual cv::Point polygonCentre(int index);//the centre of the window

  void createHistograms();
  void createHistogram(int index);

  std::vector<int> getHistogram(int index);
  std::vector<std::vector<int> > getHistograms();
  cv::Mat histToImg(int index = 0);

  void setWindowCentre(int index, cv::Point centre);
  void setWindowCentres(std::vector<cv::Point> centres);
  std::vector<cv::Point> getWindowCentres();
  cv::Point getWindowCentre(int index);

  void setInput(cv::Mat image);
};

#endif
