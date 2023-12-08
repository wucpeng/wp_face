#include "arcface.h"
#include "util.h"

using namespace std;
using namespace cv;
#define SafeFree(p) { if ((p)) free(p); (p) = NULL; }
#define SafeArrayDelete(p) { if ((p)) delete [] (p); (p) = NULL; } 
#define SafeDelete(p) { if ((p)) delete (p); (p) = NULL; } 
#define NSCALE 16 
#define FACENUM	20

ArcFace * ArcFace::m_pInstance = NULL;
ArcFace::ArcFace(std::string APPID, std::string SDKKEY, std::string ACTIVEKEY) {
    printf("\n************* Face Recognition *****************\n");
    MRESULT res = MOK;
    MPChar appId = (MPChar)APPID.data();
    MPChar sdkKey = (MPChar)SDKKEY.data();
    MPChar activeKey = (MPChar)ACTIVEKEY.data();
    res = ASFOnlineActivation(appId, sdkKey);
    if (MOK != res && MERR_ASF_ALREADY_ACTIVATED != res) {
        printf("ASFOnlineActivation fail: %d\n", res);
    } else {
        printf("ASFOnlineActivation sucess: %d\n", res);
    }
    //初始化引擎
    MHandle handle = NULL;
    MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_FACE3DANGLE | ASF_AGE | ASF_GENDER | ASF_LIVENESS; // 
    res = ASFInitEngine(ASF_DETECT_MODE_IMAGE, ASF_OP_0_ONLY, NSCALE, FACENUM, mask, &handle);
    if (res != MOK) {
        printf("ASFInitEngine fail: %d\n", res);
    }else {
        printf("ASFInitEngine sucess: %d\n", res);
    }
    this->handle = handle;
    cout << "ArcFace()" << endl;
}

ArcFace::~ArcFace() {
    ASFUninitEngine(this->handle);
    cout << "~ArcFace" << endl;
}

ArcFace * ArcFace::GetInstance(std::string APPID, std::string SDKKEY, std::string ACTIVEKEY){
	if (!m_pInstance){
		m_pInstance = new ArcFace(APPID, SDKKEY, ACTIVEKEY);
	}
	return m_pInstance;
}

void ArcFace::Init() {
    cout << "ArcFace Init" << endl;
}

std::vector<cv::Rect> ArcFace::findFacesAndDrawSave(const std::string& str, const std::string& key, const std::string& output) {
    std::vector<cv::Rect> faces;
    cv::Mat src = curlImg(str.c_str(), 10000);
    int Width = src.cols;
    if (Width == 0) {
        src.release();
        return faces;
    }
	int Height = src.rows;
    int wScore = Width % 4;
    int hScore = Height % 2;
    cv::Mat dest;
    if (wScore != 0 || hScore != 0) {
        Width -= wScore;
        Height -= hScore;
        cv::Mat dst;
        cv::resize(src, dst, cv::Size(Width, Height));
        cvtColor(dst, dest, COLOR_BGR2YUV_I420);
        dst.release();
    } else {
        cvtColor(src, dest, COLOR_BGR2YUV_I420);
    }
    int len = Height*Width*3/2;
    MUInt8* imageData = (MUInt8*)malloc(len);
    memset(imageData, 0, len);    
    memcpy(imageData, dest.data, len);
    
    dest.release();
    if (!imageData) {
        return faces;
    }
    ASVLOFFSCREEN offscreen = { 0 };
    ColorSpaceConversion(Width, Height, ASVL_PAF_NV21, imageData, offscreen);
    ASF_MultiFaceInfo detectedFaces = { 0 };
    MRESULT res = ASFDetectFacesEx(this->handle, &offscreen, &detectedFaces);
    for(int i = 0; i < detectedFaces.faceNum; ++i) {
        auto &face = detectedFaces.faceRect[i]; 
        cout << "face score " << i << " : " << detectedFaces.faceOrient[i] << endl;
        cout << "face left : " << face.left ;
        cout << ", face top : " << face.top ;
        cout << ", face right : " << face.right ;
        cout << ", face bottom : " << face.bottom << endl ;
        faces.push_back(cv::Rect(face.left, face.top, face.right - face.left, face.bottom - face.top));
    }
    SafeFree(imageData);

    if (faces.size() > 0) {
		drawFaceAndSave(faces, src, str, key, output);
	}
    src.release();
    return faces;
}


