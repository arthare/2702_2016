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
Mat templFlip;
float setupTemplPixelsPerInch=-1;
int setupLineThickness = -1;

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


 pos getMatch(Mat edgeImage, Mat templ, int match_method)
 {
        Mat result;
    int result_cols =  edgeImage.cols - templ.cols + 1;
    int result_rows = edgeImage.rows - templ.rows + 1;

    result.create( result_rows, result_cols, CV_32FC1 );
    matchTemplate( edgeImage, templ, result, match_method, templ );

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

    pos temp;


    temp.x = matchLoc.x + templ.cols /2;
    temp.y = matchLoc.y + templ.rows /2;

    temp.minVal = minVal;
    temp.maxVal = maxVal;

    return temp;
 }

Mat convolveKernel;
int setupKernelSize = 0;
void setupKernel(int size)
{
    size = max(1, size);
    if(convolveKernel.empty() || setupKernelSize != size)
    {
        convolveKernel = Mat::zeros( size, size, CV_32F );

        for(int x = 0;x < size; x++)
        {
            convolveKernel.at<float>(x, size/2) = 1;
            convolveKernel.at<float>(size/2, x) = 1;
        }
        setupKernelSize = size;
    }
}
void setupTemplate(float templPixelsPerInch, const int lineThickness)
{
    templPixelsPerInch = max(templPixelsPerInch, 1.0f);
    templPixelsPerInch = min(templPixelsPerInch, 3.0f);
    if(templ.empty() || setupTemplPixelsPerInch != templPixelsPerInch || setupLineThickness != lineThickness)
    {
        setupLineThickness = lineThickness;
        setupTemplPixelsPerInch = templPixelsPerInch;

        const float flRealWidthIn = 20;
        const float flRealHeightIn = 12;
        const float flRealThicknessIn = 2.0;

        templ = Mat::zeros(flRealHeightIn * templPixelsPerInch, flRealWidthIn * templPixelsPerInch,  CV_8UC1);

        Point ptTopRight(flRealWidthIn - flRealThicknessIn/2, 0);
        Point ptBotRight(flRealWidthIn - flRealThicknessIn/2, flRealHeightIn - flRealThicknessIn/2);
        Point ptBotLeft(0, flRealHeightIn - flRealThicknessIn/2);
        Point ptTopLeft(0,0);
        Point ptInnerTopLeft(flRealThicknessIn, 0);
        Point ptLeftArmpit(flRealThicknessIn, flRealHeightIn - flRealThicknessIn*1.5);
        Point ptRightArmpit(flRealWidthIn - flRealThicknessIn*1.5f, flRealHeightIn - flRealThicknessIn*1.5);
        Point ptInnerTopRight(flRealWidthIn - flRealThicknessIn*1.5f, 0);

        Point* rgPointOrder[] = {
            &ptTopRight,
            &ptBotRight,
            &ptBotLeft,
            &ptTopLeft,
            &ptInnerTopLeft,
            &ptLeftArmpit,
            &ptRightArmpit,
            &ptInnerTopRight,
        };
        const int NUM_POINTS = sizeof(rgPointOrder) / sizeof(rgPointOrder[0]);
        for(int x = 0;x < NUM_POINTS; x++)
        {
            // gotta scale the points
            Point pt = *rgPointOrder[x];
            Point pt2 = *rgPointOrder[(x+1) % NUM_POINTS];
            pt.x *= templPixelsPerInch;
            pt.y *= templPixelsPerInch;
            pt2.x *= templPixelsPerInch;
            pt2.y *= templPixelsPerInch;

            line(templ, pt, pt2, Scalar(255,255,255), lineThickness);
        }

        flip(templ, templFlip, 1);
    }
}
pos temple(Mat original, int* args)
{
    // args[0]: size of kernel
    // args[1]: edgedetect param 1
    // args[2]: edgedetect param 2
    // args[3]: stddev goal
    // args[4]: template scale
    // args[5]: template thickness
    const int kernelSize = args ? args[0] : 1;
    const int edgeDetect1 = args ? args[1] : 64;
    const int edgeDetect2 = args ? args[2] : 99;
    const float stdDevGoal = args ? args[3] : 37;
    const int templPixelsPerInch = (float)(args ? args[4] : 20)/10.0f;
    const int templLineThickness = args ? args[5] : 2;
    int edgeDetect3 = ((args ? args[6] : 1) * 2) + 1;
    edgeDetect3 = max(3, edgeDetect3);
    edgeDetect3 = min(7, edgeDetect3);

    setupTemplate(templPixelsPerInch, templLineThickness);
    setupKernel(kernelSize);

    /// Do the Matching and Normalize
    const int match_method = CV_TM_SQDIFF;

    Mat channels[3];
    split(original, channels);

    {
        Mat& green = channels[1];
        Scalar mean;
        Scalar stddev;
        meanStdDev(green, mean, stddev);
        for(int y=0; y < green.cols; y++)
        {
            for(int x=0; x < green.rows; x++)
            {
                uchar& g = green.at<uchar>(x,y);

                g = saturate_cast<uchar>(128-mean[0]+g);
                g = saturate_cast<uchar>((g-128)*(stdDevGoal/stddev[0])+128);

            }
        }

        if(args)
        {
            imshow("window2", green);
        }
    }

    Mat edgeDetect;
    Canny(channels[1], edgeDetect, 3*edgeDetect1, 3*edgeDetect2, edgeDetect3);

    Mat thickEdges;
    if(kernelSize > 1)
    {
        filter2D(edgeDetect, thickEdges, -1, convolveKernel, Point(-1,-1), 0, BORDER_DEFAULT);
    }
    else
    {
        thickEdges = edgeDetect;
    }

    if(args)
    {
        imshow("window3", edgeDetect);
        imshow("window4", templ);
    }


    pos normal = getMatch(thickEdges, templ, match_method);
    pos flipped = getMatch(thickEdges, templFlip, match_method);
    pos ret;
    ret.x = (normal.x + flipped.x) / 2;
    ret.y = (normal.y + flipped.y) / 2;
    return ret;
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
        cout << ss.str()<<endl;
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
