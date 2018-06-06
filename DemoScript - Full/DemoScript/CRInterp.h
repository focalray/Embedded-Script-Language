/////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008 Haangilsoft, Ltd.
// #202, Jang YoungShill Hall, 1688-5 Shinil-dong, Daeduk-gu, Daejeon Metropolitan City, Korea, 306-230
// This code may not by copied or distributed or reused without permission from Haangilsoft.
//
// Created : 2009.09.23
// File Name : CatmullRomInterpolation.h
// Desc: Catmull-Rom spline interpolator which has a parameter for bending a curve(tension).
// Author : Jinhong Kim  <marujet@haangilsoft.com>
/////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _CATMULLROMINTRP_H
#define _CATMULLROMINTRP_H


#include <vector>
using namespace std;
#include "assert.h"




namespace HaanUtils
{

struct SIMDATA 
{ 
	double 	Pos[3];
	float 	Ori[3];
	float	Dir[3];
	float	Vel[3];

	SIMDATA() 
	{
		Pos[0] = Pos[1] = Pos[2] = 0.0;
		Ori[0] = Ori[1] = Ori[2] = 0.0f;
		Dir[0] = Dir[1] = Dir[2] = 0.0f;
		Vel[0] = Vel[1] = Vel[2] = 0.0f;
	}
};


class CCatmullRomInterp
{
public:
	CCatmullRomInterp()
	{
		// Build up the default matrix.
		GenerateMatrix();
	}
	~CCatmullRomInterp()
	{
		Clear();
	}

	void GenerateMatrix(double tau = 0.5)
	{
		double mat[4][4] = {{-2.0*tau, 2.0*(2.0-tau), 2.0*(tau-2.0), 2.0*tau},
							{4.0*tau, 2.0*(tau-3.0), 2.0*(3.0-2.0*tau), -2.0*tau},
							{-2.0*tau, 0, 2.0*tau, 0},
							{0, 2.0, 0, 0}};
		memcpy(MatCatMullRom, mat, sizeof(mat));
	}

	void AddControlPoint(SIMDATA& cp)
	{
		if(list_ctrlpoints.size() == 0)
		{
			list_ctrlpoints.push_back(cp); // a dummy; for later use
		}

		list_ctrlpoints.push_back(cp);
	}

	void GetInterpPoint(int offset, double t, SIMDATA& out)
	{
		out.Pos[0] = InterpCtrlPt1D<double>(
			list_ctrlpoints[offset + 0].Pos[0],
			list_ctrlpoints[offset + 1].Pos[0],
			list_ctrlpoints[offset + 2].Pos[0],
			list_ctrlpoints[offset + 3].Pos[0], t);
		out.Pos[1] = InterpCtrlPt1D<double>(
			list_ctrlpoints[offset + 0].Pos[1],
			list_ctrlpoints[offset + 1].Pos[1],
			list_ctrlpoints[offset + 2].Pos[1],
			list_ctrlpoints[offset + 3].Pos[1], t);
		out.Pos[2] = InterpCtrlPt1D<double>(
			list_ctrlpoints[offset + 0].Pos[2],
			list_ctrlpoints[offset + 1].Pos[2],
			list_ctrlpoints[offset + 2].Pos[2],
			list_ctrlpoints[offset + 3].Pos[2], t);

		out.Ori[0] = InterpCtrlPt1D<float>(
			list_ctrlpoints[offset + 0].Ori[0],
			list_ctrlpoints[offset + 1].Ori[0],
			list_ctrlpoints[offset + 2].Ori[0],
			list_ctrlpoints[offset + 3].Ori[0], (float)t);
		out.Ori[1] = InterpCtrlPt1D<float>(
			list_ctrlpoints[offset + 0].Ori[1],
			list_ctrlpoints[offset + 1].Ori[1],
			list_ctrlpoints[offset + 2].Ori[1],
			list_ctrlpoints[offset + 3].Ori[1], (float)t);
		out.Ori[2] = InterpCtrlPt1D<float>(
			list_ctrlpoints[offset + 0].Ori[2],
			list_ctrlpoints[offset + 1].Ori[2],
			list_ctrlpoints[offset + 2].Ori[2],
			list_ctrlpoints[offset + 3].Ori[2], (float)t);
	}

