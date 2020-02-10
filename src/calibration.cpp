#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "tinyxml.h"

#define STEP 30

using namespace std;
using namespace cv;

string configFilename = "../config.xml";

Mat frame;
Mat shadow;
Mat combined;

int bottomLeftXSlider;
int bottomRightXSlider;
int topYSlider;
int topLeftXSlider;
int topRightXSlider;

int frameWidth;
int frameHeight;

int rotation;
bool endOfVid = false;

static void trackbarCallback(int, void*);
void createNewConfig(string vidName);

int main(int argc, char **argv)
{
  if(argc < 2)
  {
    cout<<"No video name provided"<<endl;
    return 0;
  }
  string vidName;
  vidName = argv[1];

  VideoCapture vid;
  vid.open(vidName);
  if(!vid.isOpened())
  {
      cout<<"Input not opened"<<endl;
      return 0;
  }

  vid>>frame;
  frameWidth = frame.cols;
  frameHeight = frame.rows;

  TiXmlDocument doc(configFilename);
	bool loadOkay = doc.LoadFile();
  bool vidFound = false;
  TiXmlHandle hDoc(&doc);
  TiXmlElement* vidElem;
  TiXmlElement* pointElem;
	if(loadOkay)
	{
	  vidElem = hDoc.FirstChild("Calibration").FirstChild("Video").ToElement();
    while(vidElem)
    {
      if(vidName == vidElem->Attribute("filename"))
      {
        vidFound = true;
        break;
      }
      vidElem = vidElem->NextSiblingElement();
    }
    if(vidFound)
    {
      rotation = atoi(vidElem->Attribute("rotation"));
      if(rotation == 90)
        rotate(frame, frame, ROTATE_90_CLOCKWISE);
      else if(rotation == 180)
        rotate(frame, frame, ROTATE_180);
      else if(rotation == 270)
        rotate(frame, frame, ROTATE_90_COUNTERCLOCKWISE);
      pointElem = vidElem->FirstChildElement("Point");
      topYSlider = frameHeight - atoi(pointElem->Attribute("y"));
      topLeftXSlider = atoi(pointElem->Attribute("x"));
      pointElem = pointElem->NextSiblingElement();
      topRightXSlider = atoi(pointElem->Attribute("x"));
      pointElem = pointElem->NextSiblingElement();
      bottomRightXSlider = atoi(pointElem->Attribute("x"));
      pointElem = pointElem->NextSiblingElement();
      bottomLeftXSlider = atoi(pointElem->Attribute("x"));
    }
	}
  if(!loadOkay || !vidFound)
  {
    bottomLeftXSlider = frameWidth/3;
    bottomRightXSlider = 2*frameWidth/3;
    topYSlider = frameHeight/2;
    topLeftXSlider = 2*frameWidth/5;
    topRightXSlider = 3*frameWidth/5;
  }

  namedWindow("Calibration", WINDOW_NORMAL);

  createTrackbar("Top Y", "Calibration", &topYSlider, frameWidth, trackbarCallback);
  createTrackbar("Top left X", "Calibration", &topLeftXSlider, frameWidth, trackbarCallback);
  createTrackbar("Top right X", "Calibration", &topRightXSlider, frameWidth, trackbarCallback);
  createTrackbar("Bottom right X", "Calibration", &bottomRightXSlider, frameWidth, trackbarCallback);
  createTrackbar("Bottom left X", "Calibration", &bottomLeftXSlider, frameWidth, trackbarCallback);

  trackbarCallback(bottomLeftXSlider, 0);
  char c = waitKey(0);
  while(c == 'r' || c == 'n')
  {
    if(c == 'r')
    {
      rotation = (rotation+90)%360;
      rotate(frame, frame, ROTATE_90_CLOCKWISE);
      frameWidth = frame.cols;
      frameHeight = frame.rows;
      bottomLeftXSlider = frameWidth/3;
      bottomRightXSlider = 2*frameWidth/3;
      topYSlider = frameHeight/2;
      topLeftXSlider = 2*frameWidth/5;
      topRightXSlider = 3*frameWidth/5;
      trackbarCallback(bottomLeftXSlider, 0);
    }
    if(c == 'n' && !endOfVid)
    {
      Mat newFrame;
      for(int i = 0; i < STEP; ++i)
      {
        vid>>newFrame;
        if(newFrame.empty())
        {
          endOfVid = true;
          break;
        }
      }
      if(!endOfVid)
      {
        frame = newFrame;
        if(rotation == 90)
          rotate(frame, frame, ROTATE_90_CLOCKWISE);
        else if(rotation == 180)
          rotate(frame, frame, ROTATE_180);
        else if(rotation == 270)
          rotate(frame, frame, ROTATE_90_COUNTERCLOCKWISE);
        trackbarCallback(bottomLeftXSlider, 0);
      }
    }
    c = waitKey(0);
  }

  if(loadOkay)
  {
    if(vidFound)
    {
      pointElem = vidElem->FirstChildElement("Point");
      pointElem->SetAttribute("x", topLeftXSlider);
      pointElem->SetAttribute("y", frameHeight - topYSlider);
      pointElem = pointElem->NextSiblingElement();
      pointElem->SetAttribute("x", topRightXSlider);
      pointElem->SetAttribute("y", frameHeight - topYSlider);
      pointElem = pointElem->NextSiblingElement();
      pointElem->SetAttribute("x", bottomRightXSlider);
      pointElem->SetAttribute("y", frameHeight);
      pointElem = pointElem->NextSiblingElement();
      pointElem->SetAttribute("x", bottomLeftXSlider);
      pointElem->SetAttribute("y", frameHeight);
    }
    else
    {
      TiXmlElement *root = hDoc.FirstChild("Calibration").ToElement();
      vidElem = new TiXmlElement("Video");
      root->LinkEndChild(vidElem);
      vidElem->SetAttribute("filename", vidName);
      vidElem->SetAttribute("rotation", rotation);
      pointElem = new TiXmlElement("Point");
      vidElem->LinkEndChild(pointElem);
      pointElem->SetAttribute("x", topLeftXSlider);
      pointElem->SetAttribute("y", frameHeight - topYSlider);
      pointElem = new TiXmlElement("Point");
      vidElem->LinkEndChild(pointElem);
      pointElem->SetAttribute("x", topRightXSlider);
      pointElem->SetAttribute("y", frameHeight - topYSlider);
      pointElem = new TiXmlElement("Point");
      vidElem->LinkEndChild(pointElem);
      pointElem->SetAttribute("x", bottomRightXSlider);
      pointElem->SetAttribute("y", frameHeight);
      pointElem = new TiXmlElement("Point");
      vidElem->LinkEndChild(pointElem);
      pointElem->SetAttribute("x", bottomLeftXSlider);
      pointElem->SetAttribute("y", frameHeight);
    }
    doc.SaveFile(configFilename);
  }
  else
	{
    cout<<"Creating new config file"<<endl;
    createNewConfig(vidName);
	}
  return 0;
}


