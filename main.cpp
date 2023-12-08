//#include <opencv2/opencv.hpp>
//#include <vector>
//#include <stdio.h>
#include <unistd.h>
#include "util.h"
#include "cvFace.h"
#include "seeta.h"
#include "arcface.h"
#include "cnn.h"
#include "face.h"
using namespace std;
using namespace cv;
const string url = "http://image.haizitong.com/516a55e452564ab3b500909980cb4994932439";
const string flower = "/home/haizitong/face/wp_face_opencv/images/flower.png";

const string APPID = "EMZi3CQ9AhTn6p6qA4X83Wnc8XHPDzz6SMPdhSCfMsim";
const string SDKKEY = "H6qfq4qxwCN146W5Rs5t5PAgFG6WVq7CqvUT5UWo8Fwa";
const string ACTIVEKEY = "82K1-118E-813R-CJQ3";    

int main(int argc, char** argv) {
    cout << "argc: " << argc << endl;
    for(int i = 0; i < argc; ++i) {
        cout << "argv " << i << " : " <<argv[i] << endl;
    }
    char* path = get_current_dir_name();
    cout << "path: " << path << endl;
    free(path);

    char tmp[256];
    getcwd(tmp, 256);
    cout << "Current working directory: " << tmp << endl;

    UnixTime ut = getUnixTime();
    cout << "UnixTime: " << ut << endl;

    // Mat src = curlImg(url.c_str(), 10000);
    // int height = src.rows;
    // int width = src.cols;
    // cout << "height: " << height << endl;
    // cout << "width: " << width << endl;
    // src.release();

    // CVFace* pCvFace = CVFace::GetInstance();
    // pCvFace->Init();
    // std::vector<cv::Rect> faces1 = pCvFace->findFacesAndDrawSave(url, flower, "./cv");
    // delete pCvFace;
    // cout << "faceNum1:" << faces1.size() << endl;
    // UnixTime ut1 = getUnixTime();
    // cout << "times1: " << ut1 - ut << endl;

    // CNN* pCNN = CNN::GetInstance();
    // pCNN->Init();
    // std::vector<cv::Rect> faces2 = pCNN->findFacesAndDrawSave(url, flower, "./cnn");
    // cout << "faceNum2:" << faces2.size() << endl;
    // delete pCNN;
    // UnixTime ut2 = getUnixTime();
    // cout << "times2: " << ut2 - ut1 << endl;

    // ArcFace* pArcFace = ArcFace::GetInstance(APPID, SDKKEY, ACTIVEKEY);
    // pArcFace->Init();
    // std::vector<cv::Rect> faces3 = pArcFace->findFacesAndDrawSave(url, flower, "./arc");
    // cout << "faceNum3:" << faces3.size() << endl;
    // delete pArcFace;
    // UnixTime ut3 = getUnixTime();
    // cout << "times3: " << ut3 - ut2 << endl;

    // Seeta* pSeeta = Seeta::GetInstance();
    // pSeeta->Init();
    // std::vector<cv::Rect> faces4 = pSeeta->findFacesAndDrawSave(url, flower, "./seeta");
    // cout << "faceNum4:" << faces4.size() << endl;
    // delete pSeeta;
    // UnixTime ut4 = getUnixTime();
    // cout << "times4: " << ut4 - ut3 << endl;
    
    CVFace* pCvFace = CVFace::GetInstance();
    CNN* pCNN = CNN::GetInstance();
    ArcFace* pArcFace = ArcFace::GetInstance(APPID, SDKKEY, ACTIVEKEY);
    Seeta* pSeeta = Seeta::GetInstance();
    Face* detecFaces[4] = {pCvFace, pCNN, pArcFace, pSeeta};
    
    for(int i = 0; i < 4; ++i) {
        std::string name = detecFaces[i]->getName();
        std::vector<cv::Rect> faces = detecFaces[i]->findFacesAndDrawSave(url, flower, "./" + name);
        cout << "Name: " << name << endl;
        cout << "faceNum:" << faces.size() << endl;

        delete detecFaces[i];
    }
    

    return 0;
}