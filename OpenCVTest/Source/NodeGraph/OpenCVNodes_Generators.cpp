//
// Copyright (c) 2021 Jimmy Lord
//
#include "OpenCVPCH.h"
#include "OpenCVNodes_Generators.h"
#include "Graph/GraphHelpers.h"

// Implementation of Robert Bridson's "Fast Poisson Disk Sampling in Arbitrary Dimensions".
// https://www.cs.ubc.ca/~rbridson/docs/bridson-siggraph07-poissondisk.pdf
// Implemented solely in 2D, which probably defeats the purpose.
void GenerateSampling(std::vector<vec2>& pointList, float minDistance, int maxSamplesPerPoint, bool startWithExistingPoints)
{
    if( minDistance < 0.0001 )
        return;

    float r = minDistance;

    vec2 pointSpaceSize( 100.0f, 100.0f );

    // Temp variables.
    std::vector<vec2> activeList;
    float cellSize = r / sqrtf(2);
    ivec2 gridSize( (int)ceil(pointSpaceSize.x / cellSize), (int)ceil(pointSpaceSize.y / cellSize) );
    std::vector<vec2> pointGrid( gridSize.x*gridSize.y );

    // Initialize the grid.
    for( int y=0; y<gridSize.y; y++ )
    {
        for( int x=0; x<gridSize.x; x++ )
        {
            pointGrid[y*gridSize.x + x].Set( -1, -1 );
        }
    }

    if( startWithExistingPoints )
    {
        // Start from end of list.
        for( size_t i=pointList.size(); i>0; i-- )
        {
            // Grad the old point.
            vec2 pos = pointList[i-1];
            int gx = (int)(pos.x / cellSize);
            int gy = (int)(pos.y / cellSize);

            // Push the sample into the active list and color the pixel.
            activeList.push_back( pos );
            pointGrid[gy*gridSize.x + gx] = pos;
        }
    }
    else
    {
        // Pick a random point.
        vec2 pos( randFloat(0,(float)pointSpaceSize.x), randFloat(0,(float)pointSpaceSize.y) );
        int gx = (int)(pos.x / cellSize);
        int gy = (int)(pos.y / cellSize);

        // Push the sample into the active list and color the pixel.
        activeList.push_back( pos );
        pointGrid[gy*gridSize.x + gx] = pos;
        pointList.clear();
        pointList.push_back( pos );
    }

    // Loop through active list.
    while( activeList.size() > 0 )
    {
        // Remove this sample from the active list.
        vec2 currentPos = activeList[0];
        activeList[0] = activeList[activeList.size()-1];
        activeList.pop_back();

        // Check a maximum number of samples around our current position.
        for( int i=0; i<maxSamplesPerPoint; i++ )
        {
            float angle = randFloat( 0, 2*PI );
            vec2 dir( cos(angle), sin(angle) );
            float dist = randFloat( r, 2*r );

            vec2 pos = currentPos + dir * dist;
            int ngx = (int)(pos.x / cellSize);
            int ngy = (int)(pos.y / cellSize);

            // Out of bounds check.
            if( pos.x < 0 || pos.x >= pointSpaceSize.x || pos.y < 0 || pos.y >= pointSpaceSize.y ||
                ngx < 0 || ngx >= gridSize.x || ngy < 0 || ngy >= gridSize.y )
            {
                continue;
            }

            // Check all neighbouring grid cells.
            bool tooClose = false;
            for( int y=-1; y<=1; y++ )
            {
                for( int x=-1; x<=1; x++ )
                {
                    ivec2 tile( ngx+x, ngy+y );
                    int tileIndex = tile.y*gridSize.x + tile.x;
                    if( tile.x < 0 || tile.x >= gridSize.x ) continue;
                    if( tile.y < 0 || tile.y >= gridSize.y ) continue;

                    if( pointGrid[tileIndex].x != -1 )
                    {
                        float distance = pointGrid[tileIndex].DistanceFrom( pos );

                        if( distance < r )
                        {
                            tooClose = true;
                        }
                    }
                }
            }

            // If we found a spot far enough from all others,
            // push the sample into the active list and color the pixel.
            if( tooClose == false )
            {
                activeList.push_back( pos );
                pointGrid[ngy*gridSize.x + ngx] = pos;
                pointList.push_back( pos );
                //cv::Vec3b color = cv::Vec3b( 255, 255, 255 );
                //if( palette != nullptr )
                //    color = (*palette)[pointList.size()];
                //cv::circle( image, cv::Point((int)pos.x,(int)pos.y), 2, color, 2 );
                //image.at<uchar>( cv::Point((int)pos.x,(int)pos.y) ) = 255;
            }
        }
    }
}