static void trackbarCallback(int, void*)
{
  shadow = Mat::zeros(frame.size(), frame.type());
  Point pts[4];
  pts[0] = Point(topLeftXSlider, frameHeight - topYSlider);
  pts[1] = Point(topRightXSlider, frameHeight - topYSlider);
  pts[2] = Point(bottomRightXSlider, frameHeight);
  pts[3] = Point(bottomLeftXSlider, frameHeight);
  fillConvexPoly(shadow, pts, 4, Scalar(255, 255, 0));
  rectangle(shadow, Point(0, 0), Point(shadow.cols, 40), Scalar(255, 255, 255), -1);
  addWeighted(frame, 1, shadow, -0.5, 0.0, combined);
  putText(combined, "Press 'r' to rotate, 'n' to use a different frame, any other key to save",
          Point(10,30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
  imshow("Calibration", combined);
}


void createNewConfig(string vidName)
{
  TiXmlDocument doc;
  TiXmlElement* msg;
  TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
  doc.LinkEndChild(decl);

  TiXmlElement *root = new TiXmlElement("Calibration");
  doc.LinkEndChild(root);

  TiXmlElement *video;
  video = new TiXmlElement("Video");
  root->LinkEndChild(video);
  video->SetAttribute("filename", vidName);
  video->SetAttribute("rotation", rotation);

  TiXmlElement *point;
  point = new TiXmlElement("Point");
  video->LinkEndChild(point);
  point->SetAttribute("x", topLeftXSlider);
  point->SetAttribute("y", frameHeight - topYSlider);
  point = new TiXmlElement("Point");
  video->LinkEndChild(point);
  point->SetAttribute("x", topRightXSlider);
  point->SetAttribute("y", frameHeight - topYSlider);
  point = new TiXmlElement("Point");
  video->LinkEndChild(point);
  point->SetAttribute("x", bottomRightXSlider);
  point->SetAttribute("y", frameHeight);
  point = new TiXmlElement("Point");
  video->LinkEndChild(point);
  point->SetAttribute("x", bottomLeftXSlider);
  point->SetAttribute("y", frameHeight);

  doc.SaveFile(configFilename);
}
