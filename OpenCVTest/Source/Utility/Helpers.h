//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __Helpers_H__
#define __Helpers_H__

#include "opencv2/opencv.hpp"

float randFloat(float min, float max);

void ShowImageWithFixedWidthAtPosition(const cv::String windowName, cv::Mat& image, int width, int posX, int posY);

void GenerateDFTFromGrayscaleImage(cv::Mat& image, cv::Mat& output);
void ShowDFTResult(const cv::String windowName, cv::Mat& image, int width, int posX, int posY);

void ShiftTopLeftToCenter(cv::Mat& image);

uint32 NextPowerOfTwo(int value);

void BindCVMat2GLTexture(cv::Mat& image, GLuint& imageTexture, uint32* w, uint32* h);
void CopyFBOToCVMat(FBODefinition* pFBO, cv::Mat& dest, bool bindFBO);
TextureDefinition* CreateOrUpdateTextureDefinitionFromOpenCVMat(cv::Mat* pImage, TextureDefinition* pOldTexture = nullptr);
void DisplayOpenCVMatAndTexture(cv::Mat* pImage, TextureDefinition* pTexture, float size, float pixelsToShow);

void PrintFloatBadlyWithPrecision(std::string& str, float value, int maxDecimalPlaces);

#endif //__Helpers_H__
