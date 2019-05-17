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

FourPoints::FourPoints()
{
    Point a(0,0);
    setBoundaries(a);
}

FourPoints::FourPoints(Point bounds)
{
    setBoundaries(bounds);
}

FourPoints::FourPoints(vector<Point> input, Point bounds)
{
    setBoundaries(bounds);
    push_back(input);
}

FourPoints::FourPoints(Point a, Point b, Point c, Point d, Point bounds)
{
    setBoundaries(bounds);
    vector<Point> temp;
    temp.push_back(a);
    temp.push_back(b);
    temp.push_back(c);
    temp.push_back(d);
    push_back(temp);
}

FourPoints::~FourPoints()
{
//  cout<<"FourPoints destructor"<<endl;
    while(points.size() > 0)
    {
        delete points[points.size()-1];
        points.pop_back();
    }
}

void FourPoints::push_back(Point a, Point b, Point c, Point d)
{
    vector<Point> temp;
    temp.push_back(a);
    temp.push_back(b);
    temp.push_back(c);
    temp.push_back(d);
    push_back(temp);
}

void FourPoints::push_back(vector<Point> input)
{
    if(input.size() != 4)
        return;

    for(int i = 0; i < 4; ++i)
    {
       if(!withinBoundaries(input[i]))
       {
           input[i].x = min(boundary.x, max(0, input[i].x));
           input[i].y = min(boundary.y, max(0, input[i].y));
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

vector<Point> FourPoints::getPoints(int index)
{
    index = max(0, min((int)points.size()-1, index));
    vector<Point> a;
    for(int i = 0; i < 4; ++i)
        a.push_back(points[index]->point[i]);

    return a;
}

void FourPoints::pop_back()
{
    if(points.size() == 0)
        return;
    if(points[points.size()-1]->count > 0)
        points[points.size()-1]->count--;
    else
        delete points[points.size()-1];
    points.pop_back();
}

void FourPoints::clear()
{
    while(!points.empty())
    {
        pop_back();
    }
}

int FourPoints::size()
{
    return (int)points.size();
}

void FourPoints::setBoundaries(int x, int y)
{
    Point a(x, y);
    setBoundaries(a);
}

void FourPoints::setBoundaries(Point newBoundary)
{
    newBoundary.x = max(0, newBoundary.x-1);
    newBoundary.y = max(0, newBoundary.y-1);
    boundary = newBoundary;
    for(size_t i = 0; i < points.size(); ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            points[i]->point[j].x = min(boundary.x, max(0, points[i]->point[j].x));
            points[i]->point[j].y = min(boundary.y, max(0, points[i]->point[j].y));
        }
    }
}

Point FourPoints::getBoundaries()
{
    return boundary;
}

bool FourPoints::withinBoundaries(Point a)
{
    return a.x >= 0 && a.x <= boundary.x && a.y >= 0 && a.y <= boundary.y;
}

Point FourPoints::polygonCentre(int index)
{
    index = max(0, min((int)points.size()-1, index));
    Point result(0,0);
    for(int j = 0; j < 4; ++j)
    {
        result.x += points[index]->point[j].x;
        result.y += points[index]->point[j].y;
    }
    result.x /= 4;
    result.y /= 4;

    return result;
}

Point FourPoints::linesIntersection(int index, int choosePair)
{
    index = max(0, min((int)points.size()-1, index));
    if(choosePair != 1 && choosePair != 2)
        choosePair = 3;
    Point chosen[4];
    for(int j = 0; j < 4; ++j)
    {
        chosen[j] = points[index]->point[j];
    }
    if(choosePair != 3)
    {
        Point temp;
        temp = chosen[3];
        chosen[3] = chosen[choosePair];
        chosen[choosePair] = temp;
    }

    int a, b, c, d;
    a = b = c = d = 0;

    a = chosen[0].x - chosen[3].x;
    b = chosen[0].y - chosen[3].y;
    c = chosen[1].x - chosen[2].x;
    d = chosen[1].y - chosen[2].y;

    if(a*d == c*b)
    {
        return Point(-1, -1);
    }
    int k = (d*(chosen[2].x-chosen[3].x)+c*(chosen[3].y-chosen[2].y))/(a*d-c*b);
    int l = (b*(chosen[2].x-chosen[3].x)+a*(chosen[3].y-chosen[2].y))/(a*d-c*b);

    Point result(chosen[3].x + k*a, chosen[3].y + k*b);

    return result;
}
