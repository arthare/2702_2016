#include "2702_proc.h"
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

using namespace std;

pos fixImage (Mat& inputImage, Mat& outputImage)
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
    Mat outputImage;
    if (args)
    {
        inRange(hsvconvert, Scalar(args[0], args[1], args[2]), Scalar(args[3], 255, 255), hsvconvert2);
        fixImage(hsvconvert2, outputImage);
        imshow("window", outputImage);
    }
    else
    {
        inRange(hsvconvert, Scalar(54, 89, 32), Scalar(91, 255, 255), hsvconvert2);
        fixImage(hsvconvert2, outputImage);
    }
    Moments center = moments(outputImage, true);
    pos ret;
    ret.x = center.m10/center.m00;
    ret.y = center.m01/center.m00;
    cout << ret.x << "," << ret.y << endl;
    return ret;

}

pos process(Mat* img, int* args)
{
    return HSV_convert(img, args);
}
