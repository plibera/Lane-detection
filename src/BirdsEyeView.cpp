#include "BirdsEyeView.h"


using namespace cv;
using namespace std;

BirdsEyeView::BirdsEyeView()
{
    inputSet = 0;
}

BirdsEyeView::BirdsEyeView(Mat input)
{
    BirdsEyeView();
    setInput(input);
}

BirdsEyeView::~BirdsEyeView()
{

}

BirdsEyeView::setInput(Mat input)
{
    src = input;
    inputSet = 1;
    roi.setBoundaries(src.cols, src.rows);
    transformed = Mat::zeros(src.cols, src.rows, src.type);
    result = Mat::zeros(src.cols, src.rows, src.type);
    performTransform();
}

Mat BirdsEyeView::getTransformed()
{
    return transformed;
}

Mat BirdsEyeView::getResult()
{
    return result;
}

void BirdsEyeView::setRoi(vector<Point> roiCorners)
{
    roi.setBoundaries(src.cols, src.rows);
    roi.clear();
    roi.push_back(roiCorners);
}


void BirdsEyeView::performTransform()
{
//actual opencv stuff
}

void BirdsEyeView::calibrate()
{
  //find lines, four points, use class FourPoints - find intersection
}
