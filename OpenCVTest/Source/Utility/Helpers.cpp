//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#include "OpenCVPCH.h"

#include "Libraries/Framework/MyFramework/SourceCommon/Renderers/OpenGL/Texture_OpenGL.h"
#include "Helpers.h"

using namespace cv;

float randFloat(float min, float max)
{
    float r01 = rand()/(float)RAND_MAX;
    return min + (max - min) * r01;
}

size_t randSizeT(size_t min, size_t max)
{
    float r01 = rand()/(float)RAND_MAX;
    return min + (int)round((max - min) * r01);
}

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

uint32 NextPowerOfTwo(int value)
{
    if( value < 0 )
        return 0;

    int power = 2;
    value--;
    while( value >>= 1 )
        power <<= 1;

    return power;
}

// From https://gist.github.com/zhangzhensong/03f67947c22acb5ee922
void BindCVMat2GLTexture(cv::Mat& image, GLuint& imageTexture, uint32* w, uint32* h)
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

        // Pad the image out to the nearest power or two.
        uint32 pow2cols = NextPowerOfTwo( image.cols );
        uint32 pow2rows = NextPowerOfTwo( image.rows );
        cv::Mat temp;
        cv::copyMakeBorder( image, temp, 0, pow2rows - image.rows, 0, pow2cols - image.cols, BORDER_CONSTANT );

        int type = image.type();
        if( type == CV_8UC3 )
        {
            // Convert from BGR to RGB.
            cv::cvtColor( temp, temp, COLOR_BGR2RGB );
        }
        else if( type == CV_32F )
        {
            double min;
            double max;
            cv::minMaxLoc( image, &min, &max );
            float scale = 255.0f / (float)(max - min);
            temp.convertTo( temp, CV_8UC1, scale );
            cv::cvtColor( temp, temp, COLOR_GRAY2RGB );
            //applyColorMap( temp, temp, cv::COLORMAP_JET );
        }
        else if( type == CV_8U )
        {
            // Convert from gray to RGB.
            cv::cvtColor( temp, temp, COLOR_GRAY2RGB );
        }
        else
        {
        }

        glTexImage2D( GL_TEXTURE_2D,      // Type of texture
                      0,                  // Pyramid level (for mip-mapping) - 0 is the top level
                      GL_RGB,             // Internal colour format to convert to
                      temp.cols,          // Image width
                      temp.rows,          // Image height
                      0,                  // Border width in pixels (can either be 1 or 0)
                      GL_RGB,             // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                      GL_UNSIGNED_BYTE,   // Image data type
                      temp.ptr() );       // The actual image data itself

        if( w ) *w = pow2cols;
        if( h ) *h = pow2rows;
    }
}

void CopyFBOToCVMat(FBODefinition* pFBO, cv::Mat& dest, bool bindFBO)
{
    int cols = pFBO->GetWidth();
    int rows = pFBO->GetHeight();

    int FBOsize = pFBO->GetTextureWidth() * pFBO->GetTextureHeight() * 3;
    uint8* pixels = MyNew uint8[FBOsize];

    if( bindFBO ) pFBO->Bind( true );
    g_pRenderer->ReadPixels( 0, 0, cols, rows, MyRE::PixelFormat_BGR, MyRE::PixelDataType_UByte, pixels );
    if( bindFBO ) pFBO->Unbind( true );

    Mat temp( rows, cols, CV_8UC3 );

    int CVStride = (int)temp.step.buf[0];
    int GLStride = (cols*3 + 3) & ~0x03;
    for( int y=0; y<rows; y++ )
    {
        memcpy( &temp.data[y*CVStride], &pixels[y*GLStride], cols*3 );
    }

    delete[] pixels;

    dest = temp;
}

TextureDefinition* CreateOrUpdateTextureDefinitionFromOpenCVMat(cv::Mat* pImage, TextureDefinition* pOldTexture)
{
    if( pImage->empty() )
        return pOldTexture;

    GLuint textureID = 0;
    if( pOldTexture != nullptr )
        textureID = ((Texture_OpenGL*)pOldTexture)->GetTextureID();

    unsigned int w, h;
    BindCVMat2GLTexture( *pImage, textureID, &w, &h );

    if( pOldTexture == nullptr )
        pOldTexture = MyNew Texture_OpenGL( textureID );

    ((Texture_OpenGL*)pOldTexture)->SetWidth( w );
    ((Texture_OpenGL*)pOldTexture)->SetHeight( h );

    return pOldTexture;
}

