#include <Windows.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "Helpers.h"

using namespace cv;

void TestSimpleModification();
void TestDFT();
void TestVideo();
void TestErosion();

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    //TestSimpleModification();
    //TestDFT();
    //TestVideo();
    TestErosion();

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
Mat erosion_src, erosion_dst;
int erosion_elem = 0;
int erosion_size = 0;
int const max_elem = 2;
int const max_kernel_size = 21;
void Erosion(int, void*);

void TestErosion()
{
    // Load the original image.
    //erosion_src = imread( "Data/adult-city-female-1574140.jpg", IMREAD_COLOR );
    erosion_src = imread( "Data/action-adventure-backlit-209209.jpg", IMREAD_COLOR );

    // Create a window for erosion kernel settings.
    namedWindow( "Erosion Controls", WINDOW_KEEPRATIO );
    resizeWindow( "Erosion Controls", 450, 100 );
    moveWindow( "Erosion Controls", 50 + (450+10)*2, 550 );

    // Select Kernel shape and size, call Erosion() every time they change.    
    createTrackbar( "Shape:", "Erosion Controls", &erosion_elem, max_elem, Erosion ); //\n 0: Rect \n 1: Cross \n 2: Ellipse
    createTrackbar( "Size:", "Erosion Controls", &erosion_size, max_kernel_size, Erosion ); //\n 2n +1

    // Display the original and call Erosion() to display the initial result.
    ShowImageWithFixedWidthAtPosition( "Original", erosion_src, 450, 50, 50 );
    Erosion( 0, 0 );

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

void Erosion(int, void*)
{
    int erosion_type = MORPH_RECT;
    if( erosion_elem == 0 )         { erosion_type = MORPH_RECT; }
    else if( erosion_elem == 1 )    { erosion_type = MORPH_CROSS; }
    else if( erosion_elem == 2 )    { erosion_type = MORPH_ELLIPSE; }

    // Create the kernel.
    Mat kernel = getStructuringElement( erosion_type, Size( 2*erosion_size + 1, 2*erosion_size+1 ), Point( erosion_size, erosion_size ) ) * 255.0f;

    // Erode.
    erode( erosion_src, erosion_dst, kernel );

    // Display result and kernel.
    ShowImageWithFixedWidthAtPosition( "Eroded", erosion_dst, 450, 50 + 450+10, 50 );
    ShowImageWithFixedWidthAtPosition( "Kernel", kernel, 450, 50 + (450+10)*2, 50 );
}
