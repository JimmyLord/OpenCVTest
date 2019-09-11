#include <Windows.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "Helpers.h"

using namespace cv;

void TestSimpleModification();
void TestDFT();
void TestVideo();
void TestMorph();
void TestThreshhold();

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    //TestSimpleModification();
    //TestDFT();
    //TestVideo();
    //TestMorph();
    TestThreshhold();

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

void TestVideo()
{
    VideoCapture vid( 0 );

    if( vid.isOpened() == false )
    {
        return;
    }

    while( true )
    {
        Mat image;
        vid.read( image );
        imshow( "Video", image );

        if( waitKey( 1000/10 ) > 0 )
            break;
    }
}

// Following tutorial by Ana Huamán here:
//   https://docs.opencv.org/master/db/df6/tutorial_erosion_dilatation.html

// Globals for Trackbar settings.
Mat g_MorphSrc, g_MorphDst;
int const g_TBNumMorphTypes = 7; // Don't allow "hit or miss" for now.
int g_TBMorphType = 0;
int const g_TBNumKernelShapes = 3;
int g_TBKernelShape = 0;
int g_TBKernelSize = 0;
int const g_MaxKernelSize = 21;
void Morph(int, void*);

void TestMorph()
{
    // Load the original image.
    //g_MorphSrc = imread( "Data/adult-city-female-1574140.jpg", IMREAD_COLOR );
    g_MorphSrc = imread( "Data/action-adventure-backlit-209209.jpg", IMREAD_COLOR );

    // Create a window for erosion kernel settings.
    namedWindow( "Erosion Controls", WINDOW_KEEPRATIO );
    resizeWindow( "Erosion Controls", 450, 100 );
    moveWindow( "Erosion Controls", 50 + (450+10)*2, 550 );

    // Select Kernel shape and size, call Erosion() every time they change.    
    createTrackbar( "Morph:", "Erosion Controls", &g_TBMorphType, g_TBNumMorphTypes-1, Morph ); //\n 0: Rect \n 1: Cross \n 2: Ellipse
    createTrackbar( "Shape:", "Erosion Controls", &g_TBKernelShape, g_TBNumKernelShapes-1, Morph ); //\n 0: Rect \n 1: Cross \n 2: Ellipse
    createTrackbar( "Size:", "Erosion Controls", &g_TBKernelSize, g_MaxKernelSize-1, Morph ); //\n 2n +1

    // Display the original and call Erosion() to display the initial result.
    ShowImageWithFixedWidthAtPosition( "Original", g_MorphSrc, 450, 50, 50 );
    Morph( 0, 0 );

    // Other test code with very low res image and kernel.
    if( false )
    {
        Mat image = getStructuringElement( MORPH_ELLIPSE, Size( 20, 20 ) ) * 255.0f;
        Mat kernel = getStructuringElement( MORPH_CROSS, Size( 3, 3 ) ) * 255.0f;

        Mat output;
        erode( image, output, kernel );
        //dilate( image, output, kernel );

        ShowImageWithFixedWidthAtPosition( "TestOriginal", image, 450, 50, 50 );
        ShowImageWithFixedWidthAtPosition( "TestEroded", output, 450, 50 + 450+10, 50 );
        ShowImageWithFixedWidthAtPosition( "TestKernel", kernel, 450, 50 + (450+10)*2, 50 );
    }
}

