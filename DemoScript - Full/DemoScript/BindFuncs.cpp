///////////////////////////////////////////////////////////////////////////////////////////////////////
// SCRIPT BINDING FUNCTIONS(HOST FUNCTION)
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindFuncs.h"

using namespace Hx;


extern CCitrus									esl;
extern int										SimTime;
extern int										SimFreq;
extern int										SimTick;
extern int										SimTickSec;
extern unsigned __int64							SimTimeStamp;
extern double									SimEventProgress;
extern double									SimEventProgressReturn;
extern double									SimEventProgressSmooth;
extern double									SimEventProgressSmoothReturn;
extern char										SimCameraFollowTarget[256];
extern double									SimCameraFollowDist[3];
extern double									SimCameraFollowAngleOffset[3];
extern char										SimCameraFollowAngleLookAt[256];
extern double									SimCameraLookatOffset[3];

extern HXCONTROL_MOVEENTITY						svHXCONTROL_MOVEENTITY;
extern HXCONTROL_ACTIVECAMERA					svHXCONTROL_ACTIVECAMERA;
extern HXCONTROL_KILLENTITY						svHXCONTROL_KILLENTITY;
extern HXCONTROL_ATMOSPHERE						svHXCONTROL_ATMOSPHERE;
extern HXCONTROL_SCENECOMPOSITION				svHXCONTROL_SCENECOMPOSITION;
extern HXCONTROL_EMITPARTICLE					svHXCONTROL_EMITPARTICLE;
extern HXCONTROL_ONEWAVE						svHXCONTROL_ONEWAVE;
extern HXCONTROL_TORPEDOHIT						svHXCONTROL_TORPEDOHIT;
extern HXCONTROL_EXPLOSION						svHXCONTROL_EXPLOSION;
extern HXCONTROL_ROCKETTRAIL					svHXCONTROL_ROCKETTRAIL;
extern HXCONTROL_CAVITATIONBUBBLE				svHXCONTROL_CAVITATIONBUBBLE;
extern HXCONTROL_CAMERA							svHXCONTROL_CAMERA;
extern HXCONTROL_CAMERAOFFSET					svHXCONTROL_CAMERAOFFSET;
extern HXCONTROL_OBJECTWATERCOUPLING_REGION		svHXCONTROL_OBJECTWATERCOUPLING_REGION;
extern HXCONTROL_OBJECTWATERCOUPLING_ADDOBJECT	svHXCONTROL_OBJECTWATERCOUPLING_ADDOBJECT;
extern HXCONTROL_GUIDELINE						svHXCONTROL_GUIDELINE;
extern HXCONTROL_WEATHER						svHXCONTROL_WEATHER;
extern HXCONTROL_UNDERWATEREXPLOSION			svHXCONTROL_UNDERWATEREXPLOSION;
extern HXCONTROL_UNDERWATER_IMPURITY			svHXCONTROL_UNDERWATER_IMPURITY;
extern HXCONTROL_SWELL							svHXCONTROL_SWELL;


extern ENTITY_SCRIPT							svEntityInfoSnapshot;
extern DYNAMIC_STATE_MISSILE					g_DSHarpoon;
extern DYNAMIC_STATE_MISSILE					g_DSTorpedo[NUMTOR];
extern DYNAMIC_STATE_MISSILE					g_DSDecoy[NUMDECOY];
extern EXPLOSION_STATE							g_Explosions[1+NUMTOR];

extern std::vector<EVENT>						g_event_list;
extern std::vector<EVENT>						g_frameevent_list;
extern std::vector<EVENT>						g_booking_event_list;
extern std::vector<EVENT>						g_booking_frameevent_list;
extern std::vector<PATHNODE>					g_path_list[100];
extern std::vector<HaanUtils::SIMDATA>			g_path_build_list[100];
extern std::vector<ENTITY>						g_entity_list;
extern VEC										g_CDTesterPos;
extern BOOL										g_OnEventStage;



