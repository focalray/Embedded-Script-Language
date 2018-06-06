
#define BOOL	int
#define TRUE	1
#define FALSE	0

#define PATH_IDX_00		0
#define PATH_IDX_01		1
#define PATH_IDX_02		2
#define PATH_IDX_03		3
#define PATH_IDX_04		4
#define PATH_IDX_05		5
#define PATH_IDX_06		6
#define PATH_IDX_07		7
#define PATH_IDX_08		8

#define PORT_SCENECOMPOSITION	0
#define PORT_CAMERA				1
#define PORT_ACTIVECAMERA		2
#define PORT_MOVEENTITY			3
#define PORT_KILLENTITY			4
#define PORT_ATMOSPHERE			5
#define PORT_EMITPARTICLE		6
#define PORT_ONEWAVE			7
#define PORT_TORPEDOHIT			8
#define PORT_EXPLOSION			9
#define PORT_ROCKETTRAIL		10
#define PORT_CAVITATIONBUBBLE	11
#define PORT_CAMERAOFFSET		12
#define PORT_OBJECTWATERCOUPLING_REGION		13
#define PORT_OBJECTWATERCOUPLING_ADDOBJECT	14
#define PORT_GUIDELINE			15
#define PORT_WEATHER			16
#define PORT_UNDERWATEREXPLOSION	17
#define PORT_UNDERWATER_IMPURITY 18
#define PORT_SWELL				19
#define PORT_ENTITYINFO			20


double temp[16];		// 글로벌 임시변수
double save[16];		// 글로벌 임시변수


//*** typedef 키워드가 제공되지 않습니다. 
//*** 상속기능 제공되지 않습니다.
//*** unsigned char ==> BYTE
//*** unsigned short ==> WORD
//*** unsigned __int64 ==> INT64
//*** unsigned int ==> DWORD

struct POS
{
	double x;
	double y;
	double z;
};
struct ORI
{
	float h;
	float p;
	float r;
};
struct ENTITY_SCRIPT
{
	POS	pos;
	ORI	ori;
	POS	dir;
	POS vel;
};
ENTITY_SCRIPT	svEntityInfo;
ENTITY_SCRIPT	g_EnemySubTorpedo1;
ENTITY_SCRIPT	g_EnemySubTorpedo2;
ENTITY_SCRIPT	g_SubmarineInfo;
POS				g_SubmarinePosLast;
//ENTITY_SCRIPT	g_EnemySubmarineInfo[2];
ENTITY_SCRIPT	g_EnemySubmarine1Info;
ENTITY_SCRIPT	g_EnemySubmarine2Info;
ENTITY_SCRIPT	g_CameraEntryInfo;
double			g_TorpedoSpeed;
int				g_ActiveCamera = 1;
int				g_DecoyFired = 0;
int				g_AutoExposure = 1;
int				tickstart;


///////////////////////////////////////// HxCode.h ///////////////////////////////////////////

#define HXCODE_BEGINPACKET          0
#define HXCODE_NEWFRAME	            1
#define HXCODE_MOVEENTITY           2
#define HXCODE_ACTIVECAMERA         3
#define HXCODE_KILLENTITY           4
#define HXCODE_ATMOSPHERE           5
#define HXCODE_SCENECOMPOSITION     6
#define HXCODE_CAMERA               7
#define HXCODE_CAMERAOFFSET         8
#define HXCODE_ONEWAVE              9

#define HXCODE_EMITPARTICLE        10
#define HXCODE_TORPEDOHIT          11 
#define HXCODE_EXPLOSION           12
#define HXCODE_ROCKETTRAIL         13
#define HXCODE_CAVITATIONBUBBLE    14
#define HXCODE_OBJECTWATERCOUPLING_REGION       15
#define HXCODE_OBJECTWATERCOUPLING_ADDOBJECT    16
#define HXCODE_GUIDELINE             17
#define HXCODE_WEATHER              18
#define HXCODE_UNDERWATEREXPLOSION 19
#define HXCODE_UNDERWATER_IMPURITY 20
#define HXCODE_SWELL 21



struct HXCONTROLHEADER
{
    WORD Code;    
    WORD Length;
};

struct HXCONTROL_BEGINPACKET
{
	HXCONTROLHEADER hdr;
    DWORD TotalPacketLength;
    DWORD iChannel;  
};

struct HXCONTROL_NEWFRAME 
{
	HXCONTROLHEADER hdr;
    DWORD iFrame;
	INT64 TimeStamp;
};

struct HXCONTROL_MOVEENTITY
{
	HXCONTROLHEADER hdr;
    DWORD iEntity;
    DWORD Flags;
        // bit0: parent
        // bit1: model
        // bit2: light
        // bit3: timestamp
        // bit4: pos
        // bit5: velo
        // bit6: matrot
    DWORD iParentEntity;
    DWORD iModel;
    DWORD iLight;
    INT64 TimeStamp;
    double Pos[3];
    float Velo[3];
    float matRot[16];
};
HXCONTROL_MOVEENTITY	svHXCONTROL_MOVEENTITY;

struct HXCONTROL_ACTIVECAMERA
{
	HXCONTROLHEADER hdr;
    DWORD iCamera;
};
HXCONTROL_ACTIVECAMERA	svHXCONTROL_ACTIVECAMERA;

struct HXCONTROL_KILLENTITY
{
	HXCONTROLHEADER hdr;
    DWORD iEntity;
    DWORD Method;
};
HXCONTROL_KILLENTITY	svHXCONTROL_KILLENTITY;