void Morph(int, void*)
{
    int morphType = MORPH_ERODE;
    if( g_TBMorphType == 0 )        { morphType = MORPH_ERODE; }
    else if( g_TBMorphType == 1 )   { morphType = MORPH_DILATE; }
    else if( g_TBMorphType == 2 )   { morphType = MORPH_OPEN; }
    else if( g_TBMorphType == 3 )   { morphType = MORPH_CLOSE; }
    else if( g_TBMorphType == 4 )   { morphType = MORPH_GRADIENT; }
    else if( g_TBMorphType == 5 )   { morphType = MORPH_TOPHAT; }
    else if( g_TBMorphType == 6 )   { morphType = MORPH_BLACKHAT; }
    //else if( g_TBMorphType == 7 )   { morphType = MORPH_HITMISS; }

    int kernelShape = MORPH_RECT;
    if( g_TBKernelShape == 0 )      { kernelShape = MORPH_RECT; }
    else if( g_TBKernelShape == 1 ) { kernelShape = MORPH_CROSS; }
    else if( g_TBKernelShape == 2 ) { kernelShape = MORPH_ELLIPSE; }

    // Create the kernel.
    Mat kernel = getStructuringElement( kernelShape, Size( 2*g_TBKernelSize+1, 2*g_TBKernelSize+1 ), Point( g_TBKernelSize, g_TBKernelSize ) ) * 255.0f;

    // Apply the morphological transformation.
    morphologyEx( g_MorphSrc, g_MorphDst, morphType, kernel );

    // Display result and kernel.
    ShowImageWithFixedWidthAtPosition( "Output", g_MorphDst, 450, 50 + 450+10, 50 );
    ShowImageWithFixedWidthAtPosition( "Kernel", kernel, 450, 50 + (450+10)*2, 50 );
}

// Globals for Trackbar settings.
Mat g_ThreshholdSrc, g_ThreshholdDst;
int const g_TBNumThreshholdTypes = 5; // Don't allow "hit or miss" for now.
int g_TBThreshholdType = 0;
int g_TBThreshholdValue = 0;
void Threshhold(int, void*);

void TestThreshhold()
{
    // Load the original image.
    //g_ThreshholdSrc = imread( "Data/adult-city-female-1574140.jpg", IMREAD_GRAYSCALE );
    g_ThreshholdSrc = imread( "Data/action-adventure-backlit-209209.jpg", IMREAD_GRAYSCALE );

    // Create a window for erosion kernel settings.
    namedWindow( "Threshhold Controls", WINDOW_KEEPRATIO );
    resizeWindow( "Threshhold Controls", 450, 100 );
    moveWindow( "Threshhold Controls", 50 + (450+10)*2, 550 );

    // Select Threshhold type and max value, call Threshhold() every time they change.    
    createTrackbar( "Type:", "Threshhold Controls", &g_TBThreshholdType, g_TBNumThreshholdTypes-1, Threshhold );
    createTrackbar( "Value:", "Threshhold Controls", &g_TBThreshholdValue, 255, Threshhold );

    // Display the original and call Erosion() to display the initial result.
    ShowImageWithFixedWidthAtPosition( "Original", g_ThreshholdSrc, 450, 50, 50 );
    Threshhold( 0, 0 );
}

void Threshhold(int, void*)
{
    int threshholdType = THRESH_BINARY;
    if( g_TBThreshholdType == 0 )        { threshholdType = THRESH_BINARY; }
    else if( g_TBThreshholdType == 1 )   { threshholdType = THRESH_BINARY_INV; }
    else if( g_TBThreshholdType == 2 )   { threshholdType = THRESH_TRUNC; }
    else if( g_TBThreshholdType == 3 )   { threshholdType = THRESH_TOZERO; }
    else if( g_TBThreshholdType == 4 )   { threshholdType = THRESH_TOZERO_INV; }
    //else if( g_TBThreshholdType == 5 )   { threshholdType = THRESH_MASK; }

    // Apply the threshold.
    threshold( g_ThreshholdSrc, g_ThreshholdDst, g_TBThreshholdValue, 255, threshholdType );
    //threshold( g_ThreshholdSrc, g_ThreshholdDst, g_TBThreshholdValue, 255, threshholdType | THRESH_OTSU );
    //threshold( g_ThreshholdSrc, g_ThreshholdDst, g_TBThreshholdValue, 255, threshholdType | THRESH_TRIANGLE );

    // Display result and kernel.
    ShowImageWithFixedWidthAtPosition( "Output", g_ThreshholdDst, 450, 50 + 450+10, 50 );
}
