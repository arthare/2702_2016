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


float runOnce(int* args);

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

    const int WILD_GUESS_FREQUENCY = 2;
    const int ARGS_TO_OPTIMIZE = 7;
    const int searchRange = 0.35; // percentage of the entire bounds of a given thingy
    int best[ARGS_TO_OPTIMIZE] = {3,106, 94, 35, 24, 2, 1};
    float bestScoreYet = 1e30f;


    const int LOWER_BOUNDS[] = {
        0,
        0, // edge1.1
        0, // edge1.2
        30, // stddev stretch
        10, // template scale
        1, // template thickness
        0, // edge 1.3 (aperture)
    };
    const int UPPER_BOUNDS[] = {
        7,
        255, // edge1.1
        255, // edge1.2
        45, // stddev stretch
        40, // edge2.1
        3, // edge2.2
        3, // edge 1.3
    };

    int tries = 0;

    while(true)
    {

        int args[ARGS_TO_OPTIMIZE] = {0};

        const bool isWildGuess = tries % WILD_GUESS_FREQUENCY == 0;
        if(isWildGuess)
        {
            // if odd, make random guess
            for(int a=0; a < ARGS_TO_OPTIMIZE; a=a+1)
            {
                args[a] = randomNumber(LOWER_BOUNDS[a],UPPER_BOUNDS[a]);
                args[a] = min(args[a], UPPER_BOUNDS[a] - 1);
                args[a] = max(args[a], LOWER_BOUNDS[a]);
            }
        }
        else
        {
            // if even, make guess close to our best so far
            for(int a=0; a < ARGS_TO_OPTIMIZE; a=a+1)
            {
                const int bounceSize = (UPPER_BOUNDS[a] - LOWER_BOUNDS[a]) * searchRange;
                args[a] = randomNumber(best[a] - bounceSize, best[a] + bounceSize);
                args[a] = min(args[a], UPPER_BOUNDS[a] - 1);
                args[a] = max(args[a], LOWER_BOUNDS[a]);
            }
        }

        cout.setstate(std::ios_base::badbit);
        int thisTry = runOnce(args);
        cout.clear();
        tries++;
        if(tries % 10000 == 0)
        {
            cout<<"Tries: "<<tries<<endl;
        }
        if(thisTry < bestScoreYet)
        {
            // we got a personal best!
            for(int a=0; a < ARGS_TO_OPTIMIZE; a=a+1)
            {
                // remember the arguments we used to achieve this
                best[a]=args[a];
            }
            bestScoreYet = thisTry;
            cout<<"This is the new best " << bestScoreYet;
            for(int a=0; a < ARGS_TO_OPTIMIZE; a=a+1)
            {
                // spit out current bests
                cout <<" best[" << a << "]: " << best[a] << " ";
            }
            cout << " Wild? "<< isWildGuess<<endl;
        }
    }
}

void RunOneFile (string File,bool shouldFlip, int *args, int&totalTime, long int&minValNotThere, int&notTherePasses, int&notThereTotal, int&widthSum, int&heightSum, long int&minValThere, int&thereTotal, float&scoreTotal, int& therePassCount)
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

        const Point ptCenter((left+right)/2, (top+bottom)/2);


        const int beforeProcess = getms();
        pos pt = process(img, args);
        const int afterProcess = getms();
        const int timeForProcess = afterProcess - beforeProcess;
        totalTime += timeForProcess;
        if (left < 0)
        {
            // don't even care...
        }
        else
        {
            widthSum += right - left;
            heightSum += bottom - top;
            minValThere += pt.minVal;

            if(pt.x >= left && pt.x <= right && pt.y >= top && pt.y <= bottom)
            {
                therePassCount++;
            }
            Point ptDistance(pt.x - ptCenter.x, pt.y - ptCenter.y);

            // note that we leave the dist squared so that our score is affect more by bad misses
            const double dist = ptDistance.x*ptDistance.x + ptDistance.y*ptDistance.y;
            scoreTotal += dist;

            thereTotal++;
        }

    }
}
float runOnce(int* args)
{
    vector<string> testFiles;
    getdir("../testdata/", testFiles);
    filterOutCrap(testFiles);
    int totalTime = 0;

    float scoreTotal = 0;
    int thereTotal = 0;

    int notTherePasses = 0;
    int notThereTotal = 0;
    //int notThere = 0;

    long minValThere = 0;
    long minValNotThere = 0;

    int widthSum = 0;
    int heightSum = 0;

    int therePassCount = 0;

    namedWindow("window");

    for(unsigned int x=0; x < testFiles.size(); x++)
    {

        const string& strTxt = testFiles[x];
        RunOneFile (strTxt ,false ,args ,totalTime ,minValNotThere ,notTherePasses ,notThereTotal ,widthSum ,heightSum ,minValThere ,thereTotal ,scoreTotal, therePassCount );
        RunOneFile (strTxt ,true ,args ,totalTime ,minValNotThere ,notTherePasses ,notThereTotal ,widthSum ,heightSum ,minValThere ,thereTotal ,scoreTotal, therePassCount );

    }
    cout << scoreTotal << " of " << thereTotal << endl;
    cout << therePassCount <<" passes for 'there' images"<<endl;
    cout << "Total time = "<< totalTime << endl;
    cout << "Average time = " << totalTime / testFiles.size() << endl;
    cout << "avg heights "<<(heightSum / thereTotal)<<endl;
    cout << "avg widths "<<(widthSum / thereTotal)<<endl;

    return scoreTotal;
}
