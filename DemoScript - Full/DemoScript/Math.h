#pragma once
namespace Hx
{
// aux. utility functions
inline float Radian(int x)  {return float(x)*0.017453292519943295769236907684885f;}
inline float Radian(float x)  {return (x)*0.017453292519943295769236907684885f;}
inline double Radian(double x)  {return (x)*0.017453292519943295769236907684885;}
inline float Degree(float x)  {return (x)*57.295779513082320876798154814114f;}
inline double Degree(double x)  {return (x)*57.295779513082320876798154814114;}
#define PIf 3.1415926535897932384626433832795f
#define PI  3.14159265358979323846264338327950288419716939937510

void EulerToRotationMatrix(IN const float HeadingPitchRoll[3], OUT float matRotation[16]);
void EulerToViewMatrix(IN const float HeadingPitchRoll[3], OUT float matView[16]);
void RotationMatrixToEuler(IN const float matRotation[16], OUT float HeadingPitchRoll[3]);
}
