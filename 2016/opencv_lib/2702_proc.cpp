#include "2702_proc.h"
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

using namespace std;

pos HSV_convert(Mat* img, int* args)
{
    Mat hsvconvert;
    Mat hsvconvert2;
    cvtColor(*img, hsvconvert, CV_BGR2GRAY );
    if (args)
    {

        inRange(hsvconvert, cv::Scalar(args[0], args[1], args[2]), cv::Scalar(args[3], args[4], args[5]), hsvconvert2);
        imshow("window", hsvconvert2);
    }
    else
    {
        inRange(hsvconvert, cv::Scalar(232, 0, 0), cv::Scalar(255, 0, 0), hsvconvert2);
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
