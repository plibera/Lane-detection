#ifndef POLYFIT_H
#define POLYFIT_H

#include <opencv2/opencv.hpp>
#include <vector>

#define DEGREE 2

double value(int x, std::vector<double> coeffs);

class PolyFit
{
  std::vector<cv::Point> input;
  std::vector<double> output;

public:
  PolyFit();
  PolyFit(std::vector<cv::Point> in);
  ~PolyFit();

  void setInput(std::vector<cv::Point> in);

  std::vector<double> solve();
};


#endif
