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


 pos getMatch(const Mat& edgeImage, const Mat& templ, int match_method, const Mat& normalImage, const int tooBrightPixelValue ,const int tooDimPixelValue, float greenRejectMultiplyer)
 {
    Mat result;
    int result_cols =  edgeImage.cols - templ.cols + 1;
    int result_rows = edgeImage.rows - templ.rows + 1;

    result.create(result_rows, result_cols, CV_32FC1 );
    matchTemplate(edgeImage, templ, result, match_method );
    imshow("window5", edgeImage);

    //normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

    Mat pixelsWeLike;
    Mat pixelsWeLikeforResult;

    inRange(normalImage, Scalar(tooDimPixelValue,tooDimPixelValue,tooDimPixelValue), Scalar(tooBrightPixelValue,tooBrightPixelValue,tooBrightPixelValue), pixelsWeLike);


    pixelsWeLikeforResult = pixelsWeLike(Rect(templ.cols/2, templ.rows/2, result.cols, result.rows));
    //Mat channels[3];
    //split(normalImage, channels);
    //for(int rows = 0; rows < pixelsWeLikeforResult.rows; rows++)
    //{
        //for(int cols = 0; cols < pixelsWeLikeforResult.cols; cols++)
        //{
            //float& g = channels[1].at<float>(rows + templ.rows/2, cols + templ.cols/2);
            //float& r = channels[0].at<float>(rows + templ.rows/2, cols + templ.cols/2);
            //float& b = channels[2].at<float>(rows + templ.rows/2, cols + templ.cols/2);
            //if(g*greenRejectMultiplyer < b || g*greenRejectMultiplyer < r)
            //{
                //pixelsWeLikeforResult.at<uchar>(rows, cols)=0;
            //}
        //}
    //}

    //cout << "templ height : " <<templ.rows << " templ width : " << templ.cols << endl;
    imshow("window2", pixelsWeLikeforResult);
    // cout << "pixels we like height : " << pixelsWeLikeforResult.rows << " pixels we like width : " << pixelsWeLikeforResult.cols << endl;

    multiply(pixelsWeLikeforResult, result, result,1,CV_32FC1);

    //bitwise_not (pixelsWeHate, pixelsWeHate);

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

void setupTemplatePNG()
{
    templ = imread("../opencv_lib/template.png");
    flip(templ, templFlip, 1);
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

void newEdgeDetect(InputArray img, OutputArray edges)
{
    Mat edgeKernel =Mat::zeros(Size(5,5),CV_32F);
    for(int i=0; i <5; i++)
    {
        edgeKernel.at<float>(i,0)=-1;
        edgeKernel.at<float>(0,i)=-1;
        edgeKernel.at<float>(i,4)=-1;
        edgeKernel.at<float>(4,i)=-1;
    }
    edgeKernel.at<float>(2,2)=12;
    filter2D(img,edges,-1,edgeKernel);

}

pos temple(Mat original, settings& s)
{
    //setupTemplate(s.templatePixelsPerInch, s.templateLineThickness);
    setupTemplatePNG();
    /// Do the Matching and Normalize
    if (s.match_method() <=  CV_TM_SQDIFF)
    {
        s.set_match_method(CV_TM_SQDIFF);
    }
    else if(s.match_method() >= CV_TM_CCOEFF_NORMED)
    {
        s.set_match_method(CV_TM_CCOEFF_NORMED);
    }

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
                uchar& px = green.at<uchar>(x,y);
                px = saturate_cast<uchar>(128-mean[0]+px);
                px = saturate_cast<uchar>((px-128)*(s.stdDevGoal()/stddev[0])+128);


            }
        }

        //imshow("window2", green);
    }

    Mat edgeDetect;
    //Canny(channels[1], edgeDetect, 3*s.edgeDetectParam1, 3*s.edgeDetectParam2, 3);
    newEdgeDetect(original,edgeDetect);

    imshow("window3", edgeDetect);

    pos normal = getMatch(edgeDetect, templ, s.match_method(), original, s.tooBrightPixelValues(), s.tooDimPixelValue(), s.greenMultiplyer());


    return normal;
}

pos hsvFilter(Mat& rawImage, settings s)
{
    Mat hsvImage;

    inRange(rawImage, Scalar(s.hMin(),s.sMin(),s.vMin()), Scalar(s.hMax(),s.sMax(),s.vMax()), hsvImage);

}

pos process(Mat img, settings s)
{
    pos templResult = temple(img, s);
    return templResult;
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
