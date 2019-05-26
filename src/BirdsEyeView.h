#ifndef BIRDSEYEVIEW_H
#define BIRDSEYEVIEW_H

#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <stdio.h>
#include "FourPoints.h"

#define THRESHOLD 100

class BirdsEyeView
{
private:
    cv::Mat src;
    cv::Mat transformed;
    cv::Mat result;//transformed and binarized
    cv::Mat unwarped;
    bool inputSet;

    FourPoints roi;//corners of the road roi

    void performTransform();

public:
    BirdsEyeView();
    BirdsEyeView(cv::Mat input);
    ~BirdsEyeView();

    void setInput(cv::Mat input);
    cv::Mat getTransformed();
    cv::Mat getResult();
    cv::Mat getUnwarped();

    //Only set roi if you have accurate coordinates and/or there aren't any straight
    //lines to perform calibration on
    void setRoi(std::vector<cv::Point> roiCorners);

    //void calibrate();
};



#endif
