#include "2702_proc.h"
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

using namespace std;

Mat outputImage;

pos fixImage (Mat inputImage)
{
    int erodeValue = 3;
    int dilatevalue = 3;

    Mat erodeElement = getStructuringElement(MORPH_RECT, Size(erodeValue,erodeValue));
    Mat dilateElement = getStructuringElement(MORPH_RECT, Size(dilatevalue,dilatevalue));

    dilate(inputImage, inputImage, dilateElement);
    erode(inputImage, inputImage, erodeElement);
    erode(inputImage, outputImage, erodeElement);
}

pos HSV_convert(Mat* img, int* args)
{
    Mat hsvconvert;
    Mat hsvconvert2;
    cvtColor(*img, hsvconvert, CV_BGR2HSV);
    if (args)
    {

        inRange(hsvconvert, Scalar(args[0], args[1], args[2]), Scalar(args[3], args[4], args[5]), hsvconvert2);
        fixImage(hsvconvert2);
        imshow("window", outputImage);
    }
    else
    {
        inRange(hsvconvert, cv::Scalar(54, 89, 32), cv::Scalar(91, 255, 255), hsvconvert2);
        fixImage(hsvconvert2);
        imshow("window",outputImage);
    }
    //threshold( hsvconvert, hsvconvert2, 1, 1, 1 );

    pos ret;
    ret.x =150;
    ret.y =346;
    return ret;

}

pos process(Mat* img, int* args)
{
    return HSV_convert(img, args);
}
