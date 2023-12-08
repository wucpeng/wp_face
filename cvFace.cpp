#include "cvFace.h"
#include "util.h"

using namespace std;
using namespace cv;
const string xmlPath = "../xml/haarcascade_frontalface_alt2.xml";

CVFace * CVFace::m_pInstance = NULL;
CVFace::CVFace() : Face() {
    if (!m_cascade.load(xmlPath)){
		cout << "cascade load failed!\n";
	}
    cout << "CVFace()" << endl;
}

CVFace::~CVFace() {
    
    cout << "~CVFace" << endl;
}

CVFace * CVFace::GetInstance(){
	if (!m_pInstance){
		m_pInstance = new CVFace();
	}
	return m_pInstance;
}

void CVFace::Init() {
    cout << "CVFace Init" << endl;
}

std::vector<cv::Rect> CVFace::findFacesAndDrawSave(const std::string& str, const std::string& key, const std::string& output){
    std::vector<cv::Rect> faces;
    cv::Mat src = curlImg(str.c_str(), 10000);
    int Width = src.cols;
    if (Width == 0) {
        src.release();
        return faces;
    }
    //double t = 0;
	//t = (double)getTickCount();
    //cout << str << " getTickCount: " << t << endl;
    m_cascade.detectMultiScale(src, faces, 1.1, 4, 0, Size(30, 30));

	if (faces.size() > 0) {
		drawFaceAndSave(faces, src, str, key, output);
	} 
	src.release();
    return faces;
}