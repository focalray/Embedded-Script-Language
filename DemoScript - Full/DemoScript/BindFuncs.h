#pragma once


using namespace Hx;

#define	NUMTOR	10
#define NUMDECOY 2

#define LERP(A, B, T)	(A*(1.0-T) + B*T)


struct EVENT
{
	char name[256];
	int  repeat;
	int  period;
	int  triggered;
	int	 knocked;
	int	 sip;
	int  sip_init;
};

struct VEC
{
	VEC(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
	VEC() : x(0.0), y(0.0), z(0.0) {}
	double x;
	double y;
	double z;
};

struct ORI
{
	ORI() : h(0.0f), p(0.0f), r(0.0f) {}
	float h;
	float p;
	float r;
};

struct PATHNODE
{
	VEC	pos;
	ORI	ori;
};

struct ENTITY_SCRIPT
{
	VEC	pos;
	ORI	ori;
	VEC	dir;
	VEC vel;
};

struct ENTITY
{
	char name[256];
	int	usepath;
	int tickcur;
	int	ticklast;
	VEC	pos;
	ORI	ori;
	VEC	dir;
	VEC vel;
	int sip_during;
	int sip_init;
	int sip_finish;
	int valid;
	int	live;
	double cdrange;
	int	cdrepeat;
	int sip_cdcb;
};

struct DYNAMIC_STATE_MISSILE
{
	int		frame;
	VEC		pos;
	VEC		dir;
	ORI		ori;
	VEC		vel;
	VEC		nextcp;
	int		stage;
	int		live;
	int		trigger;
	int		sip_collision;
	int		boom;
};

struct EXPLOSION_STATE
{
	int		tick;
};

// SCRIPT EXPOSED FUNCTIONS(HOST-FUNCS)
int _stdcall bindcall_NewEvent(char name[256], int period, int repeat);
int _stdcall bindcall_NewFrameEvent(char name[256], int period, int repeat, char cb_init[256]);
int _stdcall bindcall_SetEntityPosOri(char name[256], 
							double x, double y, double z, double h, double p, double r);
int _stdcall bindcall_SetCameraPosOri(double x, double y, double z, double h, double p, double r);
int _stdcall bindcall_SetCameraPos(double x, double y, double z);
int _stdcall bindcall_SetCameraOri(double h, double p, double r);
int _stdcall bindcall_SetCameraFollowOffset(char name[256], 
										double dx, double dy, double dz, 
										double aoh, double aop, double aor);
int _stdcall bindcall_StopCameraFollow();
int _stdcall bindcall_QueryEntityInfo(char name[256]);
int _stdcall bindcall_ResetDynamics(char name[256]);
int _stdcall bindcall_RunDynamics(char name[256]);
int _stdcall bindcall_StopDynamics(char name[256]);
int _stdcall bindcall_UpdateDynamics_Harpoon(char name[256], char ref[256],
			double x1, double y1, double z1,
			double x2, double y2, double z2,
			double altmax,
			double speed1, double speed2, char contact[256]);
int _stdcall bindcall_UpdateDynamics_Torpedo(char name[256], int tidx, char ref[256],
			double x1, double y1, double z1,
			double x2, double y2, double z2,
			double bx, double by, double bz,
			double speed, char contact[256]);
int _stdcall bindcall_BuildDynamicEntity(char name[256]);
int _stdcall bindcall_ResetPath(int iPath);
int _stdcall bindcall_AddPathNode(int iPath, 
									double x, double y, double z,
									double h, double p, double r);
int _stdcall bindcall_DriveObjectByPath(char name[256], int iPath, int duration, int bGenFwdDir, 
	char cb_init[256], char cb_during[256], char cb_finish[256]);
int _stdcall bindcall_SetWaterAttenuation(double atte);
int _stdcall bindcall_SetAirAttenuation(double atte);
int _stdcall bindcall_SetCameraLookat(char name[256]);
int _stdcall bindcall_SetCameraLookatOffset(double aoh, double aop, double aor);
int _stdcall bindcall_UseEntity(char name[256], int validate, double cdrange, char cdcb[256], int cdrepeat);
int _stdcall bindcall_ValidateEntity(char name[256]);
int _stdcall bindcall_InvalidateEntity(char name[256]);
int _stdcall bindcall_SetActiveCamera(int cam);
int _stdcall bindcall_SetCamera(int cam, int parent, double fov, double vnear, double vfar);
int _stdcall bindcall_SetOneWave(double winddir, double windspeed, double wcstrength);
int _stdcall bindcall_SetScene(double exposure, double reflection, double mixture);
int _stdcall bindcall_SetExposure(double exposure);
int _stdcall bindcall_SetWaterReflection(double refl);
int _stdcall bindcall_SetCameraPosLookat(double x, double y, double z, char name[256]);
int _stdcall bindcall_SetSubmarineWaterCouplingRegion(char submarine[256], double width);
int _stdcall bindcall_SetTOD(double tr);
int _stdcall bindcall_TranslateEntityPos(char name[256], char ref[256], double x, double y, double z);
int _stdcall bindcall_SetFogAlt(double alt);
int _stdcall bindcall_DoExplosionByTorpedo();
int _stdcall bindcall_DoExplosionByHarpoon();
int _stdcall bindcall_SetEntityLiveState(char name[256], int state);
int _stdcall bindcall_AutoExposure();
int _stdcall bindcall_SetCDTestPosition(double x, double y, double z);
int _stdcall bindcall_ResetPathDriving(char name[256]);
int _stdcall bindcall_SetEntityModel(char name[256], char model[256]);
int _stdcall bindcall_ResetGuidelines();
int _stdcall bindcall_SetFogColor(double r, double g, double b);
int _stdcall bindcall_UpdateDynamics_Decoy(char name[256], char ref[256], int iDecoy, double throwdist,
										double tx, double ty, double tz);
int _stdcall bindcall_SetCloud(int count, int visible, int alt, int width, int height);
int _stdcall bindcall_SetRain(int count, double width, double height);
int _stdcall bindcall_SetSnow(int count, double size);
int _stdcall bindcall_SetWaterColor(double r, double g, double b);
int _stdcall bindcall_SetUnderWaterExplosion(double px, double py, double pz, double progress);
int _stdcall bindcall_SetUnderWaterImpurity(int bShow);
int _stdcall bindcall_SetSwell(float Dir, float Freq, float Height, float Length);


// NON-EXPOSED FUNCTIONS(HOST-INTERNAL-FUNCS)
void nonexposed_SetCameraOri(float matRot[16]);
int nonexposed_SetCameraLookat(char name[256]);
int nonexposed_GetEntity(char name[256]);
void nonexposed_ResetExplosion(int iTor);

