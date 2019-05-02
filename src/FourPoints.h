#ifndef FOURPOINTS_H
#define FOURPOINTS_H

#include <opencv2/opencv.hpp>
#include <stdio.h>

//Class containing many sets of four points
//Every coordinate must be between 0 and boundary
class FourPoints
{
    struct Points//group representation
    {
        cv::Point* point;
        int count;

        Points();
        ~Points();
    };
    
    std::vector< Points* > points;
    cv::Point boundary;

public:
    FourPoints(cv::Point bounds);
    FourPoints(std::vector<cv::Point> input, cv::Point bounds);
    FourPoints(cv::Point a, cv::Point b, cv::Point c, cv::Point d, cv::Point bounds);
    ~FourPoints();

    void push_back(std::vector<cv::Point> input);
    void push_back(cv::Point a, cv::Point b, cv::Point c, cv::Point d);

    std::vector<cv::Point> getPoints(int index);

    //removes points that were added last
    void pop_back();
    void clear();

    void setBoundaries(int x, int y);
    void setBoundaries(cv::Point newBoundary);
    cv::Point getBoundaries();
    bool withinBoundaries(cv::Point a);

    cv::Point polygonCentre(int index);//average of coordinates

    //choosePair sets with which point point number 0 is going to create a line with
    //default pairs are 0 with 3 and 1 with 2
    cv::Point linesIntersection(int index, int choosePair = 3);
};


#endif
