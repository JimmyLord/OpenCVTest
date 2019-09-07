#include <Windows.h>
#include "opencv2/opencv.hpp"
#include "Helpers.h"

using namespace cv;

void TestSimpleModification();
void TestDFT();

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    //TestSimpleModification();
    TestDFT();

    waitKey();
}

void TestSimpleModification()
{
    // Load the original image.
    //Mat originalImage = imread( "Data/adult-city-female-1574140.jpg", IMREAD_GRAYSCALE );
    //Mat originalImage = imread( "Data/adult-city-female-1574140.jpg", IMREAD_COLOR );
    Mat originalImage = imread( "Data/action-adventure-backlit-209209.jpg", IMREAD_COLOR );

    // Clone the original and modify it.
    Mat modifiedImage = originalImage.clone();

    // Zero out the blue channel.
    for( int x=0; x<originalImage.cols; x++ )
    {
        for( int y=0; y<originalImage.rows; y++ )
        {
            modifiedImage.at<Vec3b>( y, x )[0] = 0;
        }
    }

    // Display the two images, side by side.
    {
        ShowImageWithFixedWidthAtPosition( "Original", originalImage, 450, 350, 50 );
        ShowImageWithFixedWidthAtPosition( "Modified", modifiedImage, 450, 350 + 450+10, 50 );
    }

    // Save modified image to disk.
    imwrite( "Data/outfile.jpg", modifiedImage );
}

void TestDFT()
{
    Mat originalImage = imread( "Data/action-adventure-backlit-209209.jpg", IMREAD_GRAYSCALE );

    Mat originalImageAsFloats;

    originalImage.convertTo( originalImageAsFloats, CV_32FC1, 1.0f / 255.0f );

    Mat dftValues;

    TakeDFT( originalImageAsFloats, dftValues );
    ShowDFT( dftValues );
}
