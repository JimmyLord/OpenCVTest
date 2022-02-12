//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __Helpers_H__
#define __Helpers_H__

#include "opencv2/opencv.hpp"

float randFloat(float min, float max);
size_t randSizeT(size_t min, size_t max);

void ShowImageWithFixedWidthAtPosition(const cv::String windowName, cv::Mat& image, int width, int posX, int posY);

void GenerateDFTFromGrayscaleImage(cv::Mat& image, cv::Mat& output);
void ShowDFTResult(const cv::String windowName, cv::Mat& image, int width, int posX, int posY);

void ShiftTopLeftToCenter(cv::Mat& image);

uint32 NextPowerOfTwo(int value);

void BindCVMat2GLTexture(cv::Mat& image, GLuint& imageTexture, uint32* w, uint32* h);
void CopyFBOToCVMat(FBODefinition* pFBO, cv::Mat& dest, bool bindFBO);
TextureDefinition* CreateOrUpdateTextureDefinitionFromOpenCVMat(cv::Mat* pImage, TextureDefinition* pOldTexture = nullptr);

// Display a Texture using an OpenCV matrix for sizes and to get pixel info, can also get callback if wanted.
using HoverCallbackFunc = std::function<void(Vector2 pixelPos)>;
void DisplayOpenCVMatAndTexture(cv::Mat* pImage, TextureDefinition* pTexture, int width, float pixelsToShow, HoverCallbackFunc pHoverCallbackFunc = nullptr);

void PrintFloatBadlyWithPrecision(std::string& str, float value, int maxDecimalPlaces);

typedef std::vector<cv::Vec3b> colorPalette;
std::vector<cv::Vec3b> GeneratePalette();

cv::Vec3f AsVec3f(Vector3 v);
cv::Vec3b AsVec3b(Vector3 v);
Vector3 AsVec3(cv::Vec3f v);
Vector3 AsVec3(cv::Vec3b v);

#endif //__Helpers_H__
