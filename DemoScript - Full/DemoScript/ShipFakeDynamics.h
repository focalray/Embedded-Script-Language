#pragma once


// Realtime Fake Ship Dynamics(2-way dynamic) 
// Fine-tune version.
// jhkim(Redperf)
#pragma warning(disable: 4244)

#include "CPUPerlinNoise.h"


class CShipFakeDynamics
{
	CCPUPerlinNoise m_CPUPNoise;

public:
	float fDynamicTimeSlide;
	float fDynamicTimeSlideRate;
	float fShipLength;
	float fMaxWaveLevel;
	float fBoatAltBias_Front;
	float fBoatAltBias_Back;
	float fBoatAltBias_Swing;
	float matShipPose[16];
	float fShipAlt;

public:
	CShipFakeDynamics(float TimeRate = 0.3f, float ShipLen = 50.0f, float MaxWaveLevel = 20.0f)
	{
		fDynamicTimeSlide = 0.0f;
		fDynamicTimeSlideRate = TimeRate;
		fShipLength	= ShipLen;
		fMaxWaveLevel = MaxWaveLevel;
		fBoatAltBias_Front = 0.0f;
		fBoatAltBias_Back = 0.0f;
		fBoatAltBias_Swing = 0.0f;
		fShipAlt = 0.0f;
		memset(matShipPose, 0, sizeof(matShipPose));
	}

	void Update(float dt, float front_alt, float back_alt, float CurWaveLevel, double dShipPos[3], float matShip[16])
	{
		const float SwingRange = 10.f; // degree
		const float WaveRoofAltMax = 4.0f; // meter

		fDynamicTimeSlide += fDynamicTimeSlideRate * dt;

		float fWaveLevelFactor = CurWaveLevel / fMaxWaveLevel;
		if(fWaveLevelFactor > 1.0f)
			fWaveLevelFactor = 1.0f;


		// Back noise follows Front by some numerical distance.
		// Back noise is down scaled by 10% comparing to Front.
		fBoatAltBias_Front = m_CPUPNoise.Noise(fDynamicTimeSlide, fDynamicTimeSlide + fShipLength * 0.1f);
		fBoatAltBias_Back = m_CPUPNoise.Noise(fDynamicTimeSlide, fDynamicTimeSlide) * 0.1f;
		fBoatAltBias_Swing = m_CPUPNoise.Noise(fDynamicTimeSlide, fDynamicTimeSlide) * 1.2f;
			

		static float fBoatAltBias_Front_last = fBoatAltBias_Front;
		static float fBoatAltBias_Back_last = fBoatAltBias_Back;


		// lifting force---
		if(fBoatAltBias_Front > fBoatAltBias_Front_last)
			fBoatAltBias_Front += 0.02f * dt;

		if(front_alt > dShipPos[2])
			fBoatAltBias_Front += 0.05f * dt;

		if(fBoatAltBias_Back > fBoatAltBias_Back_last)
			fBoatAltBias_Back += 0.02f * dt;

		if(back_alt > dShipPos[2])
			fBoatAltBias_Back += 0.05f * dt;


		// down force---
		if(fBoatAltBias_Front < fBoatAltBias_Front_last)
			fBoatAltBias_Front -= 0.004f * dt;

		if(fBoatAltBias_Back < fBoatAltBias_Back_last)
			fBoatAltBias_Back -= 0.004f * dt;

		fBoatAltBias_Front_last = fBoatAltBias_Front;
		fBoatAltBias_Back_last = fBoatAltBias_Back;

		fBoatAltBias_Swing *= fWaveLevelFactor;



		// calc new up vector---
		D3DXVECTOR3 vdir(matShip[4], matShip[5], matShip[6]);
		D3DXVECTOR3 vright(matShip[0], matShip[1], matShip[2]);

		D3DXVECTOR3 vfront_pos(dShipPos[0], dShipPos[1] + matShip[5]*(fShipLength*0.5f), 
								dShipPos[2] + fBoatAltBias_Front * WaveRoofAltMax);
		D3DXVECTOR3 vback_pos(dShipPos[0], dShipPos[1] - matShip[5]*(fShipLength*0.5f), 
								dShipPos[2] + fBoatAltBias_Back * WaveRoofAltMax);

		D3DXVECTOR3 vcur_pitch_line = vfront_pos - vback_pos;
		D3DXVECTOR3 vcur_guide_line(vcur_pitch_line.x, vcur_pitch_line.y, 0.0f);

		D3DXVec3Normalize(&vcur_pitch_line, &vcur_pitch_line);
		D3DXVec3Normalize(&vcur_guide_line, &vcur_guide_line);


		float fDot_pitch = D3DXVec3Dot(&vcur_pitch_line, &vcur_guide_line);
		if(fDot_pitch > 1.0f)
			fDot_pitch = 1.0f;
		else if(fDot_pitch < -1.0f)
			fDot_pitch = -1.0f;

		float fAngRad_pitch = acos(fDot_pitch) * fWaveLevelFactor;

		if(vcur_pitch_line.z <= 0.0f)
			fAngRad_pitch = -fAngRad_pitch; // reverse pitching

		fShipAlt = (fBoatAltBias_Front + fBoatAltBias_Back) * 0.5f * WaveRoofAltMax * fWaveLevelFactor;

		//char msg[256];
		//sprintf(msg, "dt=%f, fAngRad_pitch= %f\n", dt, fAngRad_pitch);
		//OutputDebugStringA(msg);

		float matOri[16];
		D3DXMatrixRotationAxis((D3DXMATRIX*)matOri, &vright, fAngRad_pitch);

		float SwingAngle = SwingRange * fBoatAltBias_Swing;
		float matSide[16];
		D3DXMatrixRotationAxis((D3DXMATRIX*)matSide, &vdir, Radian(SwingAngle));
		
		D3DXMatrixMultiply((D3DXMATRIX*)matShipPose, (D3DXMATRIX*)matShip, (D3DXMATRIX*)matSide);	
		D3DXMatrixMultiply((D3DXMATRIX*)matShipPose, (D3DXMATRIX*)matShipPose, (D3DXMATRIX*)matOri);	
	}

	double GetAltitude()
	{
		return fShipAlt;
	}
	
	void GetPoseMatrix(float mat[16])
	{
		memcpy(mat, matShipPose, sizeof(matShipPose));
	}
};


