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

vector<string> getListOfTestFiles()
{
    vector<string> allFiles;
    getdir("../testdata/", allFiles);

    vector<string> testFiles;
    for(size_t i = 0; i < allFiles.size(); ++i )
    {
        const string& file = allFiles[i];
        if(file.find(".txt") != string::npos)
        {
            testFiles.push_back(file);
        }
    }
    return testFiles;
}

bool haveSettingsChanged(const settings& orig_settings, const settings& last_settings)
{
    for (int x = 0; x < settings::ARG_COUNT; ++x)
    {
        if (orig_settings.args[x] != last_settings.args[x]) return true;
    }
    return false;
}

int main()
{
    vector<string> testFiles = getListOfTestFiles();

    /// Create Windows
    namedWindow("window");
    namedWindow("window2");
    namedWindow("window3");
    namedWindow("window4");
    namedWindow("window5");

    settings s(WITH_UI);

    for(int x = 0; x < settings::ARG_COUNT; x++)
    {
      char TrackbarName[50];
      sprintf( TrackbarName, "Arg %d", x );
      createTrackbar( TrackbarName, "window", &s.args[x], 255 );
    }

    int ixCurrentImage = 0;
    createTrackbar( "Image Selection", "window", &ixCurrentImage, 255 );

    int ixLastImage = -1;
    Mat img;
    pos lastProcessResult;
    settings lastArgs(WITH_UI);
    int boxTop;
    int boxLeft;
    int boxRight;
    int boxBottom;

    while(true)
    {

        ixCurrentImage = min((int)ixCurrentImage, (int)testFiles.size());
        bool imageHasChanged = (ixCurrentImage != ixLastImage);
        if(imageHasChanged)
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

            /*Mat flipOutput;
            flip(img,flipOutput,1);
            img=flipOutput;
            int boxRightOld=boxRight;
            boxRight=160-boxLeft;
            boxLeft=160-boxRightOld;*/
        }

        // only reprocess if args have changed
        if (imageHasChanged || haveSettingsChanged(s, lastArgs))
        {
            const pos pt = lastProcessResult = process(img, s);
            lastArgs.copyArgs(s.args);
            const int centerX = (boxLeft+boxRight) / 2;
            const int centerY = (boxTop+boxBottom) / 2;
            const int error = pow(centerX-pt.x, 2) + pow(centerY-pt.y, 2);
            cout << "Error : "<< error << endl;

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
