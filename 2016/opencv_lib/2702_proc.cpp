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

pos findDrawContours(Mat& inputImage)
{
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat result(inputImage);
    int contourIdx=-1;
    int thickness=-1;
    int lineType=8;
    int maxLevel=0;
    findContours(inputImage, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
    drawContours(result , contours, contourIdx, Scalar(0, 255, 0), thickness, lineType, hierarchy, maxLevel);
    imshow("contours", result);
    //cout << "contours.size =" << contours.size() << endl;
    //cout.flush();
    if( contours.size() != 0 )
    {
        //double value = matchShapes(inputImage, contours[1], CV_CONTOURS_MATCH_I1, 0);
        //cout << value << endl;
    }
    for(int i = 0; i < contours.size(); i++)
    {
        double value = matchShapes(inputImage, contours[i], CV_CONTOURS_MATCH_I1, 0);
        cout << "contours.size =" << contours.size() << endl;
        cout << "current contour =" << i << endl;
        cout << ""
        cout.flush();
    }



}

pos HSV_convert(Mat* img, int* args)
{
    Mat hsvconvert;
    Mat hsvconvert2;
    Mat outputImage;
    cvtColor(*img, hsvconvert, CV_BGR2HSV);
    if (args)
    {
        inRange(hsvconvert, Scalar(args[0], args[1], args[2]), Scalar(args[3], 255, 255), hsvconvert2);
        fixImage(hsvconvert2, outputImage);
        imshow("window", outputImage);
        findDrawContours(outputImage);
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
