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
    // Load the original image.
    //Mat originalImage = imread( "Data/adult-city-female-1574140.jpg", IMREAD_GRAYSCALE );
    Mat originalImage = imread( "Data/action-adventure-backlit-209209.jpg", IMREAD_GRAYSCALE );

    // Generate DFT values.
    Mat dftValues;
    GenerateDFTFromGrayscaleImage( originalImage, dftValues );

    // Rebuild the image from the DFT values.
    Mat rebuiltImage;
    dft( dftValues, rebuiltImage, DFT_INVERSE | DFT_COMPLEX_INPUT | DFT_REAL_OUTPUT | DFT_SCALE );

    // Display the results.
    //ShiftTopLeftToCenter( originalImage );
    ShowImageWithFixedWidthAtPosition( "Original", originalImage, 450, 50, 50 );
    ShowDFTResult( "DFT", dftValues, 450, 50 + 450+10, 50 );
    ShowImageWithFixedWidthAtPosition( "Rebuilt Image", rebuiltImage, 450, 50 + (450+10)*2, 50 );
}
