cmake_minimum_required(VERSION 3.3)
#set(CMAKE_CXX_FLAGS "-fpermissive -pg -DTX_VERSION")
#set(CMAKE_CXX_FLAGS "-fpermissive -pg")
set(CMAKE_CXX_FLAGS "-fpermissive -p -g -Wl,-rpath=../../libs/")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp ")
SET(CMAKE_BUILD_TYPE Debug)
# SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 
# SET(CMAKE_INSTALL_RPATH "\${ORIGIN}/../../libs") #指定运行时动态库的加载路径,ORIGIN指运行文件所在目录
project (wp_face)
find_package( OpenCV REQUIRED )

message(status " This is the CMAKE_CURRENT_SOURCE_DIR srcs:" ${CMAKE_CURRENT_SOURCE_DIR})
message(status " This is the PROJECT_SOURCE_DIR srcs:" ${PROJECT_SOURCE_DIR})
message(status " This is the CMAKE_PREFIX_PATH srcs:" ${CMAKE_PREFIX_PATH})
message(status " This is the ORIGIN srcs:" ${ORIGIN})

#message(status " This is the OpenCV_INCLUDE_DIRS srcs:" ${OpenCV_INCLUDE_DIRS})
#message(status " This is the OpenCV_LIBS srcs:" ${OpenCV_LIBS})
message(status " This is the OpenCV_VERSION srcs:" ${OpenCV_VERSION})
file(GLOB facedetectcnn_source ./facedetectcnn/*.cpp ./facedetectcnn/*.h)
file(GLOB arcface_source ./arcface/inc/*.h)
file(GLOB sources *.cpp *.h)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/seeta/includes
  ${CMAKE_CURRENT_SOURCE_DIR}/facedetectcnn
  ${CMAKE_CURRENT_SOURCE_DIR}/arcface/inc
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OpenCV_INCLUDE_DIRS}
)

set(server_STAT_SRCS
  ${sources}
  ${facedetectcnn_source}
  #${arcface_source}
)
link_directories(../../libs)
#link_directories(./seeta/linux_so)
#link_directories(./arcface/linux_so)

message(status " This is the server_share srcs:" ${server_STAT_SRCS})

#find_library(ARC_LIBRARY arcsoft_face HINTS ${CMAKE_CURRENT_SOURCE_DIR}/arcface/linux_so)
#message(status " This is the ARC_LIBRARY srcs:" ${ARC_LIBRARY})

#find_library(ARC_ENGIN_LIBRARY arcsoft_face_engine HINTS ${CMAKE_CURRENT_SOURCE_DIR}/arcface/linux_so)
#message(status " This is the ARC_ENGIN_LIBRARY srcs:" ${ARC_ENGIN_LIBRARY})

# set(ARC_DYNAMIC_LIBS  "${CMAKE_CURRENT_SOURCE_DIR}/../libs")
# add_library(libarcsoft_face SHARED IMPORTED )
# set_target_properties(libarcsoft_face PROPERTIES   IMPORTED_LOCATION "${ARC_DYNAMIC_LIBS}/libarcsoft_face.so")
# add_library(libarcsoft_face_engine SHARED IMPORTED )
# set_target_properties(libarcsoft_face_engine PROPERTIES   IMPORTED_LOCATION "${ARC_DYNAMIC_LIBS}/libarcsoft_face_engine.so")
# set(mArc_LIBS  libarcsoft_face libarcsoft_face_engine)


add_executable(${PROJECT_NAME} ${server_STAT_SRCS})
target_link_libraries(${PROJECT_NAME} curl)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries( ${PROJECT_NAME} 
 tennis_haswell
 tennis_pentium
 tennis_sandy_bridge
 tennis
 SeetaAgePredictor600
 SeetaAuthorize
 SeetaEyeStateDetector200
 SeetaFaceAntiSpoofingX600
 SeetaFaceDetector600
 SeetaFaceLandmarker600
 SeetaFaceRecognizer610
 SeetaFaceTracking600
 SeetaGenderPredictor600
 SeetaMaskDetector200
 SeetaPoseEstimation600
 SeetaQualityAssessor300
 )
#target_link_libraries(${PROJECT_NAME} ${mArc_LIBS})
target_link_libraries(${PROJECT_NAME} arcsoft_face)
target_link_libraries(${PROJECT_NAME} arcsoft_face_engine)
#target_link_libraries(${PROJECT_NAME} ${ARC_ENGIN_LIBRARY})
#target_link_libraries(${PROJECT_NAME} ${ARC_LIBRARY})