////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_NewEvent(char name[256], int period, int repeat)
{
	EVENT ev;
	strcpy_s(ev.name, 256, name);
	ev.period	= period;
	ev.repeat	= repeat;
	ev.knocked	= 0;
	ev.triggered = 0;
	ev.sip		= esl.getfunc(name);
	ev.sip_init = -1;

	if(g_OnEventStage)
		g_booking_event_list.push_back(ev);
	else
		g_event_list.push_back(ev);

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_NewFrameEvent(char name[256], int period, int repeat, char cb_init[256])
{
	EVENT ev;
	strcpy_s(ev.name, 256, name);
	ev.period	= period;
	ev.repeat	= repeat;
	ev.knocked	= 0;
	ev.triggered = 0;
	ev.sip		= esl.getfunc(name);
	ev.sip_init = esl.getfunc(cb_init);

	if(g_OnEventStage)
		g_booking_frameevent_list.push_back(ev);
	else
		g_frameevent_list.push_back(ev);

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetActiveCamera(int cam)
{
	svHXCONTROL_ACTIVECAMERA.Code = HXCODE_ACTIVECAMERA;
	svHXCONTROL_ACTIVECAMERA.Length = sizeof(svHXCONTROL_ACTIVECAMERA);
	svHXCONTROL_ACTIVECAMERA.iCamera = cam;
	AddPacket(&svHXCONTROL_ACTIVECAMERA, sizeof(svHXCONTROL_ACTIVECAMERA));

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetEntityPosOri(char name[256], 
							double x, double y, double z, double h, double p, double r)
{
	int iEnt = nonexposed_GetEntity(name);
	if(-1 == iEnt)
		return -1;
	
	ENTITY& ent = g_entity_list[iEnt];

	svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
	svHXCONTROL_MOVEENTITY.Length = sizeof(Hx::HXCONTROL_MOVEENTITY);
	svHXCONTROL_MOVEENTITY.iEntity = atoi(name);
	svHXCONTROL_MOVEENTITY.Flags = 0x50;
	svHXCONTROL_MOVEENTITY.Pos[0] = x;
	svHXCONTROL_MOVEENTITY.Pos[1] = y;
	svHXCONTROL_MOVEENTITY.Pos[2] = z;

	float hpr[3] = {(float)Radian(h), (float)Radian(p), (float)Radian(r)};
	EulerToRotationMatrix(hpr, svHXCONTROL_MOVEENTITY.matRot);

	AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));

	ent.pos.x = x;
	ent.pos.y = y;
	ent.pos.z = z;
	ent.ori.h = (float)h;
	ent.ori.p = (float)p;
	ent.ori.r = (float)r;
	ent.dir.x = svHXCONTROL_MOVEENTITY.matRot[4];
	ent.dir.y = svHXCONTROL_MOVEENTITY.matRot[5];
	ent.dir.z = svHXCONTROL_MOVEENTITY.matRot[6];
	ent.valid = 1;

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_TranslateEntityPos(char name[256], char ref[256], double x, double y, double z)
{
	int iEnt = nonexposed_GetEntity(name);
	if(-1 == iEnt)
		return -1;
	
	ENTITY& ent = g_entity_list[iEnt];

	int iRef = nonexposed_GetEntity(ref);
	if(-1 == iRef)
		return -1;

	D3DXVECTOR3 vup(0, 0, 1);
	D3DXVECTOR3 vdir((float)g_entity_list[iRef].dir.x, 
		(float)g_entity_list[iRef].dir.y, 
		(float)g_entity_list[iRef].dir.z);
	D3DXVec3Normalize(&vdir, &vdir);
	D3DXVECTOR3 vright;
	D3DXVec3Cross(&vright, &vdir, &vup);
	D3DXVec3Cross(&vup, &vright, &vdir);

	VEC pos;
	pos.x = g_entity_list[iRef].pos.x;
	pos.y = g_entity_list[iRef].pos.y;
	pos.z = g_entity_list[iRef].pos.z;
	pos.x += x * vright.x;
	pos.y += x * vright.y;
	pos.z += x * vright.z;
	pos.x += y * vdir.x;
	pos.y += y * vdir.y;
	pos.z += y * vdir.z;
	pos.x += z * vup.x;
	pos.y += z * vup.y;
	pos.z += z * vup.z;

	svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
	svHXCONTROL_MOVEENTITY.Length = sizeof(Hx::HXCONTROL_MOVEENTITY);
	svHXCONTROL_MOVEENTITY.iEntity = atoi(name);
	svHXCONTROL_MOVEENTITY.Flags = 0x10;
	svHXCONTROL_MOVEENTITY.Pos[0] = pos.x;
	svHXCONTROL_MOVEENTITY.Pos[1] = pos.y;
	svHXCONTROL_MOVEENTITY.Pos[2] = pos.z;
	AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));

	ent.pos.x = pos.x;
	ent.pos.y = pos.y;
	ent.pos.z = pos.z;
	ent.valid = 1;

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetCameraPosOri(double x, double y, double z, double h, double p, double r)
{
	svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
	svHXCONTROL_MOVEENTITY.Length = sizeof(svHXCONTROL_MOVEENTITY);
	svHXCONTROL_MOVEENTITY.Pos[0] = x;
	svHXCONTROL_MOVEENTITY.Pos[1] = y;
	svHXCONTROL_MOVEENTITY.Pos[2] = z;
	svHXCONTROL_MOVEENTITY.iEntity = 1;
	svHXCONTROL_MOVEENTITY.Flags = 0x10 | 0x40;

	float rot[3] = {(float)Radian(h), (float)Radian(p), (float)Radian(r)};
	EulerToRotationMatrix(rot, svHXCONTROL_MOVEENTITY.matRot);

	AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));

	ENTITY& ce = g_entity_list[0];
	ce.pos.x = x;
	ce.pos.y = y;
	ce.pos.z = z;
	ce.ori.h = (float)h;
	ce.ori.p = (float)p;
	ce.ori.r = (float)r;

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetCameraPos(double x, double y, double z)
{
	svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
	svHXCONTROL_MOVEENTITY.Length = sizeof(svHXCONTROL_MOVEENTITY);
	svHXCONTROL_MOVEENTITY.Pos[0] = x;
	svHXCONTROL_MOVEENTITY.Pos[1] = y;
	svHXCONTROL_MOVEENTITY.Pos[2] = z;
	svHXCONTROL_MOVEENTITY.iEntity = 1;
	svHXCONTROL_MOVEENTITY.Flags = 0x10;

	AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));

	ENTITY& ce = g_entity_list[0];
	ce.pos.x = (float)x;
	ce.pos.y = (float)y;
	ce.pos.z = (float)z;

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetCameraPosLookat(double x, double y, double z, char name[256])
{
	svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
	svHXCONTROL_MOVEENTITY.Length = sizeof(svHXCONTROL_MOVEENTITY);
	svHXCONTROL_MOVEENTITY.Pos[0] = x;
	svHXCONTROL_MOVEENTITY.Pos[1] = y;
	svHXCONTROL_MOVEENTITY.Pos[2] = z;
	svHXCONTROL_MOVEENTITY.iEntity = 1;
	svHXCONTROL_MOVEENTITY.Flags = 0x10;

	AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));

	ENTITY& ce = g_entity_list[0];
	ce.pos.x = (float)x;
	ce.pos.y = (float)y;
	ce.pos.z = (float)z;

	nonexposed_SetCameraLookat(name);

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetCameraOri(double h, double p, double r)
{
	svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
	svHXCONTROL_MOVEENTITY.Length = sizeof(svHXCONTROL_MOVEENTITY);
	svHXCONTROL_MOVEENTITY.iEntity = 1;
	svHXCONTROL_MOVEENTITY.Flags = 0x40;

	float rot[3] = {(float)Radian(h), (float)Radian(p), (float)Radian(r)};
	EulerToRotationMatrix(rot, svHXCONTROL_MOVEENTITY.matRot);

	AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));

	ENTITY& ce = g_entity_list[0];
	ce.ori.h = (float)h;
	ce.ori.p = (float)p;
	ce.ori.r = (float)r;

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void nonexposed_SetCameraOri(float matRot[16])
{
	svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
	svHXCONTROL_MOVEENTITY.Length = sizeof(svHXCONTROL_MOVEENTITY);
	svHXCONTROL_MOVEENTITY.iEntity = 1;
	svHXCONTROL_MOVEENTITY.Flags = 0x40;
	memcpy(svHXCONTROL_MOVEENTITY.matRot, matRot, sizeof(float)*16);

	AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));

	float ori[3] = {0,};
	RotationMatrixToEuler(matRot, ori);

	ENTITY& ce = g_entity_list[0];
	ce.ori.h = (float)Degree(ori[0]);
	ce.ori.p = (float)Degree(ori[1]);
	ce.ori.r = (float)Degree(ori[2]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetCameraFollowOffset(char name[256], 
										double dx, double dy, double dz, 
										double aoh, double aop, double aor)
{
	strcpy_s(SimCameraFollowTarget, 256, name);
	SimCameraFollowDist[0] = dx;
	SimCameraFollowDist[1] = dy;
	SimCameraFollowDist[2] = dz;
	SimCameraFollowAngleOffset[0] = aoh;
	SimCameraFollowAngleOffset[1] = aop;
	SimCameraFollowAngleOffset[2] = aor;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_StopCameraFollow()
{
	SimCameraFollowTarget[0] = NULL;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetCameraLookatOffset(double aoh, double aop, double aor)
{
	SimCameraLookatOffset[0] = aoh;
	SimCameraLookatOffset[1] = aop;
	SimCameraLookatOffset[2] = aor;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int nonexposed_SetCameraLookat(char name[256])
{
	if(0 == bindcall_QueryEntityInfo(name))
		return 0;

	ENTITY& ce = g_entity_list[0];

	float lookatvec[3] = 
	{
		(float)(svEntityInfoSnapshot.pos.x - ce.pos.x),
		(float)(svEntityInfoSnapshot.pos.y - ce.pos.y),
		(float)(svEntityInfoSnapshot.pos.z - ce.pos.z),
	};
	D3DXVec3Normalize((D3DXVECTOR3*)&lookatvec, (D3DXVECTOR3*)&lookatvec);

	// heading onto target ---
	float matOri[16];
	D3DXMatrixIdentity((D3DXMATRIX*)matOri);

	// dir
	matOri[4] = lookatvec[0];
	matOri[5] = lookatvec[1];
	matOri[6] = lookatvec[2];

	// right
	D3DXVec3Cross((D3DXVECTOR3*)&matOri[0], (D3DXVECTOR3*)&matOri[4], (D3DXVECTOR3*)&matOri[8]);
	D3DXVec3Normalize((D3DXVECTOR3*)&matOri[0], (D3DXVECTOR3*)&matOri[0]);

	// up
	D3DXVec3Cross((D3DXVECTOR3*)&matOri[8], (D3DXVECTOR3*)&matOri[0], (D3DXVECTOR3*)&matOri[4]);
	D3DXVec3Normalize((D3DXVECTOR3*)&matOri[8], (D3DXVECTOR3*)&matOri[8]);

	nonexposed_SetCameraOri(matOri);

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetCameraLookat(char name[256])
{
	strcpy_s(SimCameraFollowAngleLookAt, 256, name);

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_QueryEntityInfo(char name[256])
{
	if(NULL == name[0])
		return 0;

	int nEntity = g_entity_list.size();
	for(int kEntity=0; kEntity<nEntity; kEntity++)
	{
		ENTITY& ent = g_entity_list[kEntity];

		if(ent.valid && _stricmp(name, ent.name) == 0)
		{
			memcpy(&svEntityInfoSnapshot.pos, &ent.pos, sizeof(VEC));
			memcpy(&svEntityInfoSnapshot.ori, &ent.ori, sizeof(ORI));
			memcpy(&svEntityInfoSnapshot.dir, &ent.dir, sizeof(VEC));
			memcpy(&svEntityInfoSnapshot.vel, &ent.vel, sizeof(VEC));
			return 1;
		}
	}

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_ResetDynamics(char name[256])
{
	if(_stricmp(name, "Harpoon") == 0)
	{
		g_DSHarpoon.live = 0;
		g_DSHarpoon.frame = 0;
	}
	if(_stricmp(name, "Torpedo1") == 0)
		g_DSTorpedo[0].frame = 0;
	if(_stricmp(name, "Torpedo2") == 0)
		g_DSTorpedo[1].frame = 0;
	if(_stricmp(name, "Torpedo3") == 0)
		g_DSTorpedo[2].frame = 0;
	if(_stricmp(name, "Torpedo4") == 0)
		g_DSTorpedo[3].frame = 0;
	if(_stricmp(name, "Torpedo5") == 0)
		g_DSTorpedo[4].frame = 0;
	if(_stricmp(name, "Torpedo6") == 0)
		g_DSTorpedo[5].frame = 0;
	if(_stricmp(name, "Torpedo7") == 0)
		g_DSTorpedo[6].frame = 0;
	if(_stricmp(name, "Torpedo8") == 0)
		g_DSTorpedo[7].frame = 0;
	if(_stricmp(name, "Torpedo9") == 0)
		g_DSTorpedo[8].frame = 0;
	if(_stricmp(name, "Torpedo10") == 0)
		g_DSTorpedo[9].frame = 0;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_RunDynamics(char name[256])
{
	if(_stricmp(name, "Harpoon") == 0)
	{
		g_DSHarpoon.live = 1;
		g_DSHarpoon.frame = 0;
	}
	else if(_stricmp(name, "Decoy1") == 0)
	{
		g_DSDecoy[0].live = 1;
		g_DSDecoy[0].frame = 0;
	}
	else if(_stricmp(name, "Decoy2") == 0)
	{
		g_DSDecoy[1].live = 1;
		g_DSDecoy[1].frame = 0;
	}
	else if(_stricmp(name, "Torpedo1") == 0)
	{
		g_DSTorpedo[0].live = 1;
		g_DSTorpedo[0].frame = 0;
	}
	else if(_stricmp(name, "Torpedo2") == 0)
	{
		g_DSTorpedo[1].live = 1;
		g_DSTorpedo[1].frame = 0;
	}
	else if(_stricmp(name, "Torpedo3") == 0)
	{
		g_DSTorpedo[2].live = 1;
		g_DSTorpedo[2].frame = 0;
	}
	else if(_stricmp(name, "Torpedo4") == 0)
	{
		g_DSTorpedo[3].live = 1;
		g_DSTorpedo[3].frame = 0;
	}
	else if(_stricmp(name, "Torpedo5") == 0)
	{
		g_DSTorpedo[4].live = 1;
		g_DSTorpedo[4].frame = 0;
	}
	else if(_stricmp(name, "Torpedo6") == 0)
	{
		g_DSTorpedo[5].live = 1;
		g_DSTorpedo[5].frame = 0;
	}
	else if(_stricmp(name, "Torpedo7") == 0)
	{
		g_DSTorpedo[6].live = 1;
		g_DSTorpedo[6].frame = 0;
	}
	else if(_stricmp(name, "Torpedo8") == 0)
	{
		g_DSTorpedo[7].live = 1;
		g_DSTorpedo[7].frame = 0;
	}
	else if(_stricmp(name, "Torpedo9") == 0)
	{
		g_DSTorpedo[8].live = 1;
		g_DSTorpedo[8].frame = 0;
	}
	else if(_stricmp(name, "Torpedo10") == 0)
	{
		g_DSTorpedo[9].live = 1;
		g_DSTorpedo[9].frame = 0;
	}

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_StopDynamics(char name[256])
{
	if(_stricmp(name, "Harpoon") == 0)
		g_DSHarpoon.live = 0;
	else if(_stricmp(name, "Decoy1") == 0)
		g_DSDecoy[0].live = 0;
	else if(_stricmp(name, "Decoy2") == 0)
		g_DSDecoy[1].live = 0;
	else if(_stricmp(name, "Torpedo1") == 0)
		g_DSTorpedo[0].live = 0;
	else if(_stricmp(name, "Torpedo2") == 0)
		g_DSTorpedo[1].live = 0;
	else if(_stricmp(name, "Torpedo3") == 0)
		g_DSTorpedo[2].live = 0;
	else if(_stricmp(name, "Torpedo4") == 0)
		g_DSTorpedo[3].live = 0;
	else if(_stricmp(name, "Torpedo5") == 0)
		g_DSTorpedo[4].live = 0;
	else if(_stricmp(name, "Torpedo6") == 0)
		g_DSTorpedo[5].live = 0;
	else if(_stricmp(name, "Torpedo7") == 0)
		g_DSTorpedo[6].live = 0;
	else if(_stricmp(name, "Torpedo8") == 0)
		g_DSTorpedo[7].live = 0;
	else if(_stricmp(name, "Torpedo9") == 0)
		g_DSTorpedo[8].live = 0;
	else if(_stricmp(name, "Torpedo10") == 0)
		g_DSTorpedo[9].live = 0;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_UpdateDynamics_Harpoon(char name[256], char ref[256],
			double x1, double y1, double z1,
			double x2, double y2, double z2,
			double altmax,
			double speed1, double speed2,
			char contact[256])
{
	if(0 == g_DSHarpoon.live)
		return 0;

	//CCatmullRomInterp::CalcDistance
	if(0 == g_DSHarpoon.frame)
	{// init
		g_DSHarpoon.sip_collision = esl.getfunc(contact);

		VEC vec(x2 - x1, y2 - y1, z2 - z1);
		_Normalize(vec);

		int iEnt = nonexposed_GetEntity(ref);
		if(-1 == iEnt)
			return -1;

		VEC edir;
		edir.x = g_entity_list[iEnt].dir.x;
		edir.y = g_entity_list[iEnt].dir.y;
		edir.z = g_entity_list[iEnt].dir.z;

		g_DSHarpoon.nextcp.x = x1 + edir.x * 200.0;
		g_DSHarpoon.nextcp.y = y1 + edir.y * 200.0;
		g_DSHarpoon.nextcp.z = altmax;

		g_DSHarpoon.dir.x = edir.x;
		g_DSHarpoon.dir.y = edir.y;
		g_DSHarpoon.dir.z = edir.z;

		g_DSHarpoon.pos.x = x1;
		g_DSHarpoon.pos.y = y1;
		g_DSHarpoon.pos.z = z1;

		g_DSHarpoon.stage = 0;
	}
	else
	{// update
		// update dir
		VEC v(g_DSHarpoon.pos.x-x2, g_DSHarpoon.pos.y-y2, g_DSHarpoon.pos.z-z2);
		double dist = _CalcLength(v);

		if(dist < 2.0)
		{// collision
			g_DSHarpoon.live = 0;
			g_DSHarpoon.boom = 1;
			esl.call(g_DSHarpoon.sip_collision);
			return 0;
		}


		VEC tvec(g_DSHarpoon.nextcp.x-g_DSHarpoon.pos.x,
				g_DSHarpoon.nextcp.y-g_DSHarpoon.pos.y,
				g_DSHarpoon.nextcp.z-g_DSHarpoon.pos.z);
		_Normalize(tvec);

		if(2 == g_DSHarpoon.stage)
		{
			g_DSHarpoon.dir.x = LERP(g_DSHarpoon.dir.x, tvec.x, 0.1);
			g_DSHarpoon.dir.y = LERP(g_DSHarpoon.dir.y, tvec.y, 0.1);
			g_DSHarpoon.dir.z = LERP(g_DSHarpoon.dir.z, tvec.z, 0.1);
		}
		else
		{
			g_DSHarpoon.dir.x = LERP(g_DSHarpoon.dir.x, tvec.x, 0.05);
			g_DSHarpoon.dir.y = LERP(g_DSHarpoon.dir.y, tvec.y, 0.05);
			g_DSHarpoon.dir.z = LERP(g_DSHarpoon.dir.z, tvec.z, 0.05);
		}
		_Normalize(g_DSHarpoon.dir);


		switch(g_DSHarpoon.stage)
		{
		case 0:
			// 발사 후 상승단계: 최고 도달 위치로 상승.
			if(SimFreq*3.0 < g_DSHarpoon.frame)
			{// 3초 후 nextcp는 진행방향의 바다 표면으로 전환
				g_DSHarpoon.nextcp.x += (g_DSHarpoon.dir.x * 200.0);
				g_DSHarpoon.nextcp.y += (g_DSHarpoon.dir.y * 200.0);
				g_DSHarpoon.nextcp.z = 20.0;
				g_DSHarpoon.stage++;
			}
			break;
		case 1:
			// 최고 고도에서 바다쪽으로 하강: 저점에 다다랐을 경우 nextcp는 목표물로 전환.
			if(g_DSHarpoon.dir.z < 0.0 && g_DSHarpoon.pos.z < 30.0)
			{
				g_DSHarpoon.nextcp.x = x2;
				g_DSHarpoon.nextcp.y = y2;
				g_DSHarpoon.nextcp.z = z2; // 타겟고도
				g_DSHarpoon.stage++;
			}
			break;
		case 2:
			// 최고 고도에서 바다쪽으로 하강: 저점에 다다랐을 경우 nextcp는 목표물로 전환.
			float slope = (float)Degree(atan2(g_DSHarpoon.dir.z, 1.0));
			if(slope > -2.0)
			{
				g_DSHarpoon.stage++; // boost stage
			}
			break;
		}


		double dstep = 0.0;
		
		if(g_DSHarpoon.stage < 3)
			dstep = (speed1 / (double)SimFreq);
		else
			dstep = (speed2 / (double)SimFreq);

		g_DSHarpoon.pos.x += g_DSHarpoon.dir.x * dstep;
		g_DSHarpoon.pos.y += g_DSHarpoon.dir.y * dstep;
		g_DSHarpoon.pos.z += g_DSHarpoon.dir.z * dstep;

		float hdg = (float)atan2(g_DSHarpoon.dir.x, g_DSHarpoon.dir.y);
		float pitch = (float)atan2(g_DSHarpoon.dir.z, dstep);

		g_DSHarpoon.ori.h = (float)Degree(hdg);
		g_DSHarpoon.ori.p = (float)Degree(pitch);
		g_DSHarpoon.ori.r = (float)Degree(0.0);

		//DebugOutput("Harpoon = %f, %f, %f | %f, %f, %f   dist = %f\n", 
		//	g_DSHarpoon.pos.x, g_DSHarpoon.pos.y, g_DSHarpoon.pos.z,
		//	g_DSHarpoon.ori.h, g_DSHarpoon.ori.p, g_DSHarpoon.ori.r,
		//	dist);
	}


	// Harpoon position update to entity list
	int iEnt = nonexposed_GetEntity(name);
	ENTITY& ent = g_entity_list[iEnt];
	ent.pos.x = g_DSHarpoon.pos.x;
	ent.pos.y = g_DSHarpoon.pos.y;
	ent.pos.z = g_DSHarpoon.pos.z;
	ent.ori.h = g_DSHarpoon.ori.h;
	ent.ori.p = g_DSHarpoon.ori.p;
	ent.ori.r = g_DSHarpoon.ori.r;
	ent.dir.x = g_DSHarpoon.dir.x;
	ent.dir.y = g_DSHarpoon.dir.y;
	ent.dir.z = g_DSHarpoon.dir.z;
	ent.live  = g_DSHarpoon.live;

	g_DSHarpoon.frame++;

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_UpdateDynamics_Torpedo(char name[256], int tidx, char ref[256],
			double x1, double y1, double z1,
			double x2, double y2, double z2,
			double bx, double by, double bz,
			double speed, char contact[256])
{
	int iEnt = nonexposed_GetEntity(name);
	int iTor = tidx;

	if(0 == g_DSTorpedo[iTor].live)
		return 0;

	if(0 == g_DSTorpedo[iTor].frame)
	{// init
		g_DSTorpedo[iTor].sip_collision = esl.getfunc(contact);

		VEC vec(x2 - x1, y2 - y1, z2 - z1);
		_Normalize(vec);

		int iRef = nonexposed_GetEntity(ref);
		if(-1 == iRef)
			return -1;

		D3DXVECTOR3 vup(0, 0, 1);
		D3DXVECTOR3 vdir((float)g_entity_list[iRef].dir.x, 
							(float)g_entity_list[iRef].dir.y, 
							(float)g_entity_list[iRef].dir.z);
		D3DXVec3Normalize(&vdir, &vdir);
		D3DXVECTOR3 vright;
		D3DXVec3Cross(&vright, &vdir, &vup);
		D3DXVec3Cross(&vup, &vright, &vdir);

		g_DSTorpedo[iTor].nextcp.x = x1;
		g_DSTorpedo[iTor].nextcp.y = y1;
		g_DSTorpedo[iTor].nextcp.z = z1;
		g_DSTorpedo[iTor].nextcp.x += bx * vright.x;
		g_DSTorpedo[iTor].nextcp.y += bx * vright.y;
		g_DSTorpedo[iTor].nextcp.z += bx * vright.z;
		g_DSTorpedo[iTor].nextcp.x += by * vdir.x;
		g_DSTorpedo[iTor].nextcp.y += by * vdir.y;
		g_DSTorpedo[iTor].nextcp.z += by * vdir.z;
		g_DSTorpedo[iTor].nextcp.x += bz * vup.x;
		g_DSTorpedo[iTor].nextcp.y += bz * vup.y;
		g_DSTorpedo[iTor].nextcp.z += bz * vup.z;

		g_DSTorpedo[iTor].pos.x = g_DSTorpedo[iTor].nextcp.x;
		g_DSTorpedo[iTor].pos.y = g_DSTorpedo[iTor].nextcp.y;
		g_DSTorpedo[iTor].pos.z = g_DSTorpedo[iTor].nextcp.z;

		// 잠수함 앞으로 발사
		g_DSTorpedo[iTor].nextcp.x += vdir.x * 200.0;
		g_DSTorpedo[iTor].nextcp.y += vdir.y * 200.0;
		g_DSTorpedo[iTor].nextcp.z += vdir.z * 200.0;

		g_DSTorpedo[iTor].dir.x = vdir.x;
		g_DSTorpedo[iTor].dir.y = vdir.y;
		g_DSTorpedo[iTor].dir.z = vdir.z;

		g_DSTorpedo[iTor].vel.x = vdir.x * 10.0;
		g_DSTorpedo[iTor].vel.y = vdir.y * 10.0;
		g_DSTorpedo[iTor].vel.z = vdir.z * 10.0;

		g_DSTorpedo[iTor].stage = 0;
	}
	else
	{// update
		// update dir
		VEC v(g_DSTorpedo[iTor].pos.x-x2, g_DSTorpedo[iTor].pos.y-y2, g_DSTorpedo[iTor].pos.z-z2);
		double dist = _CalcLength(v);

		if(dist < 10.0)
		{// collision
			g_DSTorpedo[iTor].live = 0;
			g_DSTorpedo[iTor].boom = 1;
			esl.call(g_DSTorpedo[iTor].sip_collision);
			goto next;
		}


		VEC tvec(g_DSTorpedo[iTor].nextcp.x - g_DSTorpedo[iTor].pos.x,
				g_DSTorpedo[iTor].nextcp.y - g_DSTorpedo[iTor].pos.y,
				g_DSTorpedo[iTor].nextcp.z - g_DSTorpedo[iTor].pos.z);
		_Normalize(tvec);

		g_DSTorpedo[iTor].dir.x = LERP(g_DSTorpedo[iTor].dir.x, tvec.x, 0.1);
		g_DSTorpedo[iTor].dir.y = LERP(g_DSTorpedo[iTor].dir.y, tvec.y, 0.1);
		g_DSTorpedo[iTor].dir.z = LERP(g_DSTorpedo[iTor].dir.z, tvec.z, 0.1);
		_Normalize(g_DSTorpedo[iTor].dir);


		switch(g_DSTorpedo[iTor].stage)
		{
		case 0:
			if(SimFreq*3.0 < g_DSTorpedo[iTor].frame)
			{// 수심 z1미터까지 상승
				g_DSTorpedo[iTor].nextcp.x += g_DSTorpedo[iTor].dir.x * 20.0;
				g_DSTorpedo[iTor].nextcp.y += g_DSTorpedo[iTor].dir.y * 20.0;
				g_DSTorpedo[iTor].nextcp.z += g_DSTorpedo[iTor].dir.z * 20.0;
				g_DSTorpedo[iTor].stage++;
			}
			break;
		case 1:
			{// 타겟으로 비행
				g_DSTorpedo[iTor].nextcp.x = x2;
				g_DSTorpedo[iTor].nextcp.y = y2;
				//g_DSTorpedo[iTor].nextcp.z = z1;
				g_DSTorpedo[iTor].stage++;
			}
			break;
		case 2:
			{// Update target position
				g_DSTorpedo[iTor].nextcp.x = x2;
				g_DSTorpedo[iTor].nextcp.y = y2;
				//g_DSTorpedo[iTor].nextcp.z = z1;

				double target[3] = {x2, y2, z2};
				double d = HaanUtils::CCatmullRomInterp::CalcDistance((double*)&g_DSTorpedo[iTor].pos, target);											
				if(d < 100.0)
				{
					g_DSTorpedo[iTor].stage++;
				}
			}
			break;
		}
		

		if(3 == g_DSTorpedo[iTor].stage)
			g_DSTorpedo[iTor].nextcp.z = z2;

		double dstep = 0.0;

		double speedsec = speed * 1852.0 / 3600.0;
		
		if(0 == g_DSTorpedo[iTor].stage)
			dstep = (20.0 / (double)SimFreq);
		else
			dstep = (speedsec / (double)SimFreq);


		g_DSTorpedo[iTor].vel.x = g_DSTorpedo[iTor].dir.x * speedsec;
		g_DSTorpedo[iTor].vel.y = g_DSTorpedo[iTor].dir.y * speedsec;
		g_DSTorpedo[iTor].vel.z = g_DSTorpedo[iTor].dir.z * speedsec;

		g_DSTorpedo[iTor].pos.x += g_DSTorpedo[iTor].dir.x * dstep;
		g_DSTorpedo[iTor].pos.y += g_DSTorpedo[iTor].dir.y * dstep;
		g_DSTorpedo[iTor].pos.z += g_DSTorpedo[iTor].dir.z * dstep;

		float hdg = (float)atan2(g_DSTorpedo[iTor].dir.x, g_DSTorpedo[iTor].dir.y);
		float pitch = (float)atan2(g_DSTorpedo[iTor].dir.z, dstep);

		g_DSTorpedo[iTor].ori.h = (float)Degree(hdg);
		g_DSTorpedo[iTor].ori.p = (float)Degree(pitch);
		g_DSTorpedo[iTor].ori.r = (float)Degree(0.0);

		//DebugOutput("Torpedo[%d] = %f, %f, %f | %f, %f, %f   dist = %f\n", 
		//	iTor,
		//	g_DSTorpedo[iTor].pos.x, g_DSTorpedo[iTor].pos.y, g_DSTorpedo[iTor].pos.z,
		//	g_DSTorpedo[iTor].ori.h, g_DSTorpedo[iTor].ori.p, g_DSTorpedo[iTor].ori.r,
		//	dist);
	}


next:;

	// Torpedo position update into the entity list
	ENTITY& ent = g_entity_list[iEnt];
	ent.pos.x = g_DSTorpedo[iTor].pos.x;
	ent.pos.y = g_DSTorpedo[iTor].pos.y;
	ent.pos.z = g_DSTorpedo[iTor].pos.z;
	ent.ori.h = g_DSTorpedo[iTor].ori.h;
	ent.ori.p = g_DSTorpedo[iTor].ori.p;
	ent.ori.r = g_DSTorpedo[iTor].ori.r;
	ent.dir.x = g_DSTorpedo[iTor].dir.x;
	ent.dir.y = g_DSTorpedo[iTor].dir.y;
	ent.dir.z = g_DSTorpedo[iTor].dir.z;
	ent.live  = g_DSTorpedo[iTor].live;

	g_DSTorpedo[iTor].frame++;

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_UpdateDynamics_Decoy(char name[256], char ref[256], int iDecoy, double throwdist,
											double tx, double ty, double tz)
{
	int iRef = nonexposed_GetEntity(ref);
	if(-1 == iRef)
		return 0;

	int iEnt = nonexposed_GetEntity(name);
	if(-1 == iEnt)
		return 0;

	if(0 == g_DSDecoy[iDecoy].live)
		return 0;

	if(0 == g_DSDecoy[iDecoy].frame)
	{// init
		D3DXVECTOR3 vup(0, 0, 1);
		D3DXVECTOR3 vdir((float)g_entity_list[iRef].dir.x, 
							(float)g_entity_list[iRef].dir.y, 
							(float)g_entity_list[iRef].dir.z);
		D3DXVec3Normalize(&vdir, &vdir);
		D3DXVECTOR3 vright;
		D3DXVec3Cross(&vright, &vdir, &vup);
		D3DXVec3Cross(&vup, &vright, &vdir);

		// 잠수함 옆으로 발사
		g_DSDecoy[iDecoy].dir.x = vright.x;
		g_DSDecoy[iDecoy].dir.y = vright.y;
		g_DSDecoy[iDecoy].dir.z = vright.z;

		g_DSDecoy[iDecoy].pos.x = g_entity_list[iRef].pos.x;
		g_DSDecoy[iDecoy].pos.y = g_entity_list[iRef].pos.y;
		g_DSDecoy[iDecoy].pos.z = g_entity_list[iRef].pos.z;

		g_DSDecoy[iDecoy].nextcp.x = g_DSDecoy[iDecoy].pos.x;
		g_DSDecoy[iDecoy].nextcp.y = g_DSDecoy[iDecoy].pos.y;
		g_DSDecoy[iDecoy].nextcp.z = g_DSDecoy[iDecoy].pos.z;
		g_DSDecoy[iDecoy].nextcp.x += throwdist * vright.x;
		g_DSDecoy[iDecoy].nextcp.y += throwdist * vright.y;
		g_DSDecoy[iDecoy].nextcp.z += throwdist * vright.z;

		g_DSDecoy[iDecoy].stage = 0;
	}
	else
	{// update
		VEC v(g_DSDecoy[iDecoy].pos.x-tx, g_DSDecoy[iDecoy].pos.y-ty, g_DSDecoy[iDecoy].pos.z-tz);
		double dist = _CalcLength(v);

		if(dist < 10.0)
		{// collision
			g_DSDecoy[iDecoy].live = 0;
			g_DSDecoy[iDecoy].boom = 1;
			esl.call(g_DSDecoy[iDecoy].sip_collision);
			goto next;
		}

		switch(g_DSDecoy[iDecoy].stage)
		{
		case 0:
			if(SimFreq*3.0 < g_DSDecoy[iDecoy].frame)
			{// 지랄회전역학!
				g_DSDecoy[iDecoy].stage++;
			}
			break;
		}


		if(1 == g_DSDecoy[iDecoy].stage)
			g_DSDecoy[iDecoy].ori.p += Radian(25.0f);

		float hpr[3] = {g_DSDecoy[iDecoy].ori.h, g_DSDecoy[iDecoy].ori.p, g_DSDecoy[iDecoy].ori.r};
		float matRot[16] = {0,};
		EulerToRotationMatrix(hpr, matRot);

		g_DSDecoy[iDecoy].dir.x = matRot[4];
		g_DSDecoy[iDecoy].dir.y = matRot[5];
		g_DSDecoy[iDecoy].dir.z = matRot[6];

		double dstep = (15.0 / (double)SimFreq);
		g_DSDecoy[iDecoy].pos.x += g_DSDecoy[iDecoy].dir.x * dstep;
		g_DSDecoy[iDecoy].pos.y += g_DSDecoy[iDecoy].dir.y * dstep;
		g_DSDecoy[iDecoy].pos.z += g_DSDecoy[iDecoy].dir.z * dstep;

		//DebugOutput("Decoy[%d] = %f, %f, %f | %f, %f, %f   dist = %f\n", 
		//	iTor,
		//	g_DSTorpedo[iTor].pos.x, g_DSTorpedo[iTor].pos.y, g_DSTorpedo[iTor].pos.z,
		//	g_DSTorpedo[iTor].ori.h, g_DSTorpedo[iTor].ori.p, g_DSTorpedo[iTor].ori.r,
		//	dist);
	}

next:;

	// Decoy position update into the entity list
	ENTITY& ent = g_entity_list[iEnt];
	ent.pos.x = g_DSDecoy[iDecoy].pos.x;
	ent.pos.y = g_DSDecoy[iDecoy].pos.y;
	ent.pos.z = g_DSDecoy[iDecoy].pos.z;
	ent.ori.h = g_DSDecoy[iDecoy].ori.h;
	ent.ori.p = g_DSDecoy[iDecoy].ori.p;
	ent.ori.r = g_DSDecoy[iDecoy].ori.r;
	ent.dir.x = g_DSDecoy[iDecoy].dir.x;
	ent.dir.y = g_DSDecoy[iDecoy].dir.y;
	ent.dir.z = g_DSDecoy[iDecoy].dir.z;
	ent.live  = g_DSDecoy[iDecoy].live;

	g_DSDecoy[iDecoy].frame++;

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_BuildDynamicEntity(char name[256])
{
	if(_stricmp(name, "Harpoon") == 0)
	{
		if(g_DSHarpoon.live)
		{
			// Rocket
			svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
			svHXCONTROL_MOVEENTITY.Length = sizeof(Hx::HXCONTROL_MOVEENTITY);
			svHXCONTROL_MOVEENTITY.iEntity = 300;
			svHXCONTROL_MOVEENTITY.Flags = g_DSHarpoon.live ? 0x78: 0x7A; 
			svHXCONTROL_MOVEENTITY.iModel = g_DSHarpoon.live ? 400:0;
			svHXCONTROL_MOVEENTITY.iParentEntity = 0;
			svHXCONTROL_MOVEENTITY.TimeStamp = SimTimeStamp;
			svHXCONTROL_MOVEENTITY.Pos[0] = g_DSHarpoon.pos.x;
			svHXCONTROL_MOVEENTITY.Pos[1] = g_DSHarpoon.pos.y;
			svHXCONTROL_MOVEENTITY.Pos[2] = g_DSHarpoon.pos.z;

			float hpr[3] = {Radian(g_DSHarpoon.ori.h), Radian(g_DSHarpoon.ori.p), Radian(g_DSHarpoon.ori.r)};
			EulerToRotationMatrix(hpr, svHXCONTROL_MOVEENTITY.matRot);

			AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));



			// Rocket Trail
			if(g_DSHarpoon.pos.z > 0.0)
			{
				svHXCONTROL_ROCKETTRAIL.Code = HXCODE_ROCKETTRAIL;
				svHXCONTROL_ROCKETTRAIL.Length = sizeof(Hx::HXCONTROL_ROCKETTRAIL);
				svHXCONTROL_ROCKETTRAIL.iEntity = 301;
				if(0 == g_DSHarpoon.stage)
				{
					svHXCONTROL_ROCKETTRAIL.Speed = -200.0f;
					svHXCONTROL_ROCKETTRAIL.ParticleRadius = 0.5f;
					svHXCONTROL_ROCKETTRAIL.Color[0] = 20.0f;
					svHXCONTROL_ROCKETTRAIL.Color[1] = 20.0f;
					svHXCONTROL_ROCKETTRAIL.Color[2] = 16.0f;
					svHXCONTROL_ROCKETTRAIL.Color[3] = 1.0f;
				}
				else
				{
					svHXCONTROL_ROCKETTRAIL.Speed = -300.0f;
					svHXCONTROL_ROCKETTRAIL.ParticleRadius = 1.0f;
					svHXCONTROL_ROCKETTRAIL.Color[0] = 20.0f;
					svHXCONTROL_ROCKETTRAIL.Color[1] = 20.0f;
					svHXCONTROL_ROCKETTRAIL.Color[2] = 16.0f;
					svHXCONTROL_ROCKETTRAIL.Color[3] = 0.01f;
				}

				AddPacket(&svHXCONTROL_ROCKETTRAIL, sizeof(svHXCONTROL_ROCKETTRAIL));
			}
		}
		else if(1 == g_DSHarpoon.boom)
		{// dead!
			// Rocket
			svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
			svHXCONTROL_MOVEENTITY.Length = sizeof(Hx::HXCONTROL_MOVEENTITY);
			svHXCONTROL_MOVEENTITY.iEntity = 300;
			svHXCONTROL_MOVEENTITY.Flags = 0x7A; 
			svHXCONTROL_MOVEENTITY.iModel = 0;
			svHXCONTROL_MOVEENTITY.iParentEntity = 0;
			svHXCONTROL_MOVEENTITY.TimeStamp = SimTimeStamp;
			svHXCONTROL_MOVEENTITY.Pos[0] = -10000.0;
			svHXCONTROL_MOVEENTITY.Pos[1] = -10000.0;
			svHXCONTROL_MOVEENTITY.Pos[2] = -10000.0;
			float hpr[3] = {Radian(g_DSHarpoon.ori.h), Radian(g_DSHarpoon.ori.p), Radian(g_DSHarpoon.ori.r)};
			EulerToRotationMatrix(hpr, svHXCONTROL_MOVEENTITY.matRot);
			AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));
		}
	}


	if(_stricmp(name, "Torpedo") == 0)
	{
		for(int k=0; k<NUMTOR; k++)
		{
			if(1 == g_DSTorpedo[k].live)
			{
				// Torpedo
				svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
				svHXCONTROL_MOVEENTITY.Length = sizeof(HXCONTROL_MOVEENTITY);
				svHXCONTROL_MOVEENTITY.iEntity = 5000 + k;
				svHXCONTROL_MOVEENTITY.Flags = 0x70;	// pos, rot, vel
				svHXCONTROL_MOVEENTITY.Pos[0] = g_DSTorpedo[k].pos.x;
				svHXCONTROL_MOVEENTITY.Pos[1] = g_DSTorpedo[k].pos.y;
				svHXCONTROL_MOVEENTITY.Pos[2] = g_DSTorpedo[k].pos.z;
				svHXCONTROL_MOVEENTITY.Velo[0] = (float)g_DSTorpedo[k].vel.x;
				svHXCONTROL_MOVEENTITY.Velo[1] = (float)g_DSTorpedo[k].vel.y;
				svHXCONTROL_MOVEENTITY.Velo[2] = (float)g_DSTorpedo[k].vel.z;

				float hpr[3] = {Radian(g_DSTorpedo[k].ori.h), Radian(g_DSTorpedo[k].ori.p), 
										Radian(g_DSTorpedo[k].ori.r)};
				EulerToRotationMatrix(hpr, svHXCONTROL_MOVEENTITY.matRot);
				AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));


				svHXCONTROL_CAVITATIONBUBBLE.Code = HXCODE_CAVITATIONBUBBLE;
				svHXCONTROL_CAVITATIONBUBBLE.Length = sizeof(svHXCONTROL_CAVITATIONBUBBLE);
				svHXCONTROL_CAVITATIONBUBBLE.iEntity = 5100 + k;

				double tspeed = esl.get_f("g_TorpedoSpeed");
				if(tspeed > 50.0)
					svHXCONTROL_CAVITATIONBUBBLE.Speed = -5.0f;
				else
					svHXCONTROL_CAVITATIONBUBBLE.Speed = -5.0f;
				svHXCONTROL_CAVITATIONBUBBLE.Color[0] = 1.6f;
				svHXCONTROL_CAVITATIONBUBBLE.Color[1] = 1.6f;
				svHXCONTROL_CAVITATIONBUBBLE.Color[2] = 1.6f;
				svHXCONTROL_CAVITATIONBUBBLE.Color[3] = 0.25f;
				svHXCONTROL_CAVITATIONBUBBLE.ParticleRadius = 0.3f;
				AddPacket(&svHXCONTROL_CAVITATIONBUBBLE, sizeof(svHXCONTROL_CAVITATIONBUBBLE));


				if(0 == g_DSTorpedo[k].frame)
				{
					// 유도선 삭제
					svHXCONTROL_GUIDELINE.Code = HXCODE_GUIDELINE;
					svHXCONTROL_GUIDELINE.iEntityID = 5000 + k;
					svHXCONTROL_GUIDELINE.bVisible = FALSE;
					svHXCONTROL_GUIDELINE.Length = sizeof(Hx::HXCONTROL_GUIDELINE);
					AddPacket(&svHXCONTROL_GUIDELINE, sizeof(svHXCONTROL_GUIDELINE));
				}
				else
				{
					// 유도선
					svHXCONTROL_GUIDELINE.Code = HXCODE_GUIDELINE;
					svHXCONTROL_GUIDELINE.iEntityID = 5000 + k;
					svHXCONTROL_GUIDELINE.bVisible = TRUE;
					svHXCONTROL_GUIDELINE.Length = sizeof(Hx::HXCONTROL_GUIDELINE);
					svHXCONTROL_GUIDELINE.Pos[0] = g_DSTorpedo[k].pos.x;
					svHXCONTROL_GUIDELINE.Pos[1] = g_DSTorpedo[k].pos.y;
					svHXCONTROL_GUIDELINE.Pos[2] = g_DSTorpedo[k].pos.z;
					AddPacket(&svHXCONTROL_GUIDELINE, sizeof(svHXCONTROL_GUIDELINE));
				}
			}
			else if(1 == g_DSTorpedo[k].boom)
			{
				// Torpedo
				svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
				svHXCONTROL_MOVEENTITY.Length = sizeof(HXCONTROL_MOVEENTITY);
				svHXCONTROL_MOVEENTITY.iEntity = 5000 + k;
				svHXCONTROL_MOVEENTITY.Flags = 0x52;
				svHXCONTROL_MOVEENTITY.iModel = 0;
				svHXCONTROL_MOVEENTITY.Pos[0] = -10000.0;
				svHXCONTROL_MOVEENTITY.Pos[1] = -10000.0;
				svHXCONTROL_MOVEENTITY.Pos[2] = -10000.0;
				float hpr[3] = {Radian(g_DSTorpedo[k].ori.h), Radian(g_DSTorpedo[k].ori.p), 
										Radian(g_DSTorpedo[k].ori.r)};
				EulerToRotationMatrix(hpr, svHXCONTROL_MOVEENTITY.matRot);
				AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));

				// 유도선 삭제
				svHXCONTROL_GUIDELINE.Code = HXCODE_GUIDELINE;
				svHXCONTROL_GUIDELINE.iEntityID = 5000 + k;
				svHXCONTROL_GUIDELINE.bVisible = FALSE;
				svHXCONTROL_GUIDELINE.Length = sizeof(Hx::HXCONTROL_GUIDELINE);
//				svHXCONTROL_GUIDELINE.Pos[0] = -10000.0;
//				svHXCONTROL_GUIDELINE.Pos[1] = -10000.0;
//				svHXCONTROL_GUIDELINE.Pos[2] = -10000.0;
				AddPacket(&svHXCONTROL_GUIDELINE, sizeof(svHXCONTROL_GUIDELINE));
			}
		}
	}


	if(_stricmp(name, "Decoy") == 0)
	{
		for(int k=0; k<NUMDECOY; k++)
		{
			if(1 == g_DSDecoy[k].live)
			{		
				// Decoy
				svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
				svHXCONTROL_MOVEENTITY.Length = sizeof(HXCONTROL_MOVEENTITY);
				svHXCONTROL_MOVEENTITY.iEntity = 6000 + k;
				svHXCONTROL_MOVEENTITY.Flags = 0x50;	// rot, pos
				svHXCONTROL_MOVEENTITY.Pos[0] = g_DSDecoy[k].pos.x;
				svHXCONTROL_MOVEENTITY.Pos[1] = g_DSDecoy[k].pos.y;
				svHXCONTROL_MOVEENTITY.Pos[2] = g_DSDecoy[k].pos.z;

				float hpr[3] = {g_DSDecoy[k].ori.h, g_DSDecoy[k].ori.p, g_DSDecoy[k].ori.r};
				EulerToRotationMatrix(hpr, svHXCONTROL_MOVEENTITY.matRot);
				AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));

				svHXCONTROL_CAVITATIONBUBBLE.Code = HXCODE_CAVITATIONBUBBLE;
				svHXCONTROL_CAVITATIONBUBBLE.Length = sizeof(svHXCONTROL_CAVITATIONBUBBLE);
				svHXCONTROL_CAVITATIONBUBBLE.iEntity = 6100 + k;
				svHXCONTROL_CAVITATIONBUBBLE.Speed = -5.0f;
				svHXCONTROL_CAVITATIONBUBBLE.Color[0] = 1.6f;
				svHXCONTROL_CAVITATIONBUBBLE.Color[1] = 1.6f;
				svHXCONTROL_CAVITATIONBUBBLE.Color[2] = 1.6f;
				svHXCONTROL_CAVITATIONBUBBLE.Color[3] = 0.45f;
				svHXCONTROL_CAVITATIONBUBBLE.ParticleRadius = 1.0f;
				AddPacket(&svHXCONTROL_CAVITATIONBUBBLE, sizeof(svHXCONTROL_CAVITATIONBUBBLE));
			}
			else if(1 == g_DSDecoy[k].boom)
			{
				svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
				svHXCONTROL_MOVEENTITY.Length = sizeof(HXCONTROL_MOVEENTITY);
				svHXCONTROL_MOVEENTITY.iEntity = 6000 + k;
				svHXCONTROL_MOVEENTITY.Flags = 0x52;
				svHXCONTROL_MOVEENTITY.iModel = 0;
				svHXCONTROL_MOVEENTITY.Pos[0] = -10000.0;
				svHXCONTROL_MOVEENTITY.Pos[1] = -10000.0;
				svHXCONTROL_MOVEENTITY.Pos[2] = -10000.0;
				AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));
			}
		}
	}

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_ResetPath(int iPath)
{
	g_path_list[iPath].clear();
	g_path_build_list[iPath].clear();
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_AddPathNode(int iPath, 
									double x, double y, double z,
									double h, double p, double r)
{
	PATHNODE node;
	node.pos.x = x;
	node.pos.y = y;
	node.pos.z = z;
	node.ori.h = (float)h;
	node.ori.p = (float)p;
	node.ori.r = (float)r;
	g_path_list[iPath].push_back(node);
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int nonexposed_GetEntity(char name[256])
{
	int n = g_entity_list.size();
	for(int k=0; k<n; k++)
	{
		ENTITY& ent = g_entity_list[k];
		if(ent.valid && _stricmp(name, ent.name) == 0)
		{
			return k;
		}
	}

	return -1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_UseEntity(char name[256], int validate, double cdrange, char cdcb[256], int cdrepeat)
{
	ENTITY ent;
	strcpy_s(ent.name, 256, name);
	ent.usepath = -1;
	ent.tickcur = 0;
	ent.ticklast = -1;
	ent.sip_init = -1;
	ent.sip_during = -1;
	ent.sip_finish = -1;
	ent.valid = validate;
	ent.live = 1;
	ent.cdrange = cdrange;
	ent.cdrepeat = cdrepeat;
	ent.sip_cdcb = esl.getfunc(cdcb);
	g_entity_list.push_back(ent);
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetEntityLiveState(char name[256], int state)
{
	int k = nonexposed_GetEntity(name);
	g_entity_list[k].live = state;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_ValidateEntity(char name[256])
{
	int iEnt = nonexposed_GetEntity(name);
	if(-1 == iEnt)
		return -1;
	g_entity_list[iEnt].valid = 1;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_InvalidateEntity(char name[256])
{
	int iEnt = nonexposed_GetEntity(name);
	if(-1 == iEnt)
		return -1;
	g_entity_list[iEnt].valid = 0;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_DriveObjectByPath(char name[256], int iPath, int duration, int bGenFwdDir, 
	char cb_init[256], char cb_during[256], char cb_finish[256])
{
	int iEnt = nonexposed_GetEntity(name);
	if(-1 == iEnt)
		return -1;

	HaanUtils::CCatmullRomInterp cri;
	cri.GenerateMatrix(0.5); // curve tension

	for(unsigned int k=0; k<g_path_list[iPath].size(); k++)
	{
		PATHNODE& n = g_path_list[iPath][k];
		HaanUtils::SIMDATA s;
		s.Pos[0] = n.pos.x;
		s.Pos[1] = n.pos.y;
		s.Pos[2] = n.pos.z;
		s.Ori[0] = n.ori.h;
		s.Ori[1] = n.ori.p;
		s.Ori[2] = n.ori.r;

		cri.AddControlPoint(s);
	}

	int nTicks = duration*SimFreq;
	cri.GetInterpUniformPointArray(g_path_build_list[iPath], nTicks);
	assert(g_path_build_list[iPath].size());


	if(bGenFwdDir)
	{
		int nTPts = g_path_build_list[iPath].size();

		for(int q=0; q<nTPts-1; q++)
		{
			HaanUtils::SIMDATA& v = g_path_build_list[iPath][q];
			HaanUtils::SIMDATA& vn = g_path_build_list[iPath][q+1];

			VEC dir(vn.Pos[0] - v.Pos[0], vn.Pos[1] - v.Pos[1], vn.Pos[2] - v.Pos[2]);
			_Normalize(dir);
			double forward_diff = _CalcLength(dir); // 항상 1이 될터인데~ 그냥 1해도 될것. 나중에 바꿔야지.

			float hdg = (float)atan2(dir.x, dir.y);
			float pitch = (float)atan2(dir.z, forward_diff);
			//float pitch = (float)atan2(dir.z, dir.y);

			g_path_build_list[iPath][q].Ori[0] = (float)Degree(hdg);
			g_path_build_list[iPath][q].Ori[1] = (float)Degree(pitch);
			g_path_build_list[iPath][q].Ori[2] = (float)Degree(0.0);

			_Normalize(dir);
			g_path_build_list[iPath][q].Dir[0] = (float)dir.x;
			g_path_build_list[iPath][q].Dir[1] = (float)dir.y;
			g_path_build_list[iPath][q].Dir[2] = (float)dir.z;

			//float roll = (hdg-hdg_last) * 50.0f;

			//if(roll > RADIAN(15.0f))
			//	roll = RADIAN(15.0f);
			//else if(roll < RADIAN(-15.0f))
			//	roll = RADIAN(-15.0f);

			// Speed
			//float fDistance = sqrt((dir[0] * dir[0]) + (dir[1] * dir[1]) + (dir[2] * dir[2]));
			//float fSpeed  = fDistance / (1.0f/3.0f) * 3600;
			//data.fSpeedPfd = METERTONM(fSpeed);
		}

		memcpy(&g_path_build_list[iPath][nTPts-1], &g_path_build_list[iPath][nTPts-2], sizeof(HaanUtils::SIMDATA));
	}
	else
	{
		int nTPts = g_path_build_list[iPath].size();

		for(int q=0; q<nTPts; q++)
		{
			HaanUtils::SIMDATA& v = g_path_build_list[iPath][q];
			g_path_build_list[iPath][q].Ori[0] = v.Ori[0];
			g_path_build_list[iPath][q].Ori[1] = v.Ori[1];
			g_path_build_list[iPath][q].Ori[2] = v.Ori[2];

			float hpr[3] = {Radian(v.Ori[0]), Radian(v.Ori[1]), Radian(v.Ori[2])};
			float matRot[16] = {0,};
			EulerToRotationMatrix(hpr, matRot);

			g_path_build_list[iPath][q].Dir[0] = matRot[4];
			g_path_build_list[iPath][q].Dir[1] = matRot[5];
			g_path_build_list[iPath][q].Dir[2] = matRot[6];
		}
	}


	// Update Velocity
	{
		int nTPts = g_path_build_list[iPath].size();

		for(int q=0; q<nTPts-1; q++)
		{
			HaanUtils::SIMDATA& v = g_path_build_list[iPath][q];
			HaanUtils::SIMDATA& vn = g_path_build_list[iPath][q+1];

			VEC dir(vn.Pos[0] - v.Pos[0], vn.Pos[1] - v.Pos[1], vn.Pos[2] - v.Pos[2]);
			double forward_diff = _CalcLength(dir);

			float velo[3] = {0,};
			velo[0] = (float)(vn.Pos[0] - v.Pos[0]) * (float)SimFreq;
			velo[1] = (float)(vn.Pos[1] - v.Pos[1]) * (float)SimFreq;
			velo[2] = (float)(vn.Pos[2] - v.Pos[2]) * (float)SimFreq;

			v.Vel[0] = velo[0];
			v.Vel[1] = velo[1];
			v.Vel[2] = velo[2];
		}

		g_path_build_list[iPath][nTPts-1].Vel[0] = g_path_build_list[iPath][nTPts-2].Vel[0];
		g_path_build_list[iPath][nTPts-1].Vel[1] = g_path_build_list[iPath][nTPts-2].Vel[1];
		g_path_build_list[iPath][nTPts-1].Vel[2] = g_path_build_list[iPath][nTPts-2].Vel[2];
	}


	ENTITY& ent = g_entity_list[iEnt];
	strcpy_s(ent.name, 256, name);
	ent.usepath = iPath;
	ent.tickcur = 0;
	ent.ticklast = SimTick + nTicks;
	ent.sip_init = esl.getfunc(cb_init);
	ent.sip_during = esl.getfunc(cb_during);
	ent.sip_finish = esl.getfunc(cb_finish);
	ent.valid = 1;

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetWaterColor(double r, double g, double b)
{
    svHXCONTROL_ATMOSPHERE.WaterColor[0] = (float)r;
    svHXCONTROL_ATMOSPHERE.WaterColor[1] = (float)g;
    svHXCONTROL_ATMOSPHERE.WaterColor[2] = (float)b;
	AddPacket(&svHXCONTROL_ATMOSPHERE, sizeof(svHXCONTROL_ATMOSPHERE));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetWaterAttenuation(double atte)
{
	svHXCONTROL_ATMOSPHERE.WaterAttenuation = (float)atte;
	AddPacket(&svHXCONTROL_ATMOSPHERE, sizeof(svHXCONTROL_ATMOSPHERE));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetAirAttenuation(double atte)
{
	svHXCONTROL_ATMOSPHERE.AirAttenuation = (float)atte;
	AddPacket(&svHXCONTROL_ATMOSPHERE, sizeof(svHXCONTROL_ATMOSPHERE));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetFogAlt(double alt)
{
	svHXCONTROL_ATMOSPHERE.FogHigh = (float)alt;
	AddPacket(&svHXCONTROL_ATMOSPHERE, sizeof(svHXCONTROL_ATMOSPHERE));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetFogColor(double r, double g, double b)
{
	svHXCONTROL_ATMOSPHERE.FogColor[0] = (float)r;
	svHXCONTROL_ATMOSPHERE.FogColor[1] = (float)g;
	svHXCONTROL_ATMOSPHERE.FogColor[2] = (float)b;
	AddPacket(&svHXCONTROL_ATMOSPHERE, sizeof(svHXCONTROL_ATMOSPHERE));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetTOD(double tr)
{
	float euler[3] = {(float)Hx::Radian(100.0 + 50.0*tr), 
						(float)Hx::Radian(20.0 + 180.0*tr), 0.0f};
	float mat[16] = {0,};
	Hx::EulerToRotationMatrix(euler, mat);
	svHXCONTROL_ATMOSPHERE.SunDir[0] = mat[4];
	svHXCONTROL_ATMOSPHERE.SunDir[1] = mat[5];
	svHXCONTROL_ATMOSPHERE.SunDir[2] = mat[6];
	svHXCONTROL_ATMOSPHERE.SkyBright[0] = 10.0f - 10.0f* (float)sin(PI*0.5*tr);
	svHXCONTROL_ATMOSPHERE.SkyBright[1] = svHXCONTROL_ATMOSPHERE.SkyBright[0];
	svHXCONTROL_ATMOSPHERE.SkyBright[2] = svHXCONTROL_ATMOSPHERE.SkyBright[0];
	svHXCONTROL_ATMOSPHERE.SunLight[0] = 15.0f - 15.0f * (float)sin(PI*0.5*tr);
	svHXCONTROL_ATMOSPHERE.SunLight[1] = svHXCONTROL_ATMOSPHERE.SunLight[0];
	svHXCONTROL_ATMOSPHERE.SunLight[2] = svHXCONTROL_ATMOSPHERE.SunLight[0];
	svHXCONTROL_ATMOSPHERE.WaterColor[0] *= 1.0f - (float)sin(PI*0.5*tr);
	svHXCONTROL_ATMOSPHERE.WaterColor[1] *= 1.0f - (float)sin(PI*0.5*tr);
	svHXCONTROL_ATMOSPHERE.WaterColor[2] *= 1.0f - (float)sin(PI*0.5*tr);
	AddPacket(&svHXCONTROL_ATMOSPHERE, sizeof(svHXCONTROL_ATMOSPHERE));

	bindcall_SetFogColor(14.0 - 14.0*sin(PI*0.5*tr), 
						14.0 - 14.0*sin(PI*0.5*tr), 
						14.0 - 14.0*sin(PI*0.5*tr));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetCamera(int cam, int parent, double fov, double vnear, double vfar)
{
	svHXCONTROL_CAMERA.Code = HXCODE_CAMERA;
	svHXCONTROL_CAMERA.Length = sizeof(svHXCONTROL_CAMERA);
	svHXCONTROL_CAMERA.iCamera = cam;
	svHXCONTROL_CAMERA.iParent = parent;
	svHXCONTROL_CAMERA.vFov = (float)Radian(fov);
	svHXCONTROL_CAMERA.Near = (float)vnear;
	svHXCONTROL_CAMERA.Far = (float)vfar;
	AddPacket(&svHXCONTROL_CAMERA, sizeof(svHXCONTROL_CAMERA));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetOneWave(double winddir, double windspeed, double wcstrength)
{
	svHXCONTROL_ONEWAVE.Code = HXCODE_ONEWAVE;
	svHXCONTROL_ONEWAVE.Length = sizeof(svHXCONTROL_ONEWAVE);
	svHXCONTROL_ONEWAVE.WindDir = (float)Radian(winddir);
	svHXCONTROL_ONEWAVE.WindSpeed = (float)windspeed;
	svHXCONTROL_ONEWAVE.WhiteCapStrength = (float)wcstrength;
	AddPacket(&svHXCONTROL_ONEWAVE, sizeof(svHXCONTROL_ONEWAVE));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetScene(double exposure, double reflection, double mixture)
{
	svHXCONTROL_SCENECOMPOSITION.Code = HXCODE_SCENECOMPOSITION;
	svHXCONTROL_SCENECOMPOSITION.Length = sizeof(svHXCONTROL_SCENECOMPOSITION);
	svHXCONTROL_SCENECOMPOSITION.Exposure = (float)exposure;
	svHXCONTROL_SCENECOMPOSITION.WaterReflection = (float)reflection;
	svHXCONTROL_SCENECOMPOSITION.WaterMixture = (float)mixture;
	AddPacket(&svHXCONTROL_SCENECOMPOSITION, sizeof(svHXCONTROL_SCENECOMPOSITION));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetExposure(double exposure)
{
	svHXCONTROL_SCENECOMPOSITION.Code = HXCODE_SCENECOMPOSITION;
	svHXCONTROL_SCENECOMPOSITION.Length = sizeof(svHXCONTROL_SCENECOMPOSITION);
	svHXCONTROL_SCENECOMPOSITION.Exposure = (float)exposure;
	AddPacket(&svHXCONTROL_SCENECOMPOSITION, sizeof(svHXCONTROL_SCENECOMPOSITION));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetWaterReflection(double refl)
{
	svHXCONTROL_SCENECOMPOSITION.Code = HXCODE_SCENECOMPOSITION;
	svHXCONTROL_SCENECOMPOSITION.Length = sizeof(svHXCONTROL_SCENECOMPOSITION);
	svHXCONTROL_SCENECOMPOSITION.WaterReflection = (float)refl;
	AddPacket(&svHXCONTROL_SCENECOMPOSITION, sizeof(svHXCONTROL_SCENECOMPOSITION));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetSubmarineWaterCouplingRegion(char submarine[256], double width)
{
	if(0 == bindcall_QueryEntityInfo(submarine))
		return 0;

	double east = svEntityInfoSnapshot.pos.x + width;
	double west = svEntityInfoSnapshot.pos.x - width;
	double south = svEntityInfoSnapshot.pos.y - width;
	double north = svEntityInfoSnapshot.pos.y + width;

	svHXCONTROL_OBJECTWATERCOUPLING_REGION.Code = HXCODE_OBJECTWATERCOUPLING_REGION;
	svHXCONTROL_OBJECTWATERCOUPLING_REGION.Length = sizeof(svHXCONTROL_OBJECTWATERCOUPLING_REGION);
	svHXCONTROL_OBJECTWATERCOUPLING_REGION.BufferSize = 2048;
	svHXCONTROL_OBJECTWATERCOUPLING_REGION.East = east;
	svHXCONTROL_OBJECTWATERCOUPLING_REGION.West = west;
	svHXCONTROL_OBJECTWATERCOUPLING_REGION.South = south;
	svHXCONTROL_OBJECTWATERCOUPLING_REGION.North = north;
	AddPacket(&svHXCONTROL_OBJECTWATERCOUPLING_REGION, sizeof(svHXCONTROL_OBJECTWATERCOUPLING_REGION));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void nonexposed_ResetExplosion(int iTor)
{
	g_Explosions[iTor+1].tick = 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_DoExplosionByTorpedo()
{
	ENTITY_SCRIPT	front;
	ENTITY_SCRIPT	rear;

	if(0 == bindcall_QueryEntityInfo("403"))
		return 0;
	memcpy(&front, &svEntityInfoSnapshot, sizeof(ENTITY_SCRIPT));

	if(0 == bindcall_QueryEntityInfo("404"))
		return 0;
	memcpy(&rear, &svEntityInfoSnapshot, sizeof(ENTITY_SCRIPT));


	for(int iTor=0; iTor<NUMTOR; iTor++)
	{
		if(1 != g_DSTorpedo[iTor].boom)
			continue;

		unsigned int iExploEvent = g_Explosions[iTor+1].tick;

		double Pos[3] = {0,};
		Pos[0] = front.pos.x;
		Pos[1] = front.pos.y;
		Pos[2] = front.pos.z;
	
		float euler[3] = {0,};
		euler[0] = Radian(front.ori.h);
		euler[1] = Radian(front.ori.p);
		euler[2] = Radian(front.ori.r);


		unsigned int iFrontModel;
		unsigned int iRearModel;

		if(0 == iExploEvent)
		{
			Pos[2] = 0;
			euler[1] = 0;
			euler[2] = 0;
		
			front.pos.x = Pos[0];
			front.pos.y = Pos[1];
			front.pos.z = Pos[2];
			rear.pos.x = Pos[0];
			rear.pos.y = Pos[1];
			rear.pos.z = Pos[2];

			front.ori.h = Degree(euler[0]);
			front.ori.p = Degree(euler[1]);
			front.ori.r = Degree(euler[2]);
			rear.ori.h = Degree(euler[0]);
			rear.ori.p = Degree(euler[1]);
			rear.ori.r = Degree(euler[2]);

			iFrontModel = 200;
			iRearModel = 0;
		}
		else if(iExploEvent < 6000)
		{
			float t = 0.016666666f * (float)iExploEvent;
			float v = 8.0f;
			float t0 = 2.0f * v / 9.81f;
			if(t < t0)  
			{
				Pos[2] = v * t - 4.905f * t * t;
			}
			else  
			{
				Pos[2] = -0.2f * (t - t0);
			}


			front.pos.x = Pos[0];
			front.pos.y = Pos[1];
			front.pos.z = Pos[2];
			rear.pos.x = Pos[0];
			rear.pos.y = Pos[1];
			rear.pos.z = Pos[2];

			euler[1] = -0.02f * (float)Pos[2];
			euler[2] = -0.02f * t;

			front.ori.h = Degree(euler[0]);
			front.ori.p = Degree(euler[1]);
			front.ori.r = Degree(euler[2]);

			euler[1] *= -1;
			euler[2] *= -1;

			rear.ori.h = Degree(euler[0]);
			rear.ori.p = Degree(euler[1]);
			rear.ori.r = Degree(euler[2]);

			iFrontModel = 201;
			iRearModel = 202;
		}



		{// front
		svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
		svHXCONTROL_MOVEENTITY.Length = sizeof(HXCONTROL_MOVEENTITY);
		svHXCONTROL_MOVEENTITY.iEntity = 403;
		svHXCONTROL_MOVEENTITY.Flags = 0x5a;
		svHXCONTROL_MOVEENTITY.iModel = iFrontModel;
		svHXCONTROL_MOVEENTITY.TimeStamp = SimTimeStamp;
		memcpy(svHXCONTROL_MOVEENTITY.Pos, &front.pos, sizeof(VEC));

		float hpr[3] = {Radian(front.ori.h), Radian(front.ori.p), Radian(front.ori.r)};
		EulerToRotationMatrix(hpr, svHXCONTROL_MOVEENTITY.matRot);

		AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));
		}


		{// rear
		svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
		svHXCONTROL_MOVEENTITY.Length = sizeof(HXCONTROL_MOVEENTITY);
		svHXCONTROL_MOVEENTITY.iEntity = 404;
		svHXCONTROL_MOVEENTITY.Flags = 0x5a;
		svHXCONTROL_MOVEENTITY.iModel = iRearModel;
		svHXCONTROL_MOVEENTITY.TimeStamp = SimTimeStamp;
		memcpy(svHXCONTROL_MOVEENTITY.Pos, &rear.pos, sizeof(VEC));

		float hpr[3] = {Radian(rear.ori.h), Radian(rear.ori.p), Radian(rear.ori.r)};
		EulerToRotationMatrix(hpr, svHXCONTROL_MOVEENTITY.matRot);

		AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));
		}


		if(iExploEvent < 300)  
		{
			svHXCONTROL_TORPEDOHIT.Code = HXCODE_TORPEDOHIT;
			svHXCONTROL_TORPEDOHIT.Length = sizeof(HXCONTROL_TORPEDOHIT);

			memcpy(svHXCONTROL_TORPEDOHIT.Pos, &front.pos, sizeof(VEC));
			svHXCONTROL_TORPEDOHIT.TimeStamp = 16666 * iExploEvent;
			svHXCONTROL_TORPEDOHIT.Pos[2] = 0;
			AddPacket(&svHXCONTROL_TORPEDOHIT, sizeof(svHXCONTROL_TORPEDOHIT));
		}

		g_Explosions[iTor+1].tick++;
	}

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_DoExplosionByHarpoon()
{
	if(1 != g_DSHarpoon.boom)
		return 0;
	
	unsigned int iExploEvent = g_Explosions[0].tick;

	ENTITY_SCRIPT	front;
	ENTITY_SCRIPT	rear;

	if(0 == bindcall_QueryEntityInfo("401"))
		return 0;
	memcpy(&front, &svEntityInfoSnapshot, sizeof(ENTITY_SCRIPT));

	if(0 == bindcall_QueryEntityInfo("402"))
		return 0;
	memcpy(&rear, &svEntityInfoSnapshot, sizeof(ENTITY_SCRIPT));


	double Pos[3] = {0,};
	Pos[0] = front.pos.x;
	Pos[1] = front.pos.y;
	Pos[2] = front.pos.z;
	
	float euler[3] = {0,};
	euler[0] = Radian(front.ori.h);
	euler[1] = Radian(front.ori.p);
	euler[2] = Radian(front.ori.r);



	unsigned int iFrontModel;
	unsigned int iRearModel;

	if(0 == iExploEvent)
	{
		iFrontModel = 200;
		iRearModel = 0;
	}
	else  
	{
		iFrontModel = 201;
		iRearModel = 202;
	}



	{
		svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
		svHXCONTROL_MOVEENTITY.Length = sizeof(Hx::HXCONTROL_MOVEENTITY);
		svHXCONTROL_MOVEENTITY.iEntity = 401;
		svHXCONTROL_MOVEENTITY.Flags = 0x0a;
		svHXCONTROL_MOVEENTITY.iModel = iFrontModel;
		svHXCONTROL_MOVEENTITY.TimeStamp = SimTimeStamp;
		AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));
	}


	{
		svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
		svHXCONTROL_MOVEENTITY.Length = sizeof(Hx::HXCONTROL_MOVEENTITY);
		svHXCONTROL_MOVEENTITY.iEntity = 402;
		svHXCONTROL_MOVEENTITY.Flags = 0x0a;
		svHXCONTROL_MOVEENTITY.iModel = iRearModel;
		svHXCONTROL_MOVEENTITY.TimeStamp = SimTimeStamp;
		AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));
	}

	if(iExploEvent == 1 || iExploEvent == 8  || iExploEvent == 16 || iExploEvent == 24)  
	{
		svHXCONTROL_EXPLOSION.Code = HXCODE_EXPLOSION;
		svHXCONTROL_EXPLOSION.Length = sizeof(Hx::HXCONTROL_EXPLOSION);

		memcpy(svHXCONTROL_EXPLOSION.Pos, &front.pos, sizeof(VEC));

		svHXCONTROL_EXPLOSION.Pos[0] += 9 * (float(rand())/RAND_MAX - 0.5f);
		svHXCONTROL_EXPLOSION.Pos[1] += 9 * (float(rand())/RAND_MAX - 0.5f);
		svHXCONTROL_EXPLOSION.Pos[2] += 4;
		svHXCONTROL_EXPLOSION.TimeStamp = 16666 * iExploEvent;
		svHXCONTROL_EXPLOSION.nParticleParFrame = 10;
		svHXCONTROL_EXPLOSION.ExplosionRadius = 10;
		svHXCONTROL_EXPLOSION.ParticleRadius = 2.0f;
		svHXCONTROL_EXPLOSION.Color0[0] = 20;
		svHXCONTROL_EXPLOSION.Color0[1] = 15;
		svHXCONTROL_EXPLOSION.Color0[2] = 10;
		svHXCONTROL_EXPLOSION.Color0[3] = 4;
		svHXCONTROL_EXPLOSION.Color1[0] = 1;
		svHXCONTROL_EXPLOSION.Color1[1] = 1;
		svHXCONTROL_EXPLOSION.Color1[2] = 1;
		svHXCONTROL_EXPLOSION.Color1[3] = 0;
		AddPacket(&svHXCONTROL_EXPLOSION, sizeof(svHXCONTROL_EXPLOSION));
	}


	if(iExploEvent < 40)  
	{
		svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
		svHXCONTROL_MOVEENTITY.Length = sizeof(Hx::HXCONTROL_MOVEENTITY);
		svHXCONTROL_MOVEENTITY.iEntity = 10403;
		svHXCONTROL_MOVEENTITY.Flags = 0x15;
		svHXCONTROL_MOVEENTITY.iParentEntity = 403;
		svHXCONTROL_MOVEENTITY.iLight = 3;
		svHXCONTROL_MOVEENTITY.Pos[0] = 0;
		svHXCONTROL_MOVEENTITY.Pos[1] = 0;
		svHXCONTROL_MOVEENTITY.Pos[2] = 20;
		AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));
	}
	else  
	{
		svHXCONTROL_KILLENTITY.Code = HXCODE_KILLENTITY;
		svHXCONTROL_KILLENTITY.Length = sizeof(Hx::HXCONTROL_KILLENTITY);
		svHXCONTROL_KILLENTITY.iEntity = 10403;
		svHXCONTROL_KILLENTITY.Method = 1;
		AddPacket(&svHXCONTROL_KILLENTITY, sizeof(svHXCONTROL_KILLENTITY));
	}

	g_Explosions[0].tick++;

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_AutoExposure()
{
	int autoexp = esl.get_i("g_AutoExposure");
	if(0 == autoexp)
		return 0;

	BOOL bUnderTheSea = FALSE;

	int acam = esl.get_i("g_ActiveCamera");
	if(1 == acam)
	{
		if(g_entity_list[0].pos.z < 0.0)
			bUnderTheSea = TRUE;
	}
	else if(2 == acam)
	{
		int iPeri = nonexposed_GetEntity("111");
		int iSubm = nonexposed_GetEntity("100");
		double pz = g_entity_list[iPeri].pos.z + 12.0;
		double sz = g_entity_list[iSubm].pos.z;
		//DebugOutput("pz= %f, sz= %f\n", pz, sz);
		if(pz+sz < 0.0)
			bUnderTheSea = TRUE;
	}

	if(bUnderTheSea)
	{
		svHXCONTROL_SCENECOMPOSITION.Exposure = 
			(float)LERP(svHXCONTROL_SCENECOMPOSITION.Exposure, 0.13f, 0.05f);

		svHXCONTROL_SCENECOMPOSITION.WaterMixture = 
			(float)LERP(svHXCONTROL_SCENECOMPOSITION.WaterMixture, 0.95f, 0.05f);

		svHXCONTROL_SCENECOMPOSITION.WaterReflection = 
			(float)LERP(svHXCONTROL_SCENECOMPOSITION.WaterReflection, 0.1f, 0.05f);
	}
	else
	{
		svHXCONTROL_SCENECOMPOSITION.Exposure = 
			(float)LERP(svHXCONTROL_SCENECOMPOSITION.Exposure, 0.07f, 0.05f);

		svHXCONTROL_SCENECOMPOSITION.WaterMixture = 
			(float)LERP(svHXCONTROL_SCENECOMPOSITION.WaterMixture, 0.75f, 0.05f);

		svHXCONTROL_SCENECOMPOSITION.WaterReflection = 
			(float)LERP(svHXCONTROL_SCENECOMPOSITION.WaterReflection, 0.5f, 0.05f);
	}

	svHXCONTROL_SCENECOMPOSITION.Code = HXCODE_SCENECOMPOSITION;
	svHXCONTROL_SCENECOMPOSITION.Length = sizeof(svHXCONTROL_SCENECOMPOSITION);
	AddPacket(&svHXCONTROL_SCENECOMPOSITION, sizeof(svHXCONTROL_SCENECOMPOSITION));

	//if(bUnderTheSea)
		bindcall_SetWaterColor(0, 1, 2);
	//else
	//	bindcall_SetWaterColor(2, 4, 2);

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetCDTestPosition(double x, double y, double z)
{
	g_CDTesterPos.x = x;
	g_CDTesterPos.y = y;
	g_CDTesterPos.z = z;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_ResetPathDriving(char name[256])
{
	int iEnt = nonexposed_GetEntity(name);
	if(-1 == iEnt)
		return 0;

	ENTITY& ent = g_entity_list[iEnt];

	ent.live = 1;
	ent.tickcur = 0;

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetEntityModel(char name[256], char model[256])
{
	svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
	svHXCONTROL_MOVEENTITY.Length = sizeof(Hx::HXCONTROL_MOVEENTITY);
	svHXCONTROL_MOVEENTITY.iEntity = atoi(name);
	svHXCONTROL_MOVEENTITY.Flags = 0x2; // model change only
	svHXCONTROL_MOVEENTITY.iModel = atoi(model);
	AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_ResetGuidelines()
{
	for(int k=0; k<NUMTOR; k++)
	{
		svHXCONTROL_GUIDELINE.Code = HXCODE_GUIDELINE;
		svHXCONTROL_GUIDELINE.iEntityID = 5000 + k;
		svHXCONTROL_GUIDELINE.bVisible = FALSE;
		svHXCONTROL_GUIDELINE.Length = sizeof(Hx::HXCONTROL_GUIDELINE);
		svHXCONTROL_GUIDELINE.Pos[0] = -10000.0;
		svHXCONTROL_GUIDELINE.Pos[1] = -10000.0;
		svHXCONTROL_GUIDELINE.Pos[2] = -10000.0;
		AddPacket(&svHXCONTROL_GUIDELINE, sizeof(svHXCONTROL_GUIDELINE));
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetCloud(int count, int visible, int alt, int width, int height)
{
	svHXCONTROL_WEATHER.nCloudCount = count;
	svHXCONTROL_WEATHER.bCloudVisible = visible;
	svHXCONTROL_WEATHER.nCloudAltitude = alt;
	svHXCONTROL_WEATHER.nCloudWidth = width;
	svHXCONTROL_WEATHER.nCloudHeight = height;
	AddPacket(&svHXCONTROL_WEATHER, sizeof(svHXCONTROL_WEATHER));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetRain(int count, double width, double height)
{
	svHXCONTROL_WEATHER.nCountSnow = count;
	svHXCONTROL_WEATHER.bRain = TRUE;
	svHXCONTROL_WEATHER.fRainWidth = (float)width;
	svHXCONTROL_WEATHER.fRainHeight = (float)height;
	AddPacket(&svHXCONTROL_WEATHER, sizeof(svHXCONTROL_WEATHER));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetSnow(int count, double size)
{
	svHXCONTROL_WEATHER.nCountSnow = count;
	svHXCONTROL_WEATHER.bRain = FALSE;
	svHXCONTROL_WEATHER.fSnowSize = (float)size;
	AddPacket(&svHXCONTROL_WEATHER, sizeof(svHXCONTROL_WEATHER));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetUnderWaterExplosion(double px, double py, double pz, double progress)
{
	svHXCONTROL_UNDERWATEREXPLOSION.Code = HXCODE_UNDERWATEREXPLOSION;
	svHXCONTROL_UNDERWATEREXPLOSION.Length = sizeof(svHXCONTROL_UNDERWATEREXPLOSION);
	svHXCONTROL_UNDERWATEREXPLOSION.BaseColor[0] = 60.0f;
	svHXCONTROL_UNDERWATEREXPLOSION.BaseColor[1] = 40.0f;
	svHXCONTROL_UNDERWATEREXPLOSION.BaseColor[2] = 40.0f;
	svHXCONTROL_UNDERWATEREXPLOSION.Pos[0] = px;
	svHXCONTROL_UNDERWATEREXPLOSION.Pos[1] = py;
	svHXCONTROL_UNDERWATEREXPLOSION.Pos[2] = pz;
	svHXCONTROL_UNDERWATEREXPLOSION.Progress = (float)progress;
	svHXCONTROL_UNDERWATEREXPLOSION.Radius = 20.0f;
	AddPacket(&svHXCONTROL_UNDERWATEREXPLOSION, sizeof(svHXCONTROL_UNDERWATEREXPLOSION));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetUnderWaterImpurity(int bShow)
{
	svHXCONTROL_UNDERWATER_IMPURITY.Code = HXCODE_UNDERWATER_IMPURITY;
	svHXCONTROL_UNDERWATER_IMPURITY.Length = sizeof(svHXCONTROL_UNDERWATER_IMPURITY);
	svHXCONTROL_UNDERWATER_IMPURITY.bImpurity = bShow;
	AddPacket(&svHXCONTROL_UNDERWATER_IMPURITY, sizeof(svHXCONTROL_UNDERWATER_IMPURITY));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int _stdcall bindcall_SetSwell(float Dir, float Freq, float Height, float Length)
{
	svHXCONTROL_SWELL.Code = HXCODE_SWELL;
	svHXCONTROL_SWELL.Length = sizeof(svHXCONTROL_SWELL);
	svHXCONTROL_SWELL.fSwellDirection = Dir;
	svHXCONTROL_SWELL.fSwellFreq = Freq;
	svHXCONTROL_SWELL.fSwellHeight = Height;
	svHXCONTROL_SWELL.fSwellLength = Length;
	AddPacket(&svHXCONTROL_SWELL, sizeof(svHXCONTROL_SWELL));
	return 0;
}
