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

void BirdsEyeView::setInput(Mat input)
{
    if(input.channels() > 1)
    {
         cvtColor(input, input, COLOR_BGR2GRAY);//causes leaks
    }
    src = input;
    inputSet = 1;
    roi.setBoundaries(src.cols, src.rows);
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
    roi.setBoundaries(src.cols, src.rows);
    roi.clear();
    roi.push_back(roiCorners);
}


void BirdsEyeView::performTransform()
{
    if(roi.size() == 0)
        return;
    Point2f srcver[4];
    vector<Point> roiPoints = roi.getPoints(0);
    for(int j = 0; j < 4; ++j)
    {
        srcver[j] = roiPoints[j];
    }
    Point2f dstver[4];
    dstver[0] = Point(2*src.cols/5, 0);//src.rows/4);
    dstver[1] = Point(3*src.cols/5, 0);//src.rows/4);
    dstver[2] = Point(3*src.cols/5, src.rows);
    dstver[3] = Point(2*src.cols/5, src.rows);

    Mat M = getPerspectiveTransform(srcver, dstver);
    warpPerspective(src, transformed, M, transformed.size(), INTER_LINEAR, BORDER_CONSTANT);//causes leaks
  //  namedWindow("transformed", WINDOW_NORMAL);//causes leaks
    //imshow("transformed", transformed);//causes leaks

    threshold( transformed, result, THRESHOLD, 255, 0 );//causes possible leaks
    //namedWindow("result", WINDOW_NORMAL);//causes leaks
    //imshow("result", result);//causes leaks

    M = getPerspectiveTransform(dstver, srcver);
    warpPerspective(src, unwarped, M, unwarped.size(), INTER_LINEAR, BORDER_CONSTANT);//causes leaks
    //namedWindow("unwarped", WINDOW_NORMAL);//causes leaks
    //imshow("unwarped", unwarped);//causes leaks
}

void BirdsEyeView::calibrate()
{
  //find lines, four points, use class FourPoints - find intersection
    Mat edges;
    Canny(src, edges, 50, 200, 3);
    vector<Vec4i> lines;
    HoughLinesP( edges, lines, 1, CV_PI/180, 80, 30, 10 );

    vector <Vec4i> chosenLines;
    int y = src.rows*2 / 3;
    int xa = src.cols/4;
    int xb = src.cols*3/4;
    for(size_t i = 0; i < lines.size(); ++i)
    {
        if((lines[i][0]-lines[i][2])*(lines[i][0]-lines[i][2]) < 3*(lines[i][1]-lines[i][3])*(lines[i][1]-lines[i][3]) &&
            lines[i][1] > y && lines[i][3] > y && lines[i][0] > xa && lines[i][2] > xa &&
            lines[i][0] < xb && lines[i][2] < xb)
            chosenLines.push_back(lines[i]);
    }
    Mat show;
    src.copyTo(show);
    for(size_t i = 0; i < chosenLines.size(); ++i)
    {
      line( show, Point(chosenLines[i][0], chosenLines[i][1]),
          Point( chosenLines[i][2], chosenLines[i][3]), Scalar(128), 3, 8 );
    }
    namedWindow("lines", WINDOW_NORMAL);
    imshow("lines", show);
}
