#include "seeta.h"
#include "util.h"
using namespace std;
using namespace cv;

const string file1 = "../seeta/model/face_detector.csta";
const string file2 = "../seeta/model/face_landmarker_pts5.csta";
const string file3 = "../seeta/model/face_recognizer.csta";

Seeta * Seeta::m_pInstance = NULL;
Seeta::Seeta() {
    setFD.set_device( device );
    setFD.set_id( 0 );
    setFD.append( file1 );
    mFD = new seeta::FaceDetector(setFD);
    mFD->set(seeta::FaceDetector::PROPERTY_MIN_FACE_SIZE, 80);

    settFL.set_device( device );
    settFL.set_id( 0 );
    settFL.append( file2 );
    mFL = new seeta::FaceLandmarker(settFL);

    settFR.set_device( device );
    settFR.set_id( 0 );
    settFR.append( file3 );
    mFR = new seeta::FaceRecognizer(settFR);
    cout << "Seeta()" << endl;
}

Seeta::~Seeta() {
    delete mFD;
    delete mFL;
    delete mFR;
    cout << "~Seeta" << endl;
}

Seeta * Seeta::GetInstance(){
	if (!m_pInstance){
		m_pInstance = new Seeta();
	}
	return m_pInstance;
}

void Seeta::Init() {
    cout << "Seeta Init" << endl;
}

std::vector<cv::Rect> Seeta::findFacesAndDrawSave(const std::string& str, const std::string& key, const std::string& output) {
    std::vector<cv::Rect> faces;
    cv::Mat src = curlImg(str.c_str(), 10000);
    int Width = src.cols;
    if (Width == 0) {
        src.release();
        return faces;
    }
    seeta::cv::ImageData image = src;
    auto face_infos = mFD->detect(image);
    for(int i = 0; i < face_infos.size; ++i) {
        auto &face = face_infos.data[i]; 
        cout << "face score " << i << " : " << face.score << endl;
        faces.push_back(cv::Rect(face.pos.x, face.pos.y, face.pos.width, face.pos.height));
    }

    if (faces.size() > 0) {
		drawFaceAndSave(faces, src, str, key, output);
	}

    return faces;
}