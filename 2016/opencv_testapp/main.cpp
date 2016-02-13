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

runOnceResult runOnce(settings& s);

int randomNumber(int minNum, int maxNum)
{
    int result = (rand() % (maxNum - minNum +1)) + (minNum);

    return result;
}

void reportABestTry(const runOnceResult& result, settings& best, const char* who)
{
    cout<<who<<": Best error:" << result.totalError << " images:" << result.passCount;
    for(int a=0; a < settings::ARG_COUNT; a=a+1)
    {
        // spit out current bests
        cout <<" best[" << a << "]: " << best.args[a] << " ";
    }
    cout<<endl;
}
void tuneArgs(settings& s, const bool* DIFFERENTIABLE, const int* LOWER_BOUNDS, const int* UPPER_BOUNDS, runOnceResult& thisTry)
{
    const int veryFirstError = thisTry.totalError;

    bool stillFindingImprovements = true;
    while(stillFindingImprovements)
    {
        cout<<"Starting hillclimb cycle, looking to improve on "<<veryFirstError<<endl;

        stillFindingImprovements = false; // assume we won't find any improvements on this cycle
        for(int ixArg = 0; ixArg < settings::ARG_COUNT; ixArg++)
        {
            if(!DIFFERENTIABLE[ixArg]) continue;

            const int modSpan = 5;
            const int modStep = 2;
            const int initialValue = s.args[ixArg];
            int bestValueForThisArg = initialValue;
            int bestTotalError = thisTry.totalError;
            for(int mod = max(LOWER_BOUNDS[ixArg], initialValue-modSpan); mod <= min(UPPER_BOUNDS[ixArg]-1, initialValue+modSpan); mod+=modStep)
            {
                s.args[ixArg] = mod;
                s.args[ixArg] = min(s.args[ixArg], UPPER_BOUNDS[ixArg] - 1);
                s.args[ixArg] = max(s.args[ixArg], LOWER_BOUNDS[ixArg]);
                cout.setstate(std::ios_base::badbit);
                runOnceResult modTry = runOnce(s);
                cout.clear();

                if(modTry.totalError < bestTotalError)
                {
                    cout<<"Hillclimber improved from "<<veryFirstError<<" to "<<modTry.totalError<<endl;
                    bestValueForThisArg = mod; // just use this one now
                    bestTotalError = modTry.totalError;
                    thisTry = modTry;
                    stillFindingImprovements = true; // we found improvements!


                    reportABestTry(thisTry, s, "HillClimb");
                }

            }
            s.args[ixArg] = bestValueForThisArg;
        }
    }


}

int main()
{
    cout<<"Here's the test with default (on-robot) args"<<endl;
    settings default_settings(NO_UI);
    runOnceResult veryFirstTry = runOnce(default_settings);

    cout<<"^^^^^^^^^^"<<endl;
    cout<<"Above: the results for on-robot args"<<endl;

    int bestScore = veryFirstTry.totalError;
    //int searchRange = 35;

    const int LOWER_BOUNDS[] = {
        0,
        20, // stddev stretch
        150, //brightest pixel we will keep
        0, // Dimest pixel we will keep
    };
    const int UPPER_BOUNDS[] = {
        6,
        45, // stddev stretch
        255, //brightest pixel we will keep
        50, // dimest pixel we will keep
    };
    const bool DIFFERENTIABLE[] = {
        true,
        true,
        true,
        true,
    };

    int tries = 0;

    for (;;)
    {
        settings s(NO_UI);
        settings best(NO_UI);

        for(int a=0; a < settings::ARG_COUNT; a=a+1)
        {
            s.args[a] = randomNumber(LOWER_BOUNDS[a],UPPER_BOUNDS[a]);
            s.args[a] = min(s.args[a], UPPER_BOUNDS[a] - 1);
            s.args[a] = max(s.args[a], LOWER_BOUNDS[a]);
        }

        cout.setstate(std::ios_base::badbit);
        runOnceResult thisTry = runOnce(s);
        cout.clear();

        tries++;
        if (tries % 10000 == 0)
        {
            cout<<"Tries: "<<tries<<endl;
        }

        if (thisTry.totalError < bestScore)
        {
            tuneArgs(s, DIFFERENTIABLE, LOWER_BOUNDS, UPPER_BOUNDS, thisTry);
            // we got a personal best!
            best.copyArgs(s.args);
            bestScore = thisTry.totalError;
            reportABestTry(thisTry, best, "Random");
        }
    }
}

void RunOneFile (string File, bool shouldFlip, settings &s, int &totalTime, int &widthSum, int &heightSum, int &thereTotal, int &therePasses, int &sumError, ostream& os)
{
    string imgFile;
    ifstream in;
    in.open(File.c_str());

    in>>imgFile;

    Mat img = imread(imgFile.c_str(), CV_LOAD_IMAGE_COLOR);
    if (img.empty())
    {
      cout<<"NOT FOUND"<<endl;
      return;
    }

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
        int leftTemp = left;
        Mat dst;
        flip(img ,dst ,1 );
        img=dst;
        left=160-right;
        right=160-leftTemp;
    }

    int before = getms();

    pos pt = process(img, s);
    int after = getms();
    int time = after - before;
    totalTime +=time;

    int centerX=(left+right)/2;
    int centerY=(top+bottom)/2;
    int error=pow(centerX-pt.x,2) +pow(centerY-pt.y,2);

    sumError=sumError+error;
    widthSum += right - left;
    heightSum += bottom - top;
    os << "Error: " << error << " With File : " << imgFile << endl;
    // left >= 0, that means the target IS present
    if (pt.x > left && pt.x < right && pt.y > top && pt.y < bottom)
    {
        //cout<<"PASSED"<<endl;
        therePasses ++;

    }

    thereTotal++;
}

runOnceResult runOnce(settings &s)
{
    vector<string> testFiles = getListOfTestFiles();

    int totalTime = 0;
    int therePasses = 0;
    int thereTotal = 0;
    int widthSum = 0;
    int heightSum = 0;
    int sumError = 0;

    if (s.showUI) namedWindow("window");

    ofstream os;
    os.open("/dev/null");

    for(unsigned int x=0; x < testFiles.size(); x++)
    {
        const string& strTxt = testFiles[x];

        RunOneFile (strTxt, false, s, totalTime, widthSum, heightSum, thereTotal, therePasses, sumError, os );
        RunOneFile (strTxt, true, s, totalTime, widthSum, heightSum, thereTotal, therePasses, sumError, os );
        os.flush();

    }
    cout << therePasses << " of " << thereTotal << endl;
    cout << "Total time = "<< totalTime << endl;
    cout << "Average time = " << totalTime / testFiles.size() << endl;
    cout << "avg heights "<<(heightSum / thereTotal)<<endl;
    cout << "avg widths "<<(widthSum / thereTotal)<<endl;
    cout << "total errors "<< sumError <<endl;
    runOnceResult ret;
    ret.passCount=therePasses;
    ret.totalError=sumError;
    return ret;
}

