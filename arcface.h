#ifndef ARC_FACE_H
#define ARC_FACE_H

#include <opencv2/opencv.hpp>
#include "arcsoft_face_sdk.h"
#include "amcomdef.h"
#include "asvloffscreen.h"
#include "merror.h"
#include <string>
#include <vector>
#include "face.h"

typedef struct{
        MInt32      searchId;       // 唯一标识符
        LPASF_FaceFeature feature;  // 人脸特征值
        std::string tag;
        //MPCChar     tag;            // 备注
}ASF_FaceFeatureInfo, *LPASF_FaceFeatureInfo;

class ArcFace : public Face {
public:
    ArcFace(std::string APPID, std::string SDKKEY, std::string ACTIVEKEY);
    ~ArcFace();
    void Init();
    static ArcFace * GetInstance(std::string APPID, std::string SDKKEY, std::string ACTIVEKEY);
    std::vector<cv::Rect> findFacesAndDrawSave(const std::string& str, const std::string& key, const std::string& output);
    std::string getName() {return "arc";}
private:
    void getSdkRelatedInfo();
    int ColorSpaceConversion(MInt32 width, MInt32 height, MInt32 format, MUInt8* imgData, ASVLOFFSCREEN& offscreen);
private:
    static ArcFace * m_pInstance;
    MHandle handle;
};

#endif