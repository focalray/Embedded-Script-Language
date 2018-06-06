//////////////////////////////////////////////// 해치 ///////////////////////////////////////////

function OnEvent_1번해치열기
{
	temp[0] = 28.966 - 28.966*SimEventProgress;
	$SetEntityPosOri("101", 1.294, temp[0], 0.552, 0.0, 0.0, 0.0);
}
function OnEvent_2번해치열기
{
	temp[0] = 28.966 - 28.966*SimEventProgress;
	$SetEntityPosOri("102", -1.294, temp[0], 0.552, 0.0, 0.0, 0.0);
}
function OnEvent_3번해치열기
{
	temp[0] = 29.318 - 29.318*SimEventProgress;
	$SetEntityPosOri("103", 0.113, temp[0], 0.558, 0.0, 0.0, 0.0);
}
function OnEvent_4번해치열기
{
	temp[0] = 29.318 - 29.318*SimEventProgress;
	$SetEntityPosOri("104", -0.113, temp[0], 0.558, 0.0, 0.0, 0.0);
}
function OnEvent_5번해치열기
{
	temp[0] = 29.18 - 29.18*SimEventProgress;
	$SetEntityPosOri("105", 0.113, temp[0], 1.55, 0.0, 0.0, 0.0);
}
function OnEvent_6번해치열기
{
	temp[0] = 29.18 - 29.18*SimEventProgress;
	$SetEntityPosOri("106", -0.113, temp[0], 1.55, 0.0, 0.0, 0.0);
}
function OnEvent_7번해치열기
{
	temp[0] = 28.804 - 28.804*SimEventProgress;
	$SetEntityPosOri("107", 0.111, temp[0], 2.542, 0.0, 0.0, 0.0);
}
function OnEvent_8번해치열기
{
	temp[0] = 28.804 - 28.804*SimEventProgress;
	$SetEntityPosOri("108", -0.111, temp[0], 2.542, 0.0, 0.0, 0.0);
}

function OnEvent_1번해치닫기
{
	temp[0] = 28.966*SimEventProgress;
	$SetEntityPosOri("101", 1.294, temp[0], 0.552, 0.0, 0.0, 0.0);
}
function OnEvent_2번해치닫기
{
	temp[0] = 28.966*SimEventProgress;
	$SetEntityPosOri("102", -1.294, temp[0], 0.552, 0.0, 0.0, 0.0);
}
function OnEvent_3번해치닫기
{
	temp[0] = 29.318*SimEventProgress;
	$SetEntityPosOri("103", 0.113, temp[0], 0.558, 0.0, 0.0, 0.0);
}
function OnEvent_4번해치닫기
{
	temp[0] = 29.318*SimEventProgress;
	$SetEntityPosOri("104", -0.113, temp[0], 0.558, 0.0, 0.0, 0.0);
}
function OnEvent_5번해치닫기
{
	temp[0] = 29.18*SimEventProgress;
	$SetEntityPosOri("105", 0.113, temp[0], 1.55, 0.0, 0.0, 0.0);
}
function OnEvent_6번해치닫기
{
	temp[0] = 29.18*SimEventProgress;
	$SetEntityPosOri("106", -0.113, temp[0], 1.55, 0.0, 0.0, 0.0);
}
function OnEvent_7번해치닫기
{
	temp[0] = 28.804*SimEventProgress;
	$SetEntityPosOri("107", 0.111, temp[0], 2.542, 0.0, 0.0, 0.0);
}
function OnEvent_8번해치닫기
{
	temp[0] = 28.804*SimEventProgress;
	$SetEntityPosOri("108", -0.111, temp[0], 2.542, 0.0, 0.0, 0.0);
}


