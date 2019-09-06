#include <Windows.h>
#include "opencv2/opencv.hpp"

using namespace cv;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
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
        int displayWidth = 450;
        float ratio = (float)originalImage.rows / originalImage.cols;
        int displayHeight = (int)(displayWidth * ratio);

        namedWindow( "Original", WINDOW_KEEPRATIO );
        imshow( "Original", originalImage );
        resizeWindow( "Original", displayWidth, displayHeight );
        moveWindow( "Original", 350, 50 );

        namedWindow( "Modified", WINDOW_KEEPRATIO );
        imshow( "Modified", modifiedImage );
        resizeWindow( "Modified", displayWidth, displayHeight );
        moveWindow( "Modified", 350 + displayWidth+10, 50 );
    }

    // Save modified image to disk.
    imwrite( "Data/outfile.jpg", modifiedImage );

    waitKey();
}
