project( ObstacleDetector ) 
cmake_minimum_required( VERSION 2.8.7 )   
find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
  

add_library(ObstacleDetector  SHARED  ObstacleDetector.h main.c ../ViewpointChange/viewpoint_change.c ../ViewpointChange/viewpoint_change.h ) 
target_link_libraries(ObstacleDetector rt dl m Acquisition OperatingSystem Codecs Timers CalibrationLibrary ${OpenCV_LIBRARIES} ${LocationService_Libs} )
set_target_properties(ObstacleDetector PROPERTIES DEBUG_POSTFIX "D")
add_dependencies(ObstacleDetector Acquisition OperatingSystem Timers CalibrationLibrary)  
       


set_target_properties(ObstacleDetector PROPERTIES 
                       DEBUG_POSTFIX "D"
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                      )
