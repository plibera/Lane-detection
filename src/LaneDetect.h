#ifndef LANEDETECT_H
#define LANEDETECT_H

#include <stdio.h>
#include <vector>
#include "Window.h"
#include "PolyFit.h"


#define WINDOW_WIDTH 100
#define WINDOW_HEIGHT 50
#define WINDOWS 9

class LaneDetect
{
  cv::Mat input;
  bool inputSet;
  std::vector<std::vector<Window> > roadLines;
  std::vector<std::vector<double> > lineFits;

public:
  LaneDetect();
  LaneDetect(cv::Mat in);

  void setInput(cv::Mat in);
  std::vector<std::vector<double> > getLineFits();

  void initLines();
  void updateLines();
};


#endif
