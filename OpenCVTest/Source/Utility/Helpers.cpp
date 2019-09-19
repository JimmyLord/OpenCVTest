//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#include "OpenCVPCH.h"

#include "Libraries/Framework/MyFramework/SourceCommon/Renderers/OpenGL/Texture_OpenGL.h"
#include "Helpers.h"

using namespace cv;

void ShowImageWithFixedWidthAtPosition(const cv::String windowName, cv::Mat& image, int width, int posX, int posY)
{
    int displayWidth = width;
    float ratio = (float)image.rows / image.cols;
    int displayHeight = (int)(displayWidth * ratio);

    namedWindow( windowName, WINDOW_KEEPRATIO );
    imshow( windowName, image );
    resizeWindow( windowName, displayWidth, displayHeight );
    moveWindow( windowName, posX, posY );
}

// Following tutorials by George Lecakes at:
//   https://www.youtube.com/playlist?list=PLAp0ZhYvW6XbEveYeefGSuLhaPlFML9gP
// And OpenCV Tutorials here:
//   https://docs.opencv.org/3.4/d8/d01/tutorial_discrete_fourier_transform.html
void GenerateDFTFromGrayscaleImage(cv::Mat& image, cv::Mat& output)
{
    Mat imageAsFloats;
    image.convertTo( imageAsFloats, CV_32FC1, 1.0f / 255.0f );
    dft( imageAsFloats, output, DFT_COMPLEX_OUTPUT );
}

void ShowDFTResult(const cv::String windowName, cv::Mat& image, int width, int posX, int posY)
{
    // Split the DFT image into 2 arrays. [0] is Real numbers, [1] is Complex/Imaginary numbers
    Mat splitArray[2] = { Mat::zeros( image.size(), CV_32F ), Mat::zeros( image.size(), CV_32F ) };
    split( image, splitArray );

    // Get the magnitude of the numbers. i.e. sqrt( real^2 + complex^2 );
    Mat magnitudeOfDFT;
    magnitude( splitArray[0], splitArray[1], magnitudeOfDFT );

    // Convert magnitude values to a logarithmic scale. i.e. newValue = log( 1 + oldValue );
    magnitudeOfDFT += Scalar::all(1);
    log( magnitudeOfDFT, magnitudeOfDFT );

    // Normalize the linear magnitude values.
    normalize( magnitudeOfDFT, magnitudeOfDFT, 0, 1, NORM_MINMAX );

    ShiftTopLeftToCenter( magnitudeOfDFT );

    // Display the image.
    ShowImageWithFixedWidthAtPosition( windowName, magnitudeOfDFT, width, posX, posY );
}

void ShiftTopLeftToCenter(cv::Mat& image)
{
    // Rearrange the quadrants of the image so that the origin is at the center.
    int cx = image.cols/2;
    int cy = image.rows/2;

    Mat q0( image, Rect(0, 0, cx, cy) );   // Top-Left.
    Mat q1( image, Rect(cx, 0, cx, cy) );  // Top-Right.
    Mat q2( image, Rect(0, cy, cx, cy) );  // Bottom-Left.
    Mat q3( image, Rect(cx, cy, cx, cy) ); // Bottom-Right.

    Mat tmp;
    // Swap quadrants (Top-Left with Bottom-Right).
    q0.copyTo( tmp );
    q3.copyTo( q0 );
    tmp.copyTo( q3 );

    // Swap quadrant (Top-Right with Bottom-Left).
    q1.copyTo( tmp );
    q2.copyTo( q1 );
    tmp.copyTo( q2 );
}

int NextPowerOfTwo(int value)
{
    int power = 2;
    value--;
    while( value >>= 1 )
        power <<= 1;

    return power;
}

// From https://gist.github.com/zhangzhensong/03f67947c22acb5ee922
void BindCVMat2GLTexture(cv::Mat& image, GLuint& imageTexture)
{
    if( image.empty() )
    {
        //std::cout << "image empty" << std::endl;
    }
    else
    {
        //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

        if( imageTexture == 0 )
        {
            glGenTextures( 1, &imageTexture );
        }

        glBindTexture( GL_TEXTURE_2D, imageTexture );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

        int pow2cols = NextPowerOfTwo( image.cols );
        int pow2rows = NextPowerOfTwo( image.rows );

        cv::Mat temp;
        cv::copyMakeBorder( image, temp, 0, pow2rows - image.rows, 0, pow2cols - image.cols, BORDER_CONSTANT );

        glTexImage2D( GL_TEXTURE_2D,      // Type of texture
            0,                  // Pyramid level (for mip-mapping) - 0 is the top level
            GL_RGB,             // Internal colour format to convert to
            temp.cols,          // Image width
            temp.rows,          // Image height
            0,                  // Border width in pixels (can either be 1 or 0)
            GL_BGR,             // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
            GL_UNSIGNED_BYTE,   // Image data type
            temp.ptr() );       // The actual image data itself
    }
}

TextureDefinition* CreateOrUpdateTextureDefinitionFromOpenCVMat(cv::Mat* pImage, TextureDefinition* pOldTexture)
{
    if( pImage->empty() )
        return pOldTexture;

    GLuint textureID = 0;
    if( pOldTexture != nullptr )
        textureID = ((Texture_OpenGL*)pOldTexture)->GetTextureID();

    BindCVMat2GLTexture( *pImage, textureID );

    if( pOldTexture == nullptr )
        pOldTexture = MyNew Texture_OpenGL( textureID );

    return pOldTexture;
}

void DisplayOpenCVMatAndTexture(cv::Mat* pImage, TextureDefinition* pTexture, float size)
{
    if( pTexture != nullptr )
    {
        float aspect = (float)pImage->rows / pImage->cols;
        int pow2cols = NextPowerOfTwo( pImage->cols );
        int pow2rows = NextPowerOfTwo( pImage->rows );
        ImGui::Image( (void*)pTexture, ImVec2( size, size*aspect ),
            ImVec2( 0, 0 ), ImVec2( (float)pImage->cols / pow2cols, (float)pImage->rows / pow2rows ) );
    }
}
