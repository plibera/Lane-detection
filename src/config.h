#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>


class Config
{
  int rotation;
  std::vector<cv::Point> lanePoints;
  std::string configFilename;

public:
  Config(std::string file = "../config.xml");

  bool load(std::string vidName);
  void setConfigFile(std::string file);

  int getRotation();
  std::vector<cv::Point> getPoints();

};


#endif