void DisplayOpenCVMatAndTexture(cv::Mat* pImage, TextureDefinition* pTexture, int width, float pixelsToShow, HoverCallbackFunc pHoverCallbackFunc)
{
    if( pTexture != nullptr && pImage->empty() == false )
    {
        float aspect = (float)pImage->rows / pImage->cols;
        uint32 pow2cols = NextPowerOfTwo( pImage->cols );
        uint32 pow2rows = NextPowerOfTwo( pImage->rows );

        ImVec2 pos = ImGui::GetCursorScreenPos();

        ImVec2 uvMax = ImVec2( (float)pImage->cols / pow2cols, (float)pImage->rows / pow2rows );
        ImGui::Image( (void*)pTexture, ImVec2( (float)width, width*aspect ), ImVec2( 0, 0 ), uvMax );

        if( ImGui::IsItemHovered() )
        {
            ImGuiIO& io = ImGui::GetIO();

            ImGui::BeginTooltip();

            Vector2 textureSize = Vector2( (float)pow2cols, (float)pow2rows );
            Vector2 imageSizeNative = Vector2( (float)pImage->cols, (float)pImage->rows );
            Vector2 regionSizeNative = Vector2( pixelsToShow, pixelsToShow*aspect );
            Vector2 viewportSizeNative = Vector2( (float)width, width*aspect );

            Vector2 regionCenterViewport( io.MousePos.x - pos.x, io.MousePos.y - pos.y );
            Vector2 regionCenterNative = regionCenterViewport / viewportSizeNative * imageSizeNative;
            Vector2 regionBLNative = regionCenterNative - regionSizeNative/2;
            MyClamp( regionBLNative.x, 0.0f, imageSizeNative.x - regionSizeNative.x );
            MyClamp( regionBLNative.y, 0.0f, imageSizeNative.y - regionSizeNative.y );

            Vector2 uv0 = regionBLNative / textureSize;
            Vector2 uv1 = uv0 + regionSizeNative / textureSize;

            int type = pImage->type();
            if( type == CV_8UC3 )
            {
                Vec3b color = pImage->at<Vec3b>( (int)regionCenterNative.y, (int)regionCenterNative.x );
                ImGui::Text( "(%d,%d) %d,%d,%d", (int)regionCenterNative.x, (int)regionCenterNative.y, color[2], color[1], color[0] );
            }
            else if( type == CV_32F )
            {
                float value = pImage->at<float>( (int)regionCenterNative.y, (int)regionCenterNative.x );
                ImGui::Text( "(%d,%d) %f", (int)regionCenterNative.x, (int)regionCenterNative.y, value );
            }
            else
            {
                uint8 intensity = pImage->at<uchar>( (int)regionCenterNative.y, (int)regionCenterNative.x );
                ImGui::Text( "(%d,%d) %d", (int)regionCenterNative.x, (int)regionCenterNative.y, intensity );
            }

            ImGui::Image( (void*)pTexture, ImVec2( 128, 128 * aspect ), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,128));

            if( pHoverCallbackFunc != nullptr )
            {
                pHoverCallbackFunc( regionCenterNative );
            }
            
            ImGui::EndTooltip();
        }
    }
}

void PrintFloatBadlyWithPrecision(std::string& str, float value, int maxDecimalPlaces)
{
    char tempFormat[8];
    sprintf_s( tempFormat, 8, "%%0.%df", maxDecimalPlaces );

    char tempResult[32];
    sprintf_s( tempResult, 32, tempFormat, value );

    str += tempResult;

    str.erase( str.find_last_not_of('0') + 1, std::string::npos );
    str.erase( str.find_last_not_of('.') + 1, std::string::npos );
    replace( str.begin(), str.end(), '.', '_' );
}

std::vector<cv::Vec3b> GeneratePalette()
{
    std::vector<cv::Vec3b> palette;
    palette.push_back( cv::Vec3b( 30, 30, 30 ) );

    palette.push_back( cv::Vec3b( 128, 0, 0 ) );
    palette.push_back( cv::Vec3b( 0, 128, 0 ) );
    palette.push_back( cv::Vec3b( 0, 0, 128 ) );
    palette.push_back( cv::Vec3b( 128, 128, 0 ) );
    palette.push_back( cv::Vec3b( 128, 0, 128 ) );
    palette.push_back( cv::Vec3b( 0, 128, 128 ) );
    palette.push_back( cv::Vec3b( 128, 128, 128 ) );

    palette.push_back( cv::Vec3b( 192, 0, 0 ) );
    palette.push_back( cv::Vec3b( 0, 192, 0 ) );
    palette.push_back( cv::Vec3b( 0, 0, 192 ) );
    palette.push_back( cv::Vec3b( 192, 192, 0 ) );
    palette.push_back( cv::Vec3b( 192, 0, 192 ) );
    palette.push_back( cv::Vec3b( 0, 192, 192 ) );
    palette.push_back( cv::Vec3b( 192, 192, 192 ) );

    palette.push_back( cv::Vec3b( 255, 0, 0 ) );
    palette.push_back( cv::Vec3b( 0, 255, 0 ) );
    palette.push_back( cv::Vec3b( 0, 0, 255 ) );
    palette.push_back( cv::Vec3b( 255, 255, 0 ) );
    palette.push_back( cv::Vec3b( 255, 0, 255 ) );
    palette.push_back( cv::Vec3b( 0, 255, 255 ) );
    palette.push_back( cv::Vec3b( 255, 255, 255 ) );

    for( int i=0; i<1000; i++ )
    {
        palette.push_back( cv::Vec3b( rand()%255, rand()%255, rand()%255 ) );
    }

    return palette;
}

cv::Vec3f AsVec3f(Vector3 v)
{
    return cv::Vec3f( v.x, v.y, v.z );
}

cv::Vec3b AsVec3b(Vector3 v)
{
    return cv::Vec3b( (char)v.x, (char)v.y, (char)v.z );
}

Vector3 AsVec3(cv::Vec3f v)
{
    return Vector3( v[0], v[1], v[2] );
}

Vector3 AsVec3(cv::Vec3b v)
{
    return Vector3( v[0], v[1], v[2] );
}
