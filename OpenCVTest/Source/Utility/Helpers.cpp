#include "OpenCVPCH.h"

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
