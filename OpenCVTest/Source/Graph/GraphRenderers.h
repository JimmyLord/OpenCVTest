#ifndef __GraphRenderers_H__
#define __GraphRenderers_H__

#include "GraphTypes.h"

void DrawPointList(cv::Mat& image, vec2 imageScale, const pointList& points, cv::Vec3b color, int radius = 0);
void DrawTriangulation(cv::Mat& image, vec2 imageScale, const pointList& list, const fullNeighbourList& neighbours, cv::Scalar lineColor);
void DrawPointsThatHaveNeighbours(cv::Mat& image, ivec2 imageSize, const std::vector<vec2>& pointList, const fullNeighbourList& neighbours);
void DrawPointsFlaggedInBoolVector(cv::Mat& image, ivec2 imageSize, const std::vector<vec2>& pointList, const std::vector<bool>& flaggedPoints);
void DrawPath(cv::Mat& image, vec2 imageScale, const pointList& points, const graphPath& path, cv::Scalar lineColor);

#endif __GraphRenderers_H__
