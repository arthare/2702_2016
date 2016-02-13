#include <iostream>
#include "2702_proc.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sys/types.h>
#include <fstream>

using namespace std;
using namespace cv;

//#define SCALE_FACTOR 2



void filterOutCrap(vector<string>& crapFiles)
{
    for(int x = 0; x < crapFiles.size(); x++ )
    {
        string file = crapFiles[x];
        if(file.find(".txt") == string::npos)
        {
            crapFiles[x] = crapFiles.back();
            crapFiles.pop_back();
            x--;
        }
    }
}

bool haveArgsChanged(const int* args, const int* lastArgs, const int cArgs)
{
    for(int x = 0;x < cArgs; x++)
    {
        if(args[x] != lastArgs[x]) return true;
    }
    return false;
}
int main()
{
    vector<string> testFiles;
    getdir("../testdata/", testFiles);


    filterOutCrap(testFiles);

    /// Create Windows
    namedWindow("window");
    namedWindow("window2");
    namedWindow("window3");
    namedWindow("window4");
    namedWindow("window5");

    const int ARGS_TO_USE = ARG_COUNT+1;
    int args[ARGS_TO_USE] = {0};
    getDefaults(args);

    for(int x = 0; x < ARGS_TO_USE; x++)
    {
      char TrackbarName[50];
      sprintf( TrackbarName, "Arg %d", x );
      createTrackbar( TrackbarName, "window", &args[x], 255 );
    }


    int ixLastImage = -1;
    Mat img;
    pos lastProcessResult;
    int lastArgs[ARGS_TO_USE];
    int boxTop;
    int boxLeft;
    int boxRight;
    int boxBottom;

    while(true)
    {
        int ixCurrentImage = args[ARGS_TO_USE-1];
        ixCurrentImage = min((int)ixCurrentImage, (int)testFiles.size());
        if(ixCurrentImage != ixLastImage)
        {
            cout<<" they want us to load "<<ixCurrentImage<<", which is "<<testFiles[ixCurrentImage]<<endl;
            string strTxt = testFiles[ixCurrentImage];
            string imgFile;
            ifstream in;
            in.open(strTxt.c_str());
            in>>imgFile;
            in>>boxLeft;
            in>>boxTop;
            in>>boxRight;
            in>>boxBottom;
            cout<<"which leads us to imgfile = "<<imgFile<<endl;
            img = imread(imgFile.c_str(), CV_LOAD_IMAGE_COLOR);
            if(img.empty())
            {
              cout<<"Image "<<imgFile<<" was empty"<<endl;
            }

            ixLastImage = ixCurrentImage;
        }


        // only reprocess if args have changed
        if(haveArgsChanged(args, lastArgs, ARGS_TO_USE))
        {
            lastProcessResult = process(img, args);

            memcpy(lastArgs, args, sizeof(args));
        }

        {
            // now that we've processed, draw a circle on it
            Mat whereAt = img.clone();
            circle(whereAt, Point(lastProcessResult.x, lastProcessResult.y), 5, Scalar(255,255,255), 3);
            rectangle(whereAt, Point(boxLeft,boxTop), Point(boxRight,boxBottom),Scalar(255,255,255), 1);
            imshow("window", whereAt);
        }

        waitKey(30);
    }
}
