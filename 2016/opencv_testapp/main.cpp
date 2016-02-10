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

struct runOnceResult
{
    int passCount;
    int totalError;
};

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


runOnceResult runOnce(int* args);

int randomNumber(int minNum, int maxNum)
{
    int result = (rand() % (maxNum - minNum +1)) + (minNum);

    return result;
}

void reportABestTry(const runOnceResult& result, int* args, int cArgs, const char* who)
{
    cout<<who<<": Best error:" << result.totalError << " images:" << result.passCount;
    for(int a=0; a < cArgs; a=a+1)
    {
        // spit out current bests
        cout <<" best[" << a << "]: " << args[a] << " ";
    }
    cout<<endl;
}
void tuneArgs(int* args, const int cArgs, const bool* DIFFERENTIABLE, const int* LOWER_BOUNDS, const int* UPPER_BOUNDS, runOnceResult& thisTry)
{
    const int veryFirstError = thisTry.totalError;

    bool stillFindingImprovements = true;
    while(stillFindingImprovements)
    {
        cout<<"Starting hillclimb cycle, looking to improve on "<<veryFirstError<<endl;

        stillFindingImprovements = false; // assume we won't find any improvements on this cycle
        for(int ixArg = 0; ixArg < cArgs; ixArg++)
        {
            if(!DIFFERENTIABLE[ixArg]) continue;

            const int modSpan = 5;
            const int modStep = 2;
            const int initialValue = args[ixArg];
            int bestValueForThisArg = initialValue;
            int bestTotalError = thisTry.totalError;
            for(int mod = max(LOWER_BOUNDS[ixArg], initialValue-modSpan); mod <= min(UPPER_BOUNDS[ixArg]-1, initialValue+modSpan); mod+=modStep)
            {
                args[ixArg] = mod;
                args[ixArg] = min(args[ixArg], UPPER_BOUNDS[ixArg] - 1);
                args[ixArg] = max(args[ixArg], LOWER_BOUNDS[ixArg]);
                cout.setstate(std::ios_base::badbit);
                runOnceResult modTry = runOnce(args);
                cout.clear();

                if(modTry.totalError < bestTotalError)
                {
                    cout<<"Hillclimber improved from "<<veryFirstError<<" to "<<modTry.totalError<<endl;
                    bestValueForThisArg = mod; // just use this one now
                    bestTotalError = modTry.totalError;
                    thisTry = modTry;
                    stillFindingImprovements = true; // we found improvements!


                    reportABestTry(thisTry, args, cArgs, "HillClimb");
                }

            }
            args[ixArg] = bestValueForThisArg;
        }
    }


}


int main()
{
    cout<<"Here's the test with default (on-robot) args"<<endl;
    runOnceResult veryFirstTry = runOnce(0);
    cout<<"^^^^^^^^^^"<<endl;
    cout<<"Above: the results for on-robot args"<<endl;

    const int ARGS_TO_OPTIMIZE = 7;
    int best[ARGS_TO_OPTIMIZE] = {2, 125, 121, 37, 2.5, 1, 175};

    int bestScore = veryFirstTry.totalError;
    int searchRange = 35;

    const int LOWER_BOUNDS[] = {
        0,
        0, // edge1.1
        0, // edge1.2
        35, // stddev stretch
        15, // template pixels per inch
        1, // template line thickness
        175 //brightest pixel we will keep
    };
    const int UPPER_BOUNDS[] = {
        6,
        255, // edge1.1
        255, // edge1.2
        36, // stddev stretch
        40, //template pixels per inch
        5, //template line thickness
        255 //brightest pixel we will keep
    };
    const bool DIFFERENTIABLE[] = {
        false,
        true,
        true,
        false,
        true,
        true,
        true,
    };

    int tries = 0;

    while(true)
    {

        int args[ARGS_TO_OPTIMIZE] = {0};

        for(int a=0; a < ARGS_TO_OPTIMIZE; a=a+1)
        {
            args[a] = randomNumber(LOWER_BOUNDS[a],UPPER_BOUNDS[a]);
            args[a] = min(args[a], UPPER_BOUNDS[a] - 1);
            args[a] = max(args[a], LOWER_BOUNDS[a]);
        }

        cout.setstate(std::ios_base::badbit);
        runOnceResult thisTry = runOnce(args);
        cout.clear();

        tries++;
        if(tries % 10000 == 0)
        {
            cout<<"Tries: "<<tries<<endl;
        }
        if(thisTry.totalError < bestScore)
        {
            tuneArgs(args, ARGS_TO_OPTIMIZE, DIFFERENTIABLE, LOWER_BOUNDS, UPPER_BOUNDS, thisTry);
            // we got a personal best!
            for(int a=0; a < ARGS_TO_OPTIMIZE; a=a+1)
            {
                // remember the arguments we used to achieve this
                best[a]=args[a];
            }
            bestScore = thisTry.totalError;
            reportABestTry(thisTry, best, ARGS_TO_OPTIMIZE, "Random");
        }
    }
}

void RunOneFile (string File,bool shouldFlip, int *args, int&totalTime, long int&minValNotThere, int&notTherePasses, int&notThereTotal, int&widthSum, int&heightSum, long int&minValThere, int&thereTotal, int&therePasses, int&sumError)
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
                    //cout<<"FAILED for "<< imgFile <<" (: not there)" << "minVal " << pt.minVal<<endl;
                }
                notThereTotal++;
            }
            else
            {
                int centerX=(left+right)/2;
                int centerY=(top+bottom)/2;
                int error=pow(centerX-pt.x,2) +pow(centerY-pt.y,2);

                sumError=sumError+error;
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
runOnceResult runOnce(int* args)
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

    int sumError = 0;

    namedWindow("window");

    for(unsigned int x=0; x < testFiles.size(); x++)
    {

        const string& strTxt = testFiles[x];
        RunOneFile (strTxt ,false ,args ,totalTime ,minValNotThere ,notTherePasses ,notThereTotal ,widthSum ,heightSum ,minValThere ,thereTotal ,therePasses, sumError );
        RunOneFile (strTxt ,true ,args ,totalTime ,minValNotThere ,notTherePasses ,notThereTotal ,widthSum ,heightSum ,minValThere ,thereTotal ,therePasses, sumError );

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
    cout << "total errors "<< sumError <<endl;
    runOnceResult ret;
    ret.passCount=therePasses;
    ret.totalError=sumError;
    return ret;
}

