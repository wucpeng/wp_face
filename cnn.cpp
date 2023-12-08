#include "cnn.h"
#include "util.h"
#include "facedetectcnn.h"
using namespace std;
using namespace cv;
#define DETECT_BUFFER_SIZE 0x9000
#define MAX_PATH 256
#define CONFIDENCE 70 //人脸置信度

CNN * CNN::m_pInstance = NULL;
CNN::CNN() {
    
    cout << "CNN()" << endl;
}

CNN::~CNN() {
    
    cout << "~CNN" << endl;
}

CNN * CNN::GetInstance(){
	if (!m_pInstance){
		m_pInstance = new CNN();
	}
	return m_pInstance;
}

void CNN::Init() {
    cout << "CNN Init" << endl;
}

std::vector<cv::Rect> CNN::findFacesAndDrawSave(const std::string& str, const std::string& key, const std::string& output){
    std::vector<cv::Rect> faces;
    cv::Mat src = curlImg(str.c_str(), 10000);
    int Width = src.cols;
    if (Width == 0) {
        src.release();
        return faces;
    }
    int * pResults = NULL; 
    //pBuffer is used in the detection functions.
    //If you call functions in multiple threads, please create one buffer for each thread!
    unsigned char * pBuffer = (unsigned char *)malloc(DETECT_BUFFER_SIZE);
    if(!pBuffer)
    {
        fprintf(stderr, "Can not alloc buffer.\n");
        return faces;
    }
	// cout << "depth: " << cv::depthToString(src.depth()) << " , " << src.depth() << endl;
    // cout << "type: " << cv::typeToString(src.type()) << " , value=" << src.type() << endl;

	TickMeter cvtm;
    cvtm.start();
    UnixTime s1 = getUnixTime();
	pResults = facedetect_cnn(pBuffer, (unsigned char*)(src.ptr(0)), (int)src.cols, (int)src.rows, (int)src.step);
    //UnixTime s2 = getUnixTime();
    //cout << "times: " << s2 - s1 << endl;
    cvtm.stop();    
    printf("time = %gms\n", cvtm.getTimeMilli());
	printf("%d faces detected.\n", (pResults ? *pResults : 0));

	for(int i = 0; i < (pResults ? *pResults : 0); i++)
	{
        short * p = ((short*)(pResults + 1)) + 16*i;
		int confidence = p[0];
		if (confidence < CONFIDENCE) break;
		int x = p[1];
		int y = p[2];
		int w = p[3];
		int h = p[4];
		//show the score of the face. Its range is [0-100]
        // char sScore[256];
        // snprintf(sScore, 256, "%d", confidence);
		// printf("face %d: confidence=%d, [%d, %d, %d, %d] (%d,%d) (%d,%d) (%d,%d) (%d,%d) (%d,%d)\n", 
        //         i, confidence, x, y, w, h, 
        //         p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13],p[14]);

		int addWidth = w / 10;
		if (addWidth) {
			x -= addWidth;
			if (x < 0 ) x = 0;
			//w += addWidth;
		}
		int addHeight = h / 10;
		if (addHeight) {
			y -= addHeight;
			if (y < 0 ) y = 0;
			//h += addHeight;
		}

		cv::Rect face(x, y, w, h);
		faces.push_back(face);
	}
	free(pBuffer);
	if (faces.size() > 0) {
		drawFaceAndSave(faces, src, str, key, output);
	} 
	src.release();
    return faces;
}