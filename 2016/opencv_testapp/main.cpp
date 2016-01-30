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
int main()
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

    for(unsigned int x=0; x < testFiles.size(); x++)
    {
        const string& strTxt = testFiles[x];
        string imgFile;

        ifstream in;
        in.open(strTxt.c_str());

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

            int before = getms();

            pos pt = process(img, 0);
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
                }
                thereTotal++;
            }

        }

    }
    cout << therePasses << " of " << thereTotal << endl;
    cout << "Total time = "<< totalTime << endl;
    cout << "Average time = " << totalTime / testFiles.size() << endl;
    cout << "Target Not Present : " << notTherePasses <<  " of " << notThereTotal << endl;
    cout << "average minval there : " << minValThere / thereTotal << endl;
    cout << "average minval not there : " << minValNotThere / notThereTotal << endl;
    cout << "avg heights "<<(heightSum / thereTotal)<<endl;
    cout << "avg widths "<<(widthSum / thereTotal)<<endl;
}
