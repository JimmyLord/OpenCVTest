#include "opencv2/opencv.hpp"

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
// https://www.youtube.com/playlist?list=PLAp0ZhYvW6XbEveYeefGSuLhaPlFML9gP
void TakeDFT(cv::Mat& image, cv::Mat& output)
{
    Mat originalComplex[2] = { image, Mat::zeros( image.size(), CV_32F ) };

    Mat mergedComplex;

    merge( originalComplex, 2, mergedComplex );

    Mat finalDFT;

    dft( mergedComplex, finalDFT, DFT_COMPLEX_OUTPUT );

    output = finalDFT; 
}

void ShowDFT(cv::Mat& image)
{
    Mat splitArray[2] = { Mat::zeros( image.size(), CV_32F ), Mat::zeros( image.size(), CV_32F ) };

    split( image, splitArray );

    Mat magnitudeOfDFT;

    magnitude( splitArray[0], splitArray[1], magnitudeOfDFT );

    magnitudeOfDFT += Scalar::all(1);

    log( magnitudeOfDFT, magnitudeOfDFT );

    normalize( magnitudeOfDFT, magnitudeOfDFT, 0, 1, NORM_MINMAX );

    //imshow( "DFT", magnitudeOfDFT );

    ShowImageWithFixedWidthAtPosition( "DFT", magnitudeOfDFT, 450, 350, 50 );
}