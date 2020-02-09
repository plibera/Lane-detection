cmake_minimum_required(VERSION 2.8)

project( LaneDetection )


find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )


add_executable( laneDetection
           main.cpp
           FourPoints.cpp
           BirdsEyeView.cpp
           Window.cpp
           PolyFit.cpp
           LaneDetect.cpp)
target_link_libraries( laneDetection ${OpenCV_LIBS} )

add_library(STATIC tinyxml
  tinyxml/tinystr.cpp
  tinyxml/tinyxml.cpp
  tinyxml/tinyxmlerror.cpp
  tinyxml/tinyxmlparser.cpp
)

add_executable( calibration
          calibration.cpp
        )
target_link_libraries( calibration ${OpenCV_LIBS} tinyxml )
