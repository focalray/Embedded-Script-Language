
function OnEvent_PeriscopeUp
{	
	echoln "OnEvent_PeriscopeUp";
	temp[0] = 6.0 * SimEventProgress;
	$SetEntityPosOri("111", 0.0, 0.0, temp[0], 0.0, 0.0, 0.0);
	//$PlaySoundEffect("페리스코프업다운");
}

function OnEvent_PeriscopeDown
{
	echoln "OnEvent_PeriscopeDown";
	temp[0] = 6.0 - 6.0 * SimEventProgress;
	$SetEntityPosOri("111", 0.0, 0.0, temp[0], 0.0, 0.0, 0.0);
	//$PlaySoundEffect("페리스코프업다운");
}

function OnEvent_PeriscopeInit
{
	echoln "OnEvent_PeriscopeInit";
	$SetEntityPosOri("111", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

function OnEvent_PeriscopeZoomIn
{
	echoln "OnEvent_PeriscopeZoomIn";
	temp[0] = 20.0 - 18.0 * SimEventProgressSmooth;

	$SetCamera(2, 112, temp[0], 1.0, 24000.0);
	//$StopSoundEffect("페리스코프업다운");
	$PlaySoundEffect("페리스코프타겟");
}

function OnEvent_PeriscopeZoomOut
{
	echoln "OnEvent_PeriscopeZoomOut";
	temp[0] = 2.0 + 18.0 * SimEventProgressSmooth;

	$SetCamera(2, 112, temp[0], 1.0, 24000.0);
	//$StopSoundEffect("페리스코프업다운");
}

function OnEvent_잠망경_목표물탐색왼쪽으로
{
	echoln "OnEvent_잠망경_목표물탐색왼쪽으로";
	temp[0] = -15.0 * SimEventProgressSmooth;

/* esl: 비교관련오류
	if temp[0] > -17.5
	{
		temp[0] = -17.5;
	}
*/
	$SetEntityPosOri("111", 0.0, 0.0, 6.0, temp[0], 0.0, 0.0);
	//$StopSoundEffect("페리스코프업다운");
}
function OnEvent_잠망경_목표물탐색오른쪽으로
{
	echoln "OnEvent_잠망경_목표물탐색오른쪽으로";
	temp[0] = -15.0 - 2.3 * SimEventProgressSmooth;
	save[0] = temp[0];
	$SetEntityPosOri("111", 0.0, 0.0, 6.0, temp[0], 0.0, 0.0);
	//$StopSoundEffect("페리스코프업다운");
}

function OnEvent_PeriscopeZoomIn_적함포착_확대
{
	echoln "OnEvent_PeriscopeZoomIn_적함포착_확대";
	temp[0] = 20.0 - 19.0 * SimEventProgress;
	temp[1] = save[0];

	$SetEntityPosOri("111", 0.0, 0.0, 6.0, temp[1], 0.0, 0.0);
	$SetCamera(2, 112, temp[0], 1.0, 24000.0);
	//$StopSoundEffect("페리스코프업다운");
}

function OnEvent_잠망경_적함포착후_약간오른쪽으로
{
	echoln "OnEvent_잠망경_적함포착후_약간오른쪽으로";
	temp[0] = -17.3 + 1.0 * SimEventProgress;
	$SetEntityPosOri("111", 0.0, 0.0, 6.0, temp[0], 0.0, 0.0);
	//$StopSoundEffect("페리스코프업다운");
}

function OnEvent_잠망경_적함폭발관찰
{
	echoln "OnEvent_잠망경_적함폭발관찰";

	g_ActiveCamera = 2;
	temp[0] = -17.5;
	$SetEntityPosOri("111", 0.0, 0.0, 6.0, temp[0], 0.0, 0.0);
	$SetCamera(2, 112, 10.0, 1.0, 24000.0);
	//$StopSoundEffect("페리스코프업다운");
}