	static double CalcDistance(double v1[3], double v2[3])
	{
		double dRes = sqrt( ( v2[0] - v1[0] ) * ( v2[0] - v1[0] ) +
			( v2[1] - v1[1] ) * ( v2[1] - v1[1] ) +
			( v2[2] - v1[2] ) * ( v2[2] - v1[2] ) );
		return dRes;
	}

	int GetInterpUniformPointArray(std::vector<SIMDATA>& arr, int nSample)
	{
		int nTotalCpts = list_ctrlpoints.size();
		if(nTotalCpts < 3)
			return 0;

		SIMDATA s_first = list_ctrlpoints[1];
		SIMDATA s_first_next = list_ctrlpoints[2];
		SIMDATA s_first_before;
		s_first_before.Pos[0] = s_first.Pos[0] + (s_first.Pos[0] - s_first_next.Pos[0]);
		s_first_before.Pos[1] = s_first.Pos[1] + (s_first.Pos[1] - s_first_next.Pos[1]);
		s_first_before.Pos[2] = s_first.Pos[2] + (s_first.Pos[2] - s_first_next.Pos[2]);
		list_ctrlpoints[0] = s_first_before;  // replace dummy.

		SIMDATA s_last = list_ctrlpoints[nTotalCpts-1];
		SIMDATA s_last_before = list_ctrlpoints[nTotalCpts-2];
		SIMDATA s_last_next;
		s_last_next.Pos[0] = s_last.Pos[0] + (s_last.Pos[0] - s_last_before.Pos[0]);
		s_last_next.Pos[1] = s_last.Pos[1] + (s_last.Pos[1] - s_last_before.Pos[1]);
		s_last_next.Pos[2] = s_last.Pos[2] + (s_last.Pos[2] - s_last_before.Pos[2]);
		list_ctrlpoints.push_back(s_last_next);

		nTotalCpts = list_ctrlpoints.size(); // get total ctrl points again.

		std::vector<double> du_list;


		arr.clear();

		// calc Td
		double Td = 0.0;
		for(int i=1; i<nTotalCpts-2; i++)
		{
			SIMDATA& smp1 = list_ctrlpoints[i];
			SIMDATA& smp2 = list_ctrlpoints[i+1];

			double du = CalcDistance(smp1.Pos, smp2.Pos);

			Td += du;
			du_list.push_back(du);
		}

		for(unsigned int i=0; i<du_list.size(); i++)
		{
			double du = du_list[i];
			double r = 0.0;
			if(0.0 == Td)
				r = 1.0;
			else
				r = du / Td;
			double rS = r * (double)nSample;
			double step = 1.0 / rS;

			double t=0.0;
			for(; t<=1.0; t+=step)
			{
				SIMDATA out;
				GetInterpPoint(i, t, out);
				arr.push_back(out);
			}
		}

		return arr.size();
	}

	DWORD GetCtrlPtCount()
	{
		return list_ctrlpoints.size()-2;
	}

	void Clear()
	{
		list_ctrlpoints.clear();
	}

protected:
	template<class T>
	T InterpCtrlPt1D(T p1, T p2, T p3, T p4, T t)
	{
		T t2 = t*t;
		T t3 = t2*t;
		return (T)(0.5 * 
			((MatCatMullRom[0][0]*t3 + MatCatMullRom[1][0]*t2 + MatCatMullRom[2][0]*t + MatCatMullRom[3][0])*p1 +
			(MatCatMullRom[0][1]*t3 + MatCatMullRom[1][1]*t2 + MatCatMullRom[2][1]*t + MatCatMullRom[3][1])*p2 + 
			(MatCatMullRom[0][2]*t3 + MatCatMullRom[1][2]*t2 + MatCatMullRom[2][2]*t + MatCatMullRom[3][2])*p3 + 
			(MatCatMullRom[0][3]*t3 + MatCatMullRom[1][3]*t2 + MatCatMullRom[2][3]*t + MatCatMullRom[3][3])*p4)); 
	}


protected:
	double MatCatMullRom[4][4];
	std::vector<SIMDATA> list_ctrlpoints;
};
};
#endif
