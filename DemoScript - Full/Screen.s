function OnEvent_FadeIn
{
	temp[0] = 0.07 * SimEventProgress;
	$SetScene(temp[0], 0.5, 0.75); // exposure, reflection, mixture
}

