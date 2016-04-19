#include <iostream>
#include <math.h>

const double globalSetPoint = 2;

double abs(double i)
{
    return i > 0 ? i : -i;
}

using namespace std;

class Vector3
{
public:
    Vector3() : x(0),y(0),z(0) {};
    Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
    {
    }

    const Vector3& operator += (const Vector3& o)
    {
        x += o.x;
        y += o.y;
        z += o.z;
        return *this;
    }
    const Vector3 operator * (const double& o) const
    {
        Vector3 ret;
        ret.x = x*o;
        ret.y = y*o;
        ret.z = z*o;
        return ret;
    }
    const Vector3 operator / (const double& o) const
    {
        Vector3 ret;
        ret.x = x/o;
        ret.y = y/o;
        ret.z = z/o;
        return ret;
    }

    double Length() const
    {
        return sqrt(x*x+y*y+z*z);
    }
    Vector3 Unit() const
    {
        const double len = Length();
        Vector3 ret;
        ret.x = x / len;
        ret.y = y / len;
        ret.z = z / len;
        return ret;
    }
    Vector3 operator -() const
    {
        return Vector3(-x, -y, -z);
    }
private:
    friend class PhysicsEntity;

    double x,y,z;
};

class PhysicsEntity
{
public:
    PhysicsEntity(double massKg) : massKg(massKg) {}

    double GetX() const {return position.x;}
    double GetY() const {return position.y;}
    double GetZ() const {return position.z;}
    double GetSpeedX() const {return speed.x;}
    double GetSpeedY() const {return speed.y;}
    double GetSpeedZ() const {return speed.z;}

protected:
    void AddForce(Vector3 force)
    {
        forceSum += force;
    }
    void Entity_Tick(double dt)
    {
        // friction calcs
        // f = normal force * uf
        const double frictionConstant = 0.1;
        const double normalForce = massKg * 9.81;
        const double currentSpeed = speed.Length();
        if(currentSpeed > 0)
        {
            const double frictionForceNewtons = normalForce * frictionConstant;
            const Vector3 unitSpeed = speed.Unit();
            const Vector3 negSpeedUnit = -unitSpeed;

            const Vector3 frictionForce = negSpeedUnit*frictionForceNewtons;
            AddForce(frictionForce);
        }

        Vector3 accel = forceSum / massKg;
        speed += accel * dt;


        position += speed * dt;

        forceSum = Vector3(0,0,0);


    }

private:
    Vector3 position;
    Vector3 speed;

    Vector3 forceSum;
    float massKg;
};

class PIDLoop
{
public:
    PIDLoop(double p, double i, double d) : sumI(0), lastError(0), kp(p), ki(i), kd(d), isDone(false)
    {
    }

    bool IsDone() const
    {
        return isDone;
    }

    void Tick(double inputSensor, double* pdOutput, double setPoint, double dt)
    {
        const double error = setPoint - inputSensor;
        const double p = error*kp;
        sumI += error*dt;
        const double i = sumI*ki;
        const double errorChange = (error - lastError)/dt;
        const double d = errorChange*kd;
        *pdOutput = p+i+d;
        if (abs(error) < 0.01 && abs(errorChange) < 0.01)
        {
            isDone = true;
        } else
        {
            isDone = false;
        }
        lastError = error;
    }
private:
    double sumI;
    double lastError;

    const double kp;
    const double ki;
    const double kd;
    bool isDone;
};

class Robot : public PhysicsEntity
{
public:
    Robot() : PhysicsEntity(54), pid(20,0,1.25) {}
    void Tick(double dt)
    {

        double motorPower = 0;
        const double inputSensor = GetX();
        pid.Tick(inputSensor, &motorPower, globalSetPoint, dt);

        SetMotorPower(motorPower);


        PhysicsEntity::Entity_Tick(dt);
    }
    bool IsDone() const
    {
        return pid.IsDone();
    }

private:
    void SetMotorPower(double power)
    {
        if (power >= 1)
        {
            power = 1;
        }
        else if (power <= -1)
        {
            power = -1;
        }

        const double motorForce = power * 100;
        AddForce(Vector3(motorForce,0,0));
    }

private:
    PIDLoop pid;
};

int main()
{
    Robot r;
    double totalTime = 0;
    int lastReportedSecond = 0;
    while(true)
    {
        const double dt = 0.001;
        totalTime += dt;
        r.Tick(dt);
        if (r.IsDone())
        {
            cout<<totalTime<<" Robot position: "<<r.GetX()<<endl;
            return 0;
        }

        const int thisSecond = (totalTime * 10);
        if(thisSecond != lastReportedSecond)
        {
            cout<<totalTime<<" Robot position: "<<r.GetX()<<" speed: "<<r.GetSpeedX()<<endl;
            lastReportedSecond = thisSecond;
        }
    }
    return 0;
}
