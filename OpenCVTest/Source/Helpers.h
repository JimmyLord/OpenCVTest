#ifndef __Helpers_H__
#define __Helpers_H__

#include "opencv2/opencv.hpp"

void ShowImageWithFixedWidthAtPosition(const cv::String windowName, cv::Mat& image, int width, int posX, int posY);

void TakeDFT(cv::Mat& image, cv::Mat& output);
void ShowDFT(cv::Mat& image);

#endif //__Helpers_H__
