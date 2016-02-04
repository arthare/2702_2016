#include "2702_proc.h"
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <dirent.h>

#include <sys/stat.h>

using namespace cv;

using namespace std;

Mat templ;
void drawCircle (Mat& img, Mat& result, Point matchLoc)
{
        circle(result, matchLoc, 20, Scalar(0, 255, 255));
        circle(result, matchLoc, 19, Scalar(255, 255, 255));

        imshow("window", result);

        circle(img, matchLoc, 20, Scalar(0, 255, 255));
        circle(img, matchLoc, 19, Scalar(255, 255, 255));

        imshow("window2", img);
}

void dumptuff ()
{
    cout << "CV_8UC1 "<<CV_8UC1<<endl;
    cout << "CV_8UC2 "<<CV_8UC2<<endl;
    cout << "CV_8UC3 "<< CV_8UC3<<endl;
    cout << "CV_8UC4 "<< CV_8UC4<<endl;

    cout << "CV_8SC1 "<< CV_8SC1<<endl;
    cout << "CV_8SC2 "<< CV_8SC2<<endl;
    cout << "CV_8SC3 "<< CV_8SC3<<endl;
    cout << "CV_8SC4 "<< CV_8SC4<<endl;

    cout << "CV_16UC1 "<< CV_16UC1<<endl;
    cout << "CV_16UC2 "<< CV_16UC2<<endl;
    cout << "CV_16UC3 "<< CV_16UC3<<endl;
    cout << "CV_16UC4 "<<CV_16UC4<<endl;

    cout << "CV_16SC1 "<< CV_16SC1<<endl;
    cout << "CV_16SC2 "<< CV_16SC2<<endl;
    cout << "CV_16SC3 "<< CV_16SC3<<endl;
    cout << "CV_16SC4 "<< CV_16SC4<<endl;

    cout << "CV_32SC1 "<< CV_32SC1<<endl;
    cout << "CV_32SC2 "<< CV_32SC2<<endl;
    cout << "CV_32SC3 "<< CV_32SC3<<endl;
    cout << "CV_32SC4 "<< CV_32SC4<<endl;

    cout << "CV_32FC1 "<< CV_32FC1<<endl;
    cout << "CV_32FC2 "<< CV_32FC2<<endl;
    cout << "CV_32FC3 "<< CV_32FC3<<endl;
    cout << "CV_32FC4 "<< CV_32FC4<<endl;

    cout << "CV_64FC1 "<< CV_64FC1<<endl;
    cout << "CV_64FC2 "<< CV_64FC2<<endl;
    cout << "CV_64FC3 "<< CV_64FC3<<endl;
    cout << "CV_64FC4 "<< CV_64FC4<<endl;
}
pos temple(Mat original, int* args)
{
    if(templ.empty())
    {
        templ = imread( "../opencv_lib/template.png",CV_LOAD_IMAGE_GRAYSCALE );
        //cout<<"Templ type: "<< templL.type()<<endl;
        //cout<<"Templ depth"<< templL.depth()<<endl;
    }

    /// Do the Matching and Normalize
    int match_method = args ? args[0] : 0;
    if (match_method <=  CV_TM_SQDIFF)
    {
        match_method = CV_TM_SQDIFF;
    }
    else if(match_method >= CV_TM_CCOEFF_NORMED)
    {
        match_method = CV_TM_CCOEFF_NORMED;
    }

    Mat channels[3];
    split(original, channels);

    Mat edgeDetect;
    if(args)
    {
        Canny(channels[1], edgeDetect, 3*args[1], 3*args[2], 3);
        //imshow("window2", edgeDetect);
    }
    else
    {
        Canny(channels[1], edgeDetect, 156, 384);
    }
    //cout<<"img type: "<< edgeDetect.type()<<endl;
    //cout<<"img depth"<< edgeDetect.depth()<<endl;

    Mat result;
    int result_cols =  edgeDetect.cols - templ.cols + 1;
    int result_rows = edgeDetect.rows - templ.rows + 1;

    result.create( result_rows, result_cols, CV_32FC1 );
    matchTemplate( edgeDetect, templ, result, match_method );
    //normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

    /// Localizing the best match with minMaxLoc
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;


    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

    /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
    {
        matchLoc = minLoc;
    }
    else
    {
        matchLoc = maxLoc;
    }

    if(args)
    {
        /*Mat resultDraw = original.clone();
        Mat imgDraw = edgeDetect.clone();

        templL.copyTo(imgDraw.rowRange(matchLocL.y, matchLocL.y + templL.rows).colRange(matchLocL.x, matchLocL.x + templL.cols));
        drawCircle(imgDraw, resultDraw, matchLocL);*/
    }

    pos temp;

    /*if (minValL > 6870000)
    {
        temp.x = -1;
        temp.y = -1;
    }
    else*/
    {
        temp.x = matchLoc.x + templ.cols /2;
        temp.y = matchLoc.y + templ.rows /2;
    }
    temp.minVal = minVal;
    temp.maxVal = maxVal;

    //cout << temp.x << "," <<temp.y << endl;
    //cout << (int)result.at<uchar>(temp.x,temp.y) << "," << (long)minVal << "," << (long)maxVal << endl;
    return temp;
}

pos process(Mat img, int* args)
{
    return temple(img, args);
}

int getms (void)
{
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds

    return s * 1000 + ms;
}
int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        return -1;
    }

    while ((dirp = readdir(dp)) != NULL) {

        stringstream ss;
        ss<<dir<<dirp->d_name;
        files.push_back(ss.str());
    }
    closedir(dp);
    return 0;
}

bool fileExists (const std::string& name)
{
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}
