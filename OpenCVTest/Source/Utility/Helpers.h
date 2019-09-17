//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __Helpers_H__
#define __Helpers_H__

#include "opencv2/opencv.hpp"

void ShowImageWithFixedWidthAtPosition(const cv::String windowName, cv::Mat& image, int width, int posX, int posY);

void GenerateDFTFromGrayscaleImage(cv::Mat& image, cv::Mat& output);
void ShowDFTResult(const cv::String windowName, cv::Mat& image, int width, int posX, int posY);

void ShiftTopLeftToCenter(cv::Mat& image);

void BindCVMat2GLTexture(cv::Mat& image, GLuint& imageTexture);
TextureDefinition* CreateOrUpdateTextureDefinitionFromOpenCVMat(cv::Mat* pImage, TextureDefinition* pOldTexture = nullptr);

#endif //__Helpers_H__
