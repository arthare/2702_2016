#include "2702_proc.h"
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

using namespace std;

pos HSV_convert(Mat* img, int* args)
{
    int erodeValue = 3;
    int dilatevalue = 3;
    Mat hsvconvert;
    Mat hsvconvert2;
    Mat hsvconvertE;
    Mat hsvconvertD;
    Mat erodeElement = getStructuringElement(MORPH_RECT, Size(erodeValue,erodeValue));
    Mat dilateElement = getStructuringElement(MORPH_RECT, Size(dilatevalue,dilatevalue));
    cvtColor(*img, hsvconvert, CV_BGR2HSV);
    if (args)
    {

        inRange(hsvconvert, Scalar(args[0], args[1], args[2]), Scalar(args[3], args[4], args[5]), hsvconvert2);
        dilate(hsvconvert2, hsvconvert2, dilateElement);
        erode(hsvconvert2, hsvconvert2, erodeElement);
        erode(hsvconvert2, hsvconvert2, erodeElement);
        dilate(hsvconvert2, hsvconvert2, dilateElement);
        imshow("window", hsvconvert2);
    }
    else
    {
        inRange(hsvconvert, cv::Scalar(54, 89, 32), cv::Scalar(91, 255, 255), hsvconvert2);
        dilate(hsvconvert2, hsvconvert2, dilateElement);
        erode(hsvconvert2, hsvconvert2, erodeElement);
        erode(hsvconvert2, hsvconvert2, erodeElement);
        dilate(hsvconvert2, hsvconvert2, dilateElement);
        imshow("window", hsvconvert2);
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
