#ifndef CNN_H
#define CNN_H

#include <opencv2/opencv.hpp>
#include "face.h"

class CNN : public Face {
public:
    CNN();
    ~CNN();
    void Init();
    static CNN * GetInstance();
    std::vector<cv::Rect> findFacesAndDrawSave(const std::string& str, const std::string& key, const std::string& output);
    std::string getName() {return "cnn";}
private:
    static CNN * m_pInstance;
};

#endif