#pragma once

#include <vector>

// implement this interface to create something that the hillclimber can climb
class Climbable
{
public:
    virtual ~Climbable() {};
    // evaluate the given set of parameters.
    // examples:
    //   if your task was "find the best set of parameters to make a PID loop faster", then your Evaluate()
    //   would run a simulation with the given set of parameters, and return how long it took.
    //
    //   if your task was "find the best parameters for a vision system", you might apply your vision system
    //   to a wide variety of sample images, and return how far off you were from the centerpoint in each one
    virtual float Evaluate(std::vector<float> params) = 0;

    // for parameter "index", how high can it be?
    virtual float GetVarMax(int index) const = 0;

    // for parameter "index", how low can it be?
    virtual float GetVarMin(int index) const = 0;

    // for parameter "index", what value should it start at?
    virtual float GetVarInitial(int index) const = 0;

    // whether we should hillclimb on a given parameter.
    virtual bool ShouldClimbOnParameter(int index) const {return true;}

    // how many parameters are there?
    virtual int GetParamCount() const = 0;

    virtual void ReportProgress(const std::vector<float>& params, float errorAchieved, const char* preamble) const {};
};

void DoHillClimb(Climbable* pTarget);
