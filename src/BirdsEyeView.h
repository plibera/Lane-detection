#ifndef BIRDSEYEVIEW_H
#define BIRDSEYEVIEW_H

#include <opencv2/opencv.hpp>
#include <stdio.h>


class BirdsEyeView
{
private:
    cv::Mat src;
    cv::Mat transformed;
    cv::Mat result;//transformed and binarized
    bool inputSet;

    FourPoints roi;//corners of the road roi

    void performTransform();

public:
    BirdsEyeView();
    BirdsEyeView(Mat input);
    ~BirdsEyeView();

    void setInput(Mat input);
    cv::Mat getTransformed();
    cv::Mat getResult();

    void setRoi(FourPoints newRoi);
    FourPoints getRoi();

    void calibrate();
};



#endif
