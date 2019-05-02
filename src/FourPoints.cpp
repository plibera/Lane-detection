#include "FourPoints.h"

using namespace cv;
using namespace std;


FourPoints::Points::Points()
{
    point = new Point[4];
    count = 0;
}

FourPoints::Points::~Points()
{
    delete[] point;
}


FourPoints::FourPoints(Point bounds)
{
    setBoundaries(bounds);
}

FourPoints::FourPoints(vector<Point> input, Point bounds)
{
    setBoundaries(bounds);
    add(input);
}

FourPoints::FourPoints(Point a, Point b, Point c, Point d, Point bounds)
{
    setBoundaries(bounds);
    vector<Point> temp;
    temp.push_back(a);
    temp.push_back(b);
    temp.push_back(c);
    temp.push_back(d);
    add(temp);
}

FourPoints::~FourPoints()
{
    while(points.size() > 0)
    {
        delete points[points.size()-1];
        points.pop_back();
    }
}

void FourPoints::add(Point a, Point b, Point c, Point d)
{
    vector<Point> temp;
    temp.push_back(a);
    temp.push_back(b);
    temp.push_back(c);
    temp.push_back(d);
    add(temp);
}

void FourPoints::add(vector<Point> input)
{
    if(input.size() != 4)
        return;

    for(int i = 0; i < 4; ++i)
    {
       if(!withinBoundaries(input[i]))
       {
           input[i].x = min(boundary.x, max(0, input[i].x));
           input[i].y = min(bpundary.y, max(0, input[i].y));
       }
    }

    int sameFour = -1;
    for(size_t i = 0; i < points.size(); ++i)
    {
        bool same = 1;
        for(int j = 0; j < 4; ++j)
        {
            if(points[i]->point[j] != input[j])
                same = 0;
        }
        if(same)
            sameFour = i;
    }
    if(sameFour > -1)
    {
        points[sameFour]->count++;
        points.push_back(points[sameFour]);
    }
    else
    {
        Points* a = new Points;
        for(int i = 0; i < 4; ++i)
        {
            a->point[i] = input[i];
        }
        points.push_back(a);
    }
}

vector<Point> getPoints(int index)
{
    index = max(0, min(points.size()-1, index));
    vector<Point> a;
    for(int i = 0; i < 4; ++i)
        a.push_back(points[index]->point[0]);

    return a;
}
