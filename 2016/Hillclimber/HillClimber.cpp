#include "HillClimber.h"
#include <iostream>
#include <stdint.h> // FLOAT_MAX
#include <random>

using namespace std;

// how close does a random guess have to be to our current best to be considered for further tuning?
// ex: if our current best is 50, and a guess hits 54 with a CANDIDATE_THRESHOLD of 1.1, it will be considered since 54 < (50*1.1)
const float CANDIDATE_THRESHOLD = 1.1;

// when doing hillclimb steps, how big of a step should we take?
// stepSize = (GetVarMax - GetVarMin) / PARAM_SPAN_STEPS
// make bigger to use smaller steps
const float PARAM_SPAN_STEPS = 250.0f;

random_device randomNumberGenerator;
float randomZeroOne()
{
    unsigned int rnd = randomNumberGenerator();
    double max = randomNumberGenerator.max();
    double min = randomNumberGenerator.min();
    double offset = rnd - min;
    return (offset / (max - min));
}
float randomNumber(float minNum, float maxNum)
{
    const float span = maxNum - minNum;

    return randomZeroOne() * span + minNum;
}

// takes a set of parameters, and tweaks them looking for a local error-minimum.  Returns the tweaked parameters
vector<float> tuneArgs(const std::vector<float>& startParams, const float startError, Climbable* pTarget, float* pTunedError)
{
    float bestErrorSoFar = startError;
    vector<float> paramsLocal = startParams;
    vector<float> bestParams = startParams;
    bool stillFindingImprovements = true;
    while(stillFindingImprovements)
    {
        stillFindingImprovements = false; // assume we won't find any improvements on this cycle
        for(unsigned int ixArg = 0; ixArg < startParams.size(); ixArg++)
        {
            if(!pTarget->ShouldClimbOnParameter(ixArg)) continue;

            const float maxParamValue = pTarget->GetVarMax(ixArg);
            const float minParamValue = pTarget->GetVarMin(ixArg);
            const float paramSpan = maxParamValue - minParamValue;

            const float stepsToMake = 5;
            const float stepSize = paramSpan / PARAM_SPAN_STEPS;
            const float modSpan = stepSize * stepsToMake;
            const float initialValue = paramsLocal[ixArg];
            float bestValueForThisArg = initialValue;

            for(float mod = max(minParamValue, initialValue-modSpan); mod <= min(maxParamValue, initialValue+modSpan); mod+=stepSize)
            {
                paramsLocal[ixArg] = mod;
                paramsLocal[ixArg] = min(paramsLocal[ixArg], maxParamValue);
                paramsLocal[ixArg] = max(paramsLocal[ixArg], minParamValue);

                // setstate/clear makes sure that any couts in the evaluate code don't make it to us
                cout.setstate(std::ios_base::badbit);
                float evalResult = pTarget->Evaluate(paramsLocal);
                cout.clear();

                if(evalResult < bestErrorSoFar)
                {
                    bestValueForThisArg = mod; // just use this one now
                    bestErrorSoFar = evalResult;
                    bestParams = paramsLocal;
                    stillFindingImprovements = true; // we found improvements!


                    pTarget->ReportProgress(paramsLocal, evalResult, "tuned to");
                }

            }
            paramsLocal[ixArg] = bestValueForThisArg;
        }
    }
    *pTunedError = bestErrorSoFar;
    return bestParams;
}

// given a target, performs a hill climb on it
void DoHillClimb(Climbable* pTarget)
{

    float bestError = 1e30f;
    vector<float> bestParams;

    { // measuring initial goodness
        for(int a=0; a < pTarget->GetParamCount(); a=a+1)
        {
            bestParams.push_back(pTarget->GetVarInitial(a));
        }

        cout.setstate(std::ios_base::badbit);
        bestError = pTarget->Evaluate(bestParams);
        cout.clear();

        pTarget->ReportProgress(bestParams, bestError, "Started At");
    }
    for (;;)
    {
        vector<float> params;

        // how tightly around the current best will we center our random guesses?
        // 0: exactly the same as the current best
        // 1: nothing like the current best
        const float tightness = randomZeroOne();
        for(int a=0; a < pTarget->GetParamCount(); a=a+1)
        {
            const float paramMax = pTarget->GetVarMax(a);
            const float paramMin = pTarget->GetVarMin(a);
            const float paramSpan = paramMax - paramMin;
            // pick a random value centered around
            const float randomSpan = paramSpan * tightness;

            params.push_back(randomNumber(bestParams[a] - randomSpan/2,bestParams[a] + randomSpan/2));
        }

        cout.setstate(std::ios_base::badbit);
        float thisError = pTarget->Evaluate(params);
        cout.clear();

        if (thisError < bestError * CANDIDATE_THRESHOLD)
        {
            pTarget->ReportProgress(params, thisError, "About to tune candidate");

            float tunedError = 0;
            vector<float> tunedParams = tuneArgs(params, thisError, pTarget, &tunedError);

            if(tunedError < bestError)
            {
                // we got a personal best!
                bestParams = tunedParams;
                bestError = tunedError;
                pTarget->ReportProgress(bestParams, bestError, "Tuned and improved to");
            }
            else
            {
                // we didn't tune to anything better
                pTarget->ReportProgress(tunedParams, tunedError, "Tuned to");
                pTarget->ReportProgress(bestParams, bestError, "Best is still");
            }
        }
    }
}
