#ifndef UTIL_H
#define UTIL_H
#include <opencv2/opencv.hpp>

typedef long long UnixTime;

UnixTime getUnixTime();
void timestampToTime(char* timeStamp, char* dateTime, int dateTimeSize);
size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata);
void modifyImageFormat(cv::Mat& out, cv::Mat& in, int channel, int depth);
cv::Mat curlImg(const char *img_url, int timeout);

std::vector<std::string> stringSplit(const std::string& str, char delim);
std::string getImageUrlId(const std::string& str);
std::string getCurrentPath();
void imageBackgroundBlack(cv::Mat& src, const std::string key);

bool MixImage(cv::Mat& srcImage, cv::Mat mixImage, cv::Point startPoint);
void drawFaceAndSave(std::vector<cv::Rect>& faces, cv::Mat& src, const std::string& str, const std::string& key, const std::string& output);

#endif