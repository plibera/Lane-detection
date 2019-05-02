#include "BirdsEyeView.h"


BirdsEyeView::BirdsEyeView()
{
    inputSet = 0;
}

BirdsEyeView::BirdsEyeView(cv::Mat input)
{
    BirdsEyeView();
    setInput(input);
}

BirdsEyeView::~BirdsEyeView()
{

}

BirdsEyeView::setInput(cv::Mat input)
{
    src = input;
    inputSet = 1;
    roi.setBoundaries(src.cols, src.rows);
    performTransform();
}

cv::Mat BirdsEyeView::getTransformed()
{
    return transformed;
}

cv::Mat BirdsEyeView::getResult()
{
    return result;
}

void BirdsEyeView::setRoi(FourPoints newRoi)
{
    newRoi.setBoundaries(src.cols, src.rows);
    roi = newRoi;
}

FourPoints BirdsEyeView::getRoi()
{
    return roi;
}


void BirdsEyeView::performTransform()
{

}
