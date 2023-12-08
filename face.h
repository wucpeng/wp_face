#ifndef FACE_H
#define FACE_H
#include <opencv2/opencv.hpp>
class Face {
public:
    Face();
    virtual ~Face();
    virtual std::vector<cv::Rect> findFacesAndDrawSave(const std::string& str, const std::string& key, const std::string& output) = 0;
    virtual std::string getName() {return "face";};
};
#endif