void ArcFace::getSdkRelatedInfo() {
    printf("\n************* ArcFace SDK Info *****************\n");
    MRESULT res = MOK;
    //采集当前设备信息，用于离线激活
    // char* deviceInfo = NULL;
    // res = ASFGetActiveDeviceInfo(&deviceInfo);
    // if (res != MOK) {
    //     printf("ASFGetActiveDeviceInfo fail: %d\n", res);
    // } else {
    //     printf("ASFGetActiveDeviceInfo sucess: %s\n", deviceInfo);
    // }
    //获取激活文件信息
    ASF_ActiveFileInfo activeFileInfo = { 0 };
    res = ASFGetActiveFileInfo(&activeFileInfo);
    if (res != MOK){
        printf("ASFGetActiveFileInfo fail: %d\n", res);
    }else{
        //这里仅获取了有效期时间，还需要其他信息直接打印即可
        char startDateTime[32];
        timestampToTime(activeFileInfo.startTime, startDateTime, 32);
        printf("startTime: %s\n", startDateTime);
        char endDateTime[32];
        timestampToTime(activeFileInfo.endTime, endDateTime, 32);
        printf("endTime: %s\n", endDateTime);
    }
    //SDK版本信息
    const ASF_VERSION version = ASFGetVersion();
    printf("\nVersion:%s\n", version.Version);
    printf("BuildDate:%s\n", version.BuildDate);
    printf("CopyRight:%s\n", version.CopyRight);
}

int ArcFace::ColorSpaceConversion(MInt32 width, MInt32 height, MInt32 format, MUInt8* imgData, ASVLOFFSCREEN& offscreen){
	offscreen.u32PixelArrayFormat = (unsigned int)format;
	offscreen.i32Width = width;
	offscreen.i32Height = height;
	
	switch (offscreen.u32PixelArrayFormat)
	{
	case ASVL_PAF_RGB24_B8G8R8:
		offscreen.pi32Pitch[0] = offscreen.i32Width * 3;
		offscreen.ppu8Plane[0] = imgData;
		break;
	case ASVL_PAF_I420:
		offscreen.pi32Pitch[0] = width;
		offscreen.pi32Pitch[1] = width >> 1;
		offscreen.pi32Pitch[2] = width >> 1;
		offscreen.ppu8Plane[0] = imgData;
		offscreen.ppu8Plane[1] = offscreen.ppu8Plane[0] + offscreen.i32Height*offscreen.i32Width;
		offscreen.ppu8Plane[2] = offscreen.ppu8Plane[0] + offscreen.i32Height*offscreen.i32Width * 5 / 4;
		break;
	case ASVL_PAF_NV12:
	case ASVL_PAF_NV21:
		offscreen.pi32Pitch[0] = offscreen.i32Width;
		offscreen.pi32Pitch[1] = offscreen.pi32Pitch[0];
		offscreen.ppu8Plane[0] = imgData;
		offscreen.ppu8Plane[1] = offscreen.ppu8Plane[0] + offscreen.pi32Pitch[0] * offscreen.i32Height;
		break;
	case ASVL_PAF_YUYV:
	case ASVL_PAF_DEPTH_U16:
		offscreen.pi32Pitch[0] = offscreen.i32Width * 2;
		offscreen.ppu8Plane[0] = imgData;
		break;
	case ASVL_PAF_GRAY:
		offscreen.pi32Pitch[0] = offscreen.i32Width;
		offscreen.ppu8Plane[0] = imgData;
		break;
	default:
		return 0;
	}
	return 1;
}