
#include "stdafx.h"
#include <math.h>

#define IN
#define OUT

namespace Hx
{
void EulerToRotationMatrix(IN const float HeadingPitchRoll[3], OUT float matRotation[16])
{
    float cosHeading = cosf(HeadingPitchRoll[0]);
    float sinHeading = sinf(HeadingPitchRoll[0]);
    float cosPitch = cosf(HeadingPitchRoll[1]);
    float sinPitch = sinf(HeadingPitchRoll[1]);
    float cosRoll = cosf(HeadingPitchRoll[2]);
    float sinRoll = sinf(HeadingPitchRoll[2]);
    matRotation[0] = cosRoll * cosHeading;
    matRotation[1] = -cosRoll * sinHeading;
    matRotation[2] = -sinRoll;
    matRotation[3] = 0;
    matRotation[4] = cosPitch * sinHeading;
    matRotation[5] = cosPitch * cosHeading;
    matRotation[6] = sinPitch;
    matRotation[7] = 0;
    matRotation[8] = matRotation[1] * matRotation[6] - matRotation[2] * matRotation[5];
    matRotation[9] = matRotation[2] * matRotation[4] - matRotation[0] * matRotation[6];
    matRotation[10] = matRotation[0] * matRotation[5] - matRotation[1] * matRotation[4];
    matRotation[11] = 0;
    matRotation[12] = 0;
    matRotation[13] = 0;
    matRotation[14] = 0;
    matRotation[15] = 1;
}

void EulerToViewMatrix(IN const float HeadingPitchRoll[3], OUT float matView[16])
{
    float cosHeading = cosf(HeadingPitchRoll[0]);
    float sinHeading = sinf(HeadingPitchRoll[0]);
    float cosPitch = cosf(HeadingPitchRoll[1]);
    float sinPitch = sinf(HeadingPitchRoll[1]);
    float cosRoll = cosf(HeadingPitchRoll[2]);
    float sinRoll = sinf(HeadingPitchRoll[2]);

    matView[0] = cosRoll * cosHeading;
    matView[4] = -cosRoll * sinHeading;
    matView[8] = -sinRoll;
    matView[12] = 0;
    matView[1] = cosPitch * sinHeading;
    matView[5] = cosPitch * cosHeading;
    matView[9] = sinPitch;
    matView[13] = 0;
    matView[2] = matView[4] * matView[9] - matView[8] * matView[5];
    matView[6] = matView[8] * matView[1] - matView[0] * matView[9];
    matView[10] = matView[0] * matView[5] - matView[4] * matView[1];
    matView[14] = 0;
    matView[3] = 0;
    matView[7] = 0;
    matView[11] = 0;
    matView[15] = 1;
}

void RotationMatrixToEuler(IN const float matRotation[16], OUT float HeadingPitchRoll[3])
{
//    HeadingPitchRoll[0] = atan2f(matRotation[5], matRotation[4]);
    HeadingPitchRoll[0] = atan2f(matRotation[4], matRotation[5]);
    HeadingPitchRoll[1] = asinf(matRotation[6]);
    HeadingPitchRoll[2] = -asinf(matRotation[2]);
}

}

