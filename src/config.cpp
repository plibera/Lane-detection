#include "config.h"
#include "tinyxml.h"

using namespace std;
using namespace cv;


Config::Config(string file)
{
  configFilename = file;
  rotation = 0;
}

void Config::setConfigFile(string file)
{
  configFilename = file;
}

bool Config::load(string file)
{
  string vidName = file;

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
      Point a;
      rotation = atoi(vidElem->Attribute("rotation"));
      pointElem = vidElem->FirstChildElement("Point");
      for(int i = 0; i < 4; ++i)
      {
        a.x = atoi(pointElem->Attribute("x"));
        a.y = atoi(pointElem->Attribute("y"));
        lanePoints.push_back(a);
        pointElem = pointElem->NextSiblingElement();
      }
      return true;
    }
    cout<<"Video "<<vidName<<" was not found in the config file "<<configFilename<<endl;
    cout<<"Calirate the video with ./calibrate "<<vidName<<endl;
    return false;
	}
  cout<<"Configuration file "<<configFilename<<" could not be opened."<<endl;
  return false;
}

int Config::getRotation()
{
  return rotation;
}

vector<Point> Config::getPoints()
{
  return lanePoints;
}
