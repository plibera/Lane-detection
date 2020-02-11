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

BirdsEyeView::~BirdsEyeView(){}


void BirdsEyeView::setInput(Mat input)
{
    if(input.channels() > 1)
    {
         cvtColor(input, input, COLOR_BGR2GRAY);
    }
    src = input;
    inputSet = 1;
    transformed = Mat::zeros(src.rows, src.cols, src.type());
    result = Mat::zeros(src.rows, src.cols, src.type());
    unwarped = Mat::zeros(src.rows, src.cols, src.type());
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

Mat BirdsEyeView::getUnwarped()
{
  return unwarped;
}

void BirdsEyeView::setRoi(vector<Point> roiCorners)
{
    if(roiCorners.size() >= 4)
      roi = roiCorners;
}


void BirdsEyeView::performTransform()
{
    if(roi.size() < 4)
        return;
    Point2f srcver[4];
    for(int j = 0; j < 4; ++j)
    {
        srcver[j] = roi[j];
    }
    Point2f dstver[4];
    dstver[0] = Point(2*src.cols/5, src.rows/4);
    dstver[1] = Point(3*src.cols/5, src.rows/4);
    dstver[2] = Point(3*src.cols/5, src.rows);
    dstver[3] = Point(2*src.cols/5, src.rows);

    Mat M = getPerspectiveTransform(srcver, dstver);
    warpPerspective(src, transformed, M, transformed.size(), INTER_LINEAR, BORDER_CONSTANT);
  //  namedWindow("transformed", WINDOW_NORMAL);
    //imshow("transformed", transformed);

    //adaptiveThreshold( transformed, result, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, -2);
    threshold( transformed, result, THRESHOLD, 255, 0 );
    //namedWindow("result", WINDOW_NORMAL);
    //imshow("result", result);

    M = getPerspectiveTransform(dstver, srcver);
    warpPerspective(src, unwarped, M, unwarped.size(), INTER_LINEAR, BORDER_CONSTANT);
    //namedWindow("unwarped", WINDOW_NORMAL);
    //imshow("unwarped", unwarped);
}