void DrawSampling(cv::Mat& image, ivec2 imageSize, const std::vector<vec2>& pointList, const colorPalette* palette, const std::vector<size_t> pointListLayerStarts)
{
    cv::Vec3b color = cv::Vec3b( 255, 255, 255 );

    bool colorEachVertex = true;
    if( pointList.size() > 20 )
        colorEachVertex = false;

    int layerCount = (int)pointListLayerStarts.size()-1;

    for( size_t i=pointList.size(); i>0; i-- )
    {
        if( i <= pointListLayerStarts[layerCount] )
        {
            layerCount--;
        }

        int size = 5 - layerCount*2;

        if( palette != nullptr )
        {
            if( colorEachVertex == true )
            {
                color = palette->at( i );
            }
            else
            {
                color = palette->at( layerCount+1 );
                size = 10 - layerCount*2;
                if( size < 2 )
                    size = 2;
            }
        }

        vec2 pos = pointList[i-1] / 100.0f * vec2( (float)imageSize.x, (float)imageSize.y );

        if( size < 1 )
            size = 1;
        cv::circle( image, cv::Point((int)pos.x,(int)pos.y), size, color, -1 );

        if( layerCount == 0 && colorEachVertex == false && pointList.size() < 50 )
        {
            if( palette )
            {
                color = palette->at( i );
            }
            cv::circle( image, cv::Point((int)pos.x,(int)pos.y), size+2, color, 2 );
        }
    }
}

// Modification of the above that takes in a grayscale image that controls point density.
void GenerateSamplingWithVaryingPointDensity(std::vector<vec2>& pointList, int maxSamplesPerPoint, cv::Mat& pointDensityImage, float minDistance, float maxDistance)
{
    float r = minDistance;

    if( maxDistance < minDistance )
        maxDistance = minDistance;

    int halfMaxCellSpan = (int)ceil( maxDistance/minDistance );

    vec2 pointSpaceSize( 100.0f, 100.0f );

    // Temp variables.
    std::vector<vec2> activeList;
    float cellSize = minDistance / sqrtf(2);
    ivec2 gridSize( (int)ceil(pointSpaceSize.x / cellSize), (int)ceil(pointSpaceSize.y / cellSize) );
    std::vector<vec2> pointGrid( gridSize.x*gridSize.y );

    // Initialize the grid.
    for( int y=0; y<gridSize.y; y++ )
    {
        for( int x=0; x<gridSize.x; x++ )
        {
            pointGrid[y*gridSize.x + x].Set( -1, -1 );
        }
    }

    // Pick a random point.
    vec2 pos( randFloat(0,(float)pointSpaceSize.x), randFloat(0,(float)pointSpaceSize.y) );
    int gx = (int)(pos.x / cellSize);
    int gy = (int)(pos.y / cellSize);

    // Push the sample into the active list and color the pixel.
    activeList.push_back( pos );
    pointGrid[gy*gridSize.x + gx] = pos;
    pointList.clear();
    pointList.push_back( pos );
    //cv::Vec3b color = cv::Vec3b( 255, 255, 255 );
    //if( palette != nullptr )
    //    color = (*palette)[1];
    //cv::circle( image, cv::Point((int)pos.x,(int)pos.y), 2, color, 2 );
    //image.at<uchar>( cv::Point((int)pos.x,(int)pos.y) ) = 255;

    // Loop through active list.
    while( activeList.size() > 0 )
    {
        // Remove this sample from the active list.
        vec2 currentPos = activeList[0];
        activeList[0] = activeList[activeList.size()-1];
        activeList.pop_back();

        // Check a maximum number of samples around our current position.
        for( int i=0; i<maxSamplesPerPoint; i++ )
        {
            float angle = randFloat( 0, 2*PI );
            vec2 dir( cos(angle), sin(angle) );
            
            // Determine the desired density around the pixel we're currently on.
            float perc = 1.0f;
            if( pointDensityImage.cols != 0 )
            {
                int gray = pointDensityImage.at<uchar>( cv::Point((int)currentPos.x,(int)currentPos.y) );
                perc = gray / 255.0f;
            }
            float DesiredDistance = minDistance + (maxDistance - minDistance) * perc;

            float dist = randFloat( DesiredDistance, 2*DesiredDistance );

            pos = currentPos + dir * dist;
            int ngx = (int)(pos.x / cellSize);
            int ngy = (int)(pos.y / cellSize);

            // Out of bounds check.
            if( pos.x < 0 || pos.x >= pointSpaceSize.x || pos.y < 0 || pos.y >= pointSpaceSize.y ||
                ngx < 0 || ngx >= gridSize.x || ngy < 0 || ngy >= gridSize.y )
            {
                continue;
            }

            // Check all neighbouring grid cells.
            bool tooClose = false;
            for( int y=-halfMaxCellSpan; y<=halfMaxCellSpan; y++ )
            {
                for( int x=-halfMaxCellSpan; x<=halfMaxCellSpan; x++ )
                {
                    ivec2 tile( ngx+x, ngy+y );
                    int tileIndex = tile.y*gridSize.x + tile.x;
                    if( tile.x < 0 || tile.x >= gridSize.x ) continue;
                    if( tile.y < 0 || tile.y >= gridSize.y ) continue;

                    if( pointGrid[tileIndex].x != -1 )
                    {
                        float distance = pointGrid[tileIndex].DistanceFrom( pos );

                        if( distance < DesiredDistance )
                        {
                            tooClose = true;
                        }
                    }
                }
            }

            // If we found a spot far enough from all others,
            // push the sample into the active list and color the pixel.
            if( tooClose == false )
            {
                activeList.push_back( pos );
                pointGrid[ngy*gridSize.x + ngx] = pos;
                pointList.push_back( pos );
                //cv::Vec3b color = cv::Vec3b( 255, 255, 255 );
                //if( palette != nullptr )
                //    color = (*palette)[pointList.size()];
                //cv::circle( image, cv::Point((int)pos.x,(int)pos.y), 2, color, 2 );
                //image.at<uchar>( cv::Point((int)pos.x,(int)pos.y) ) = 255;
            }
        }
    }
}

