#include "util.h"
#include <time.h>
#include <vector>
#include <string>
#include <unistd.h>
#include <curl/curl.h>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;


//时间戳转换为日期格式
void timestampToTime(char* timeStamp, char* dateTime, int dateTimeSize)
{
	time_t tTimeStamp = atoll(timeStamp);
	struct tm* pTm = gmtime(&tTimeStamp);
	strftime(dateTime, dateTimeSize, "%Y-%m-%d %H:%M:%S", pTm);
}


UnixTime getUnixTime() {
    struct timeval tv;
    gettimeofday (&tv , NULL);
    UnixTime currentTimeStamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return currentTimeStamp;
}


//curl writefunction to be passed as a parameter
// we can't ever expect to get the whole image in one piece,
// every router / hub is entitled to fragment it into parts
// (like 1-8k at a time),
// so insert the part at the end of our stream.
size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    vector<uchar> *stream = (vector<uchar>*)userdata;
    //cout<<ptr<< size<<endl;
    size_t count = size * nmemb;
    stream->insert(stream->end(), ptr, ptr + count);
    //cout<<count<<endl;
    return count;
}

void modifyImageFormat(cv::Mat& out, cv::Mat& in, int channel, int depth) 
{
    float factor = 1;
    if (depth == 2) {
        factor = 1.0 / 255;
    } else if (depth == 4) {
        factor = 1.0 / (255 * 255);
    }
    if (channel == 4 && depth > 0) {
        cv::Mat tmp;
        cv::cvtColor(in, tmp, cv::COLOR_BGRA2BGR);
        tmp.convertTo(out, CV_8UC3, factor);
        tmp.release();
    } else if (channel == 4) {
        cv::cvtColor(in, out, cv::COLOR_BGRA2BGR);
    } else if (depth > 0) {
        in.convertTo(out, CV_8UC3, factor);
    }
}


//function to retrieve the image as cv::Mat data type
cv::Mat curlImg(const char *img_url, int timeout)
{
    //cout<<img_url<<endl;
    char *pCaPath = "/opt/ssl_csr/*.haizitong.com.chained.crt";
    vector<uchar> stream;
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, img_url); //the img url
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); // pass the writefunction
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream); // pass the stream ptr to the writefunction
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout); // timeout if curl_easy hangs, 
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); //重定向一次

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L); //表示启用了验证访问的服务器合法性，且必须设置 CURLOPT_CAINFO 或 CURLOPT_CAPATH 其中一个
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L); //表示验证 CA 证书中的 common name 是否与访问的服务器域名是否一致
    curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
    CURLcode res = curl_easy_perform(curl); // start curl    
    curl_easy_cleanup(curl); // cleanup
    if (res != CURLE_OK) {
        cout << "curl errcode=" << res << " " << img_url <<endl;
        cv::Mat dest(0, 0, 1);
        return dest;
    }
    //cout<<img_url<<" download: " << stream.size() << " " << stream.max_size()<<endl;
    cv::Mat src = imdecode(stream, -1); // 'keep-as-is'
    //int Width = flower.cols;
    //int Height = src.rows;
    //cout << "Width:" << Width << ",Height:"<<Height <<endl;
    int depth = src.depth();
    int channel = src.channels();
    // cout << "dims: " << src.dims << endl;
    // cout << "channels: " << channel << endl;
    // cout << "depth: " << cv::depthToString(src.depth()) << " , " << src.depth() << endl;
    // cout << "type: " << cv::typeToString(src.type()) << " , value=" << src.type() << endl;
    // cout << "depth:" << depth << ", channel" << channel << endl;
    if (depth != 0 || channel > 3) { //bit数仅支持8
        cv::Mat mid; //(src.cols, src.rows, CV_8UC3);
        modifyImageFormat(mid, src, channel, depth);
        src.release();
        return mid;
    } 
    return src;
}

std::vector<std::string> stringSplit(const std::string& str, char delim) {
    std::stringstream ss(str);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        if (!item.empty()) {
            elems.push_back(item);
        }
    }
    return elems;
}

std::string getImageUrlId(const std::string& str) {
    std::vector<std::string> vecList = stringSplit(str, '/');
    return vecList[vecList.size() - 1];
}

std::string getCurrentPath() {
    char buffer[256];
    getcwd(buffer, 256);
    cout << buffer << endl;
    return string(buffer);
}

