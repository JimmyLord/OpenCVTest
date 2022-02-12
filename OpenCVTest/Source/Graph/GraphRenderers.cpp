#include "OpenCVPCH.h"

#include "GraphTypes.h"
#include "GraphRenderers.h"

void DrawPointList(cv::Mat& image, vec2 imageScale, const pointList& points, cv::Vec3b color, int radius)
{
    for( size_t i=0; i<points.size(); i++ )
    {
        vec2 p1 = points[i] * imageScale;

        cv::circle( image, cvPoint(p1), radius, color, radius );
    }
}

void DrawTriangulation(cv::Mat& image, vec2 imageScale, const pointList& list, const fullNeighbourList& neighbours, cv::Scalar lineColor)
{
    for( size_t i=0; i<neighbours.size(); i++ )
    {
        for( const vertIndex n : neighbours[i] )
        {
            if( i < n )
            {
                vec2 p1 = list[i] * imageScale;
                vec2 p2 = list[n] * imageScale;

                cv::line( image, cvPoint(p1), cvPoint(p2), lineColor );
            }
        }
    }
}

void DrawPointsThatHaveNeighbours(cv::Mat& image, ivec2 imageSize, const std::vector<vec2>& pointList, const fullNeighbourList& neighbours)
{
    cv::Vec3b color = cv::Vec3b( 0, 255, 0 );
    int size = 8;

    bool colorEachVertex = true;
    if( pointList.size() > 20 )
        colorEachVertex = false;

    for( size_t i=0; i<pointList.size(); i++ )
    {
        if( neighbours.size() != pointList.size() || neighbours[i].size() == 0 )
            continue;

        if( i > 0 )
        {
            color = cv::Vec3b( 0, 192, 0 );
            size = 4;
        }

        vec2 pos = pointList[i] / 100.0f * vec2( (float)imageSize.x, (float)imageSize.y );

        cv::circle( image, cv::Point((int)pos.x,(int)pos.y), size, color, -1 );
    }
}

void DrawPointsFlaggedInBoolVector(cv::Mat& image, ivec2 imageSize, const std::vector<vec2>& pointList, const std::vector<bool>& flaggedPoints)
{
    cv::Vec3b color = cv::Vec3b( 0, 255, 0 );
    int size = 1;

    bool colorEachVertex = true;
    if( pointList.size() > 20 )
        colorEachVertex = false;

    for( size_t i=0; i<pointList.size(); i++ )
    {
        if( flaggedPoints[i] )
        {
            vec2 pos = pointList[i] / 100.0f * vec2( (float)imageSize.x, (float)imageSize.y );

            cv::circle( image, cv::Point((int)pos.x,(int)pos.y), size, color, -1 );
        }
    }
}

void DrawPath(cv::Mat& image, vec2 imageScale, const pointList& points, const graphPath& path, cv::Scalar lineColor)
{
    assert( path.size() >= 2 );

    for( size_t i=0; i<path.size()-1; i++ )
    {
        vertIndex i1 = path[i];
        vertIndex i2 = path[i+1];

        vec2 p1 = points[i1] * imageScale;
        vec2 p2 = points[i2] * imageScale;

        cv::line( image, cvPoint(p1), cvPoint(p2), lineColor );
    }
}