void GenerateGrid(std::vector<vec2>& pointList, fullNeighbourList& neighbours, ivec2 gridSize, float padding, bool connectDiagonals)
{
    if( gridSize.x < 2 )
        gridSize.x = 2;

    if( gridSize.y < 2 )
        gridSize.y = 2;

    pointList.clear();
    neighbours.clear();

    pointList.resize( gridSize.x * gridSize.y );
    neighbours.resize( gridSize.x * gridSize.y );

    vec2 offset = padding/2;
    vec2 step = (100.0f - padding) / (gridSize-1);

    //// Plot the 4 corners first.
    //pointList[0].Set( offset.x + step.x*0,              offset.y + step.y*0 ); // bl
    //pointList[1].Set( offset.x + step.x*0,              offset.y + step.y*(gridSize.y-1) ); // tl
    //pointList[2].Set( offset.x + step.x*(gridSize.x-1), offset.y + step.y*(gridSize.y-1) ); // tr
    //pointList[3].Set( offset.x + step.x*(gridSize.x-1), offset.y + step.y*0 ); // br

    // Plot the rest.
    int index = 0;
    for( int y=0; y<gridSize.y; y++ )
    {
        for( int x=0; x<gridSize.x; x++ )
        {
            //if( x == 0 && y == 0 ) continue;
            //if( x == 0 && y == gridSize.y-1 ) continue;
            //if( x == gridSize.x-1 && y == gridSize.y-1 ) continue;
            //if( x == gridSize.x-1 && y == 0 ) continue;

            pointList[index].Set( offset.x + step.x*x, offset.y + step.y*y );

            int mx = gridSize.x-1;
            int my = gridSize.y-1;

            if( x>0 )  { neighbours[index].insert( (y+0)*gridSize.x + (x-1) ); }
            if( x<mx ) { neighbours[index].insert( (y+0)*gridSize.x + (x+1) ); }
            if( y>0 )  { neighbours[index].insert( (y-1)*gridSize.x + (x+0) ); }
            if( y<my ) { neighbours[index].insert( (y+1)*gridSize.x + (x+0) ); }

            if( connectDiagonals )
            {
                if( x>0  && y>0  ) { neighbours[index].insert( (y-1)*gridSize.x + (x-1) ); }
                if( x>0  && y<my ) { neighbours[index].insert( (y+1)*gridSize.x + (x-1) ); }
                if( x<mx && y>0  ) { neighbours[index].insert( (y-1)*gridSize.x + (x+1) ); }
                if( x<mx && y<my ) { neighbours[index].insert( (y+1)*gridSize.x + (x+1) ); }
            }

            index++;
        }
    }

    // Swap the corners to the first 4 indices
    int indexBL = 0;
    int indexTL = (gridSize.y-1)*gridSize.x;
    int indexTR = (gridSize.y-1)*gridSize.x + gridSize.x-1;
    int indexBR = gridSize.x-1;

    Graph_SwapIndex( pointList, neighbours, 0, indexBL );
    Graph_SwapIndex( pointList, neighbours, 1, indexTL );
    Graph_SwapIndex( pointList, neighbours, 2, indexTR );
    Graph_SwapIndex( pointList, neighbours, 3, indexBR );
}