struct HXCONTROL_ATMOSPHERE
{
	HXCONTROLHEADER hdr;
    float AirAttenuation;
    float WaterAttenuation;
    float FogHigh;
    float FogColor[3];
    float WaterColor[3];
    float SkyBright[3];
    float SunLight[3];
    float SunDir[3];
};
HXCONTROL_ATMOSPHERE	svHXCONTROL_ATMOSPHERE;

struct HXCONTROL_SCENECOMPOSITION
{
	HXCONTROLHEADER hdr;
    float Exposure;
    float WaterReflection;
    float WaterMixture;
};
HXCONTROL_SCENECOMPOSITION	svHXCONTROL_SCENECOMPOSITION;

struct HXCONTROL_EMITPARTICLE
{
	HXCONTROLHEADER hdr;
    int Type;
    int Life;
    double Pos[3];
    float Velo[3];
    float Color0[4];
    float Color1[4];
    float Radius0;
    float Radius1;
    float ColorTransitionStrength;
    float AlphaTransitionStrength;
    float RadiusTransitionStrength;
};
HXCONTROL_EMITPARTICLE	svHXCONTROL_EMITPARTICLE;

struct HXCONTROL_ONEWAVE
{
	HXCONTROLHEADER hdr;
    float WindDir;
    float WindSpeed;
    float WhiteCapStrength;
};
HXCONTROL_ONEWAVE	svHXCONTROL_ONEWAVE;

struct HXCONTROL_TORPEDOHIT
{
	HXCONTROLHEADER hdr;
    double Pos[3];
    DWORD TimeStamp;
};

//#PORTPACK 8
HXCONTROL_TORPEDOHIT	svHXCONTROL_TORPEDOHIT;
//#PORTPACK 4

struct HXCONTROL_EXPLOSION
{
	HXCONTROLHEADER hdr;
    double Pos[3];
    DWORD TimeStamp;
    DWORD nParticleParFrame;
    float ExplosionRadius;
    float ParticleRadius;
    float Color0[4];
    float Color1[4];
};
HXCONTROL_EXPLOSION		svHXCONTROL_EXPLOSION;

struct HXCONTROL_ROCKETTRAIL
{
	HXCONTROLHEADER hdr;
    DWORD iEntity;
    float ParticleRadius;
    float Speed;
    float Color[4];
};
HXCONTROL_ROCKETTRAIL	varHXCONTROL_ROCKETTRAIL;

struct HXCONTROL_CAVITATIONBUBBLE
{
	HXCONTROLHEADER hdr;
    DWORD iEntity;
    float ParticleRadius;
    float Speed;
    float Color[4];
};
HXCONTROL_CAVITATIONBUBBLE	svHXCONTROL_CAVITATIONBUBBLE;

struct HXCONTROL_CAMERA
{
	HXCONTROLHEADER hdr;
    dword iCamera;
    dword iParent;
    float vFov;
    float Near;
    float Far;
};
HXCONTROL_CAMERA			svHXCONTROL_CAMERA;

struct HXCONTROL_CAMERAOFFSET
{
	HXCONTROLHEADER hdr;
    dword iCamera;
    double Pos[3];
    float matRot[16];
};
HXCONTROL_CAMERAOFFSET		svHXCONTROL_CAMERAOFFSET;

struct HXCONTROL_OBJECTWATERCOUPLING_REGION
{
	HXCONTROLHEADER hdr;
    DWORD BufferSize;
    double West;
    double South;
    double East;
    double North;
};
HXCONTROL_OBJECTWATERCOUPLING_REGION	svHXCONTROL_OBJECTWATERCOUPLING_REGION;

struct HXCONTROL_OBJECTWATERCOUPLING_ADDOBJECT
{
	HXCONTROLHEADER hdr;
    DWORD AddRemove; // 0: add, 1: remove
    DWORD iEntity;
};
HXCONTROL_OBJECTWATERCOUPLING_ADDOBJECT		svHXCONTROL_OBJECTWATERCOUPLING_ADDOBJECT;

struct HXCONTROL_GUIDELINE
{
	HXCONTROLHEADER hdr;
	DWORD iEntityID;
	DWORD bVisible;
	double Pos[3];
};
HXCONTROL_GUIDELINE		svHXCONTROL_GUIDELINE;


struct HXCONTROL_WEATHER
{
	HXCONTROLHEADER hdr;
	DWORD nCountSnow;
	DWORD bRain;
	float fSnowSize;
	float fRainWidth;
	float fRainHeight;
	DWORD nCloudCount;
	DWORD bCloudVisible;
	DWORD nCloudAltitude;
	DWORD nCloudWidth;
	DWORD nCloudHeight;
};
HXCONTROL_WEATHER		svHXCONTROL_WEATHER;


struct HXCONTROL_UNDERWATEREXPLOSION
{
	HXCONTROLHEADER hdr;
    float Progress;
    float Radius;
    float BaseColor[4];
    double Pos[3];
};
HXCONTROL_UNDERWATEREXPLOSION	svHXCONTROL_UNDERWATEREXPLOSION;


struct HXCONTROL_UNDERWATER_IMPURITY
{
	HXCONTROLHEADER hdr;
	DWORD bImpurity;
};
HXCONTROL_UNDERWATER_IMPURITY	svHXCONTROL_UNDERWATER_IMPURITY;


struct HXCONTROL_SWELL
{
	HXCONTROLHEADER hdr;
	float fSwellDirection;
	float fSwellHeight;
	float fSwellLength;
	float fSwellFreq;
};
HXCONTROL_SWELL		svHXCONTROL_SWELL;

