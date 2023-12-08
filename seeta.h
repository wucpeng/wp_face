#ifndef SEETA_H
#define SEETA_H

#include <opencv2/opencv.hpp>
#include <seeta/Struct_cv.h>
#include <seeta/Struct.h>

#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>
#include <seeta/FaceRecognizer.h>
#include "face.h"

typedef unsigned long  uint32;
typedef struct{
    uint32      searchId; // 唯一标识符
    float*      feature;  // 人脸特征值
    std::string tag;      // 备注
}SF_FaceFeatureInfo;
typedef struct __face_score
{
	uint32      searchId;
	float       score;
	std::string name;
} MFaceSore, *PMFaceSore;

class Seeta : public Face {
public:
    Seeta();
    ~Seeta();
    void Init();
    static Seeta * GetInstance();
    std::vector<cv::Rect> findFacesAndDrawSave(const std::string& str, const std::string& key, const std::string& output);
    std::string getName() {return "seeta";}
private:
    static Seeta * m_pInstance;
    const seeta::ModelSetting::Device device = seeta::ModelSetting::CPU;
    seeta::ModelSetting setFD;
    seeta::ModelSetting settFL;
    seeta::ModelSetting settFR;
    seeta::FaceDetector* mFD = NULL;
    seeta::FaceLandmarker* mFL = NULL;
    seeta::FaceRecognizer* mFR = NULL;
    float threshold = 0.60;
    std::vector<SF_FaceFeatureInfo> m_vecFeatureInfoList;
};

#endif