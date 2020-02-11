#include <stdio.h>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "BirdsEyeView.h"
#include "Window.h"
#include "PolyFit.h"
#include "LaneDetect.h"
#include "config.h"

using namespace cv;
using namespace std;


int main(int argc, char** argv )
{
    if(argc < 2)
    {
        cout<<"Input not specified"<<endl;
        return -1;
    }
    string vidName = argv[1];
    Config config;
    if(argc > 2)
    {
      config.setConfigFile(argv[2]);
    }
    if(!config.load(vidName))
      return 1;

    VideoCapture vid;
    vid.open(vidName);

    if(!vid.isOpened())
    {
        cout<<"Input not opened"<<endl;
        return -1;
    }

    Mat frame, birdView, histogram;

    BirdsEyeView bird;
    vid >> frame;
    if(config.getRotation() == 90)
      rotate(frame, frame, ROTATE_90_CLOCKWISE);
    else if(config.getRotation() == 180)
      rotate(frame, frame, ROTATE_180);
    else if(config.getRotation() == 270)
      rotate(frame, frame, ROTATE_90_COUNTERCLOCKWISE);
    bird.setInput(frame);
    bird.setRoi(config.getPoints());
    birdView = bird.getResult();

    cout<<"Frame resolution: "<<frame.cols<<" "<<frame.rows<<endl;

    bird.setInput(frame);
    birdView = bird.getResult();

    LaneDetect lanes;

    lanes.setInput(birdView);
    lanes.initLines();

    int frame_id = 0;
    vector<vector<double> > polyLines;
    Mat result, output, fin;
    result = Mat::zeros(frame.rows, frame.rows, CV_8UC3);
    output = Mat::zeros(frame.rows, frame.rows, CV_8UC3);
    while(!frame.empty())
    {
        //imshow("Frame", frame);
        bird.setInput(frame);
        birdView = bird.getResult();

        lanes.setInput(birdView);
        lanes.updateLines();

        polyLines = lanes.getLineFits();

        if(polyLines[0][0] == 0 && polyLines[0][1] == 0 && polyLines[0][2] == 0)
        {
          lanes.initLines();
        }

        result = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
        int x;
        for(int y = 0; y < result.rows; ++y)
        {
          for(int i = 0; i < polyLines.size(); ++i)
          {
            x = value(y, polyLines[i]);
            if(x >= 0 && x < result.cols)
              result.at<Vec3b>(Point(x, y)) = Vec3b(100, 100, 100);
          }
        }

        bird.setInput(result);
        result = bird.getUnwarped();
        cvtColor(result, output, CV_GRAY2BGR);
        fin = frame;
        for(int x = 0; x < output.cols; ++x)
        {
          for(int y = 0; y < output.rows; ++y)
          {
            if(y > frame.rows - WINDOWS*WINDOW_HEIGHT)
            {
              Vec3b pixelVal = output.at<Vec3b>(Point(x, y));
              if(pixelVal.val[0] > 0)
                frame.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 255);
            }
          }
        }
        imshow("Final", fin);

        char c = (char)waitKey(30);
        if(c == 27)//break when user presses ESC
            break;

        vid >> frame;
        if(config.getRotation() == 90)
          rotate(frame, frame, ROTATE_90_CLOCKWISE);
        else if(config.getRotation() == 180)
          rotate(frame, frame, ROTATE_180);
        else if(config.getRotation() == 270)
          rotate(frame, frame, ROTATE_90_COUNTERCLOCKWISE);
    }
    return 0;
}