void imageBackgroundBlack(cv::Mat& src, const std::string key) {
    int Width = src.cols;
	int Height = src.rows;
    int dims = src.channels();
    for(int j = 0; j < Height; ++j){
        for(int i = 0; i < Width; ++i) {
            if (dims == 4){
				Vec4b bgr=src.at<Vec4b>(j, i);//返回4个值哦	可以把bgr这个结构看成数组
                //cout << (int)bgr[3] << " "; 
                if ((int)bgr[3] <= 10) { //bgr[0] == 255 && bgr[1] == 255 && bgr[2] ==255
                    src.at<Vec4b>(j, i)[0] = 0; //255 - bgr[0];
				    src.at<Vec4b>(j, i)[1] = 0; //255 - bgr[1];
				    src.at<Vec4b>(j, i)[2] = 0; //255 - bgr[2];
                }
			}
        }
        //cout << endl; 
    }
    cv::Mat dst;
    cv::cvtColor(src, dst, cv::COLOR_BGRA2BGR);
    cv::imwrite("back-"+ key + ".png", dst);
    dst.release();
}

//图片混合
bool MixImage(cv::Mat& srcImage, cv::Mat mixImage, cv::Point startPoint){
	//检查图片数据
	if (!srcImage.data || !mixImage.data)
	{
		cout << "输入图片 数据错误！" << endl ;
		return false;
	}
	//检查行列是否越界
	int addCols = startPoint.x + mixImage.cols > srcImage.cols ? 0 : mixImage.cols;
	int addRows = startPoint.y + mixImage.rows > srcImage.rows ? 0 : mixImage.rows;
	if (addCols ==0 || addRows ==0)
	{
		cout << "添加图片超出" << endl;
		return false;
	}

	//ROI 混合区域
	Mat roiImage = srcImage(Rect(startPoint.x, startPoint.y, addCols, addRows));
	//图片类型一致
	if (srcImage.type() == mixImage.type()){
		mixImage.copyTo(roiImage, mixImage);
		return true;
	}

	Mat maskImage;
	//原始图片：灰度  贴图：彩色
	if (srcImage.type() == CV_8U && mixImage.type() == CV_8UC3)
	{
		cvtColor(mixImage, maskImage, COLOR_BGR2GRAY);
		maskImage.copyTo(roiImage, maskImage);
		return true;
	}

	//原始图片：彩色  贴图：灰色
	if (srcImage.type() == CV_8UC3 && mixImage.type() == CV_8U)
	{
		cvtColor(mixImage, maskImage, COLOR_GRAY2BGR);
		maskImage.copyTo(roiImage, maskImage);
		return true;
	}

	return false;
}

void drawFaceAndSave(std::vector<cv::Rect>& faces, cv::Mat& src, const std::string& str, const std::string& key, const std::string& output) {
	string id = getImageUrlId(str);
	cv::Mat flower = cv::imread(key.c_str()); // 0 bgr, 1 grey, -1 aplha
	int Width = flower.cols;
	int Height = flower.rows;
	//cout << "Height=" << Height << " , Width=" << Width << endl;
	for(int i = 0; i < faces.size(); ++i) {
        cv::Rect rc = faces[i];
		// float factor = (float)rc.width / Width;
		// int height = (int)(factor * Height);
		// cout << "rcHeight=" << rc.height << " , rcWidth=" << rc.width << endl;
		// cout << "factor: " << factor << " , height=" << height << endl;
		cv::Size dsize;
		if (rc.width > rc.height) {
			float factor = (float)rc.width / Width;
			int height = (int)(factor * Height);
			// cout << "rcHeight=" << rc.height << " , rcWidth=" << rc.width << endl;
			// cout << "factor: " << factor << " , height=" << height << endl;
			dsize = cv::Size(rc.width, height);
		} else {
			float factor = (float)rc.height / Height;
			int width = (int)(factor * Width);
			// cout << "rcHeight=" << rc.height << " , rcWidth=" << rc.width << endl;
			// cout << "factor: " << factor << " , width=" << width << endl;
			dsize = cv::Size(width, rc.height);
		}
        //cv::Size dsize = cv::Size(rc.width, rc.height);
        cv::Mat flowerMode = cv::Mat(dsize, CV_8U);
        cv::resize(flower, flowerMode, dsize);
        cv::Point startPoint(rc.x, rc.y);
        bool isDone = MixImage(src, flowerMode, startPoint);
        flowerMode.release();
    }
	string fileName = output + "/" + id + ".png";
    imwrite(fileName.c_str(), src);
	flower.release();
}
