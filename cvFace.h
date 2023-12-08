#ifndef CV_FACE_H
#define CV_FACE_H

#include <opencv2/opencv.hpp>
#include "face.h"

class CVFace : public Face 
{
public:
    CVFace();
    ~CVFace();
    void Init();
    static CVFace * GetInstance();
    std::vector<cv::Rect> findFacesAndDrawSave(const std::string& str, const std::string& key, const std::string& output);
    std::string getName() {return "cv";}
private:
    static CVFace * m_pInstance;
    cv::CascadeClassifier m_cascade;
};

#endif