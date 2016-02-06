#include <iostream>
#include "2702_proc.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>

using namespace std;
using namespace cv;



void filterOutCrap(vector<string>& crapFiles)
{
    for(unsigned int x = 0; x < crapFiles.size(); x++ )
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


int runOnce(int* args);

int randomNumber(int minNum, int maxNum)
{
    int result = (rand() % (maxNum - minNum +1)) + (minNum);

    return result;
}


int main()
{
    cout<<"Here's the test with default (on-robot) args"<<endl;
    runOnce(0);
    cout<<"^^^^^^^^^^"<<endl;
    cout<<"Above: the results for on-robot args"<<endl;

    int bestx = 0;
    int besty = 0;
    int bestz = 0;
    int store = 0;
    int searchRange = 45;

    int tries = 0;

    while(true)
    {
        cout.setstate(std::ios_base::badbit);

        int args[3] = {0};

        if(tries % 2)
        {
            args[1] = randomNumber(0,255);
            args[2] = randomNumber(0,255);
            args[3] = randomNumber(0,255);
        }
        else
        {
            args[1] = randomNumber(bestx - searchRange, bestx + searchRange);
            args[2] = randomNumber(besty - searchRange, besty + searchRange);
            args[3] = randomNumber(bestz - searchRange, bestz + searchRange);
        }

        int thisTry = runOnce(args);
        cout.clear();
        tries++;
        if(tries % 10000 == 0)
        {
            cout<<"Tries: "<<tries<<endl;
        }
        if(thisTry > store)
        {
            bestx = args[1];
            besty = args[2];
            bestz = args[3];
            store = thisTry;
            cout<<"This is the new best " << store <<" best x " << bestx <<" best y "<< besty<<" best z "<< bestz << " " << tries % 2 << endl;
        }
    }
}

void RunOneFile (string File,bool shouldFlip, int *args, int&totalTime, long int&minValNotThere, int&notTherePasses, int&notThereTotal, int&widthSum, int&heightSum, long int&minValThere, int&thereTotal, int&therePasses)
{
 string imgFile;

        ifstream in;
        in.open(File.c_str());

        in>>imgFile;

        Mat img = imread(imgFile.c_str(), CV_LOAD_IMAGE_COLOR);
        if(img.empty())
        {
          cout<<"NOT FOUND"<<endl;
        }

        else
        {

            int left;
            int right;
            int top;
            int bottom;
            in>> left;
            in>> top;
            in>> right;
            in>> bottom;

            if (shouldFlip)
            {
            Mat dst;
            flip(img ,dst ,1 );
            img=dst;
            left=160-right;
            right=160-left;
            }

            int before = getms();
            pos pt = process(img, args);
           int after = getms();
           int time = after - before;
           totalTime +=time;
            if (left < 0)
            {
                minValNotThere += pt.minVal;
                // the txt file said the target isn't there.  let's see how they guessed
                if(pt.x == -1 && pt.y == -1)
                {
                    // they correctly guessed that it isn't there
                    //cout<<"PASSED"<< " (: not there)" <<endl;
                    notTherePasses ++;

                }
                else
                {
                    // they guessed it was there, but it's not!
                    cout<<"FAILED for "<< imgFile <<" (: not there)" << "minVal " << pt.minVal<<endl;
                }
                notThereTotal++;
            }
            else
            {
                widthSum += right - left;
                heightSum += bottom - top;
                minValThere += pt.minVal;
                // left >= 0, that means the target IS present
                if (pt.x > left && pt.x < right && pt.y > top && pt.y < bottom)
                {
                    //cout<<"PASSED"<<endl;
                    therePasses ++;

                }
                else
                {
                    cout<<"FAILED for "<<imgFile<< "minVal " << pt.minVal<<endl;
                    {
                        // draw the image and where they said the target was
                       /*Mat show = img.clone();
                       circle(show,Point(pt.x, pt.y) , 20, Scalar(255, 0, 0));
                       circle(show,Point(pt.x, pt.y) , 3, Scalar(0, 0, 255));
                       cout << left << "," << top << "," << right << "," << bottom << endl;
                       cout << pt.x << "," << pt.y << endl;
                       rectangle(show, Point(left,top), Point(right,bottom), Scalar(255,255,255));
                       imshow("window", show);*/
                       //waitKey(0);
                    }
                }
                thereTotal++;
            }

        }
}
int runOnce(int* args)
{
    vector<string> testFiles;
    getdir("../testdata/", testFiles);
    filterOutCrap(testFiles);
    int totalTime = 0;

    int therePasses = 0;
    int thereTotal = 0;

    int notTherePasses = 0;
    int notThereTotal = 0;
    //int notThere = 0;

    long minValThere = 0;
    long minValNotThere = 0;

    int widthSum = 0;
    int heightSum = 0;

    namedWindow("window");

    for(unsigned int x=0; x < testFiles.size(); x++)
    {

        const string& strTxt = testFiles[x];
        RunOneFile (strTxt ,false ,args ,totalTime ,minValNotThere ,notTherePasses ,notThereTotal ,widthSum ,heightSum ,minValThere ,thereTotal ,therePasses );
        RunOneFile (strTxt ,true ,args ,totalTime ,minValNotThere ,notTherePasses ,notThereTotal ,widthSum ,heightSum ,minValThere ,thereTotal ,therePasses );

    }
    cout << therePasses << " of " << thereTotal << endl;
    cout << "Total time = "<< totalTime << endl;
    cout << "Average time = " << totalTime / testFiles.size() << endl;
    cout << "Target Not Present : " << notTherePasses <<  " of " << notThereTotal << endl;
    cout << "average minval there : " << minValThere / thereTotal << endl;
    cout << "average minval not there : " << minValNotThere / notThereTotal << endl;
    cout << "average minval : " << (minValThere / thereTotal + minValNotThere / notThereTotal) / 2 << endl;
    cout << "avg heights "<<(heightSum / thereTotal)<<endl;
    cout << "avg widths "<<(widthSum / thereTotal)<<endl;

    return therePasses;
}
