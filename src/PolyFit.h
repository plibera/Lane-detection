#ifndef POLYFIT_H
#define POLYFIT_H

#include <opencv2/opencv.hpp>
#include "Polynomial.h"


class PolyFit
{
  std::vector<cv::Point> input;
  Polynomial <double> output;

public:
  PolyFit();
  PolyFit(std::vector<cv::Point> in);
  ~PolyFit();

  void setInput(std::vector<cv::Point> in);

  Polynomial<double> solve();
};


#endif
