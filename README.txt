ICS 167 Project
Jiapeng Zhao	59535899
Eric Liu		56277704
Ken Liu		71888352

Generate a shield each time a player joins or respawns:
Added files: 
	Shield.h, Shield.cpp
	ShieldClient.h, ShieldClient.cpp
	ShieldServer.h, ShieldServer.cpp
Modified files:
	RoboCatShared.h, RoboCatClientPCH.h, RoboCatServerPCH.h: Share new files.
	HUD.h, HUD.cpp: Added text "Shield: **" next to "Health: **"
	TextureManager.cpp: Loaded picture of shield
	RoboCat.h, RoboCat.cpp: Added ECRS_Shield, added one more writeState for shield
	RoboCatClient.cpp: Added one more readState for shield
	RoboCatServer.h, RoboCatServer.cpp: Added CollectShield and set dirty, modified TakeDamage function.
	Server.cpp: Add CreateRandomShield function and call it in SpawnCatForPlayer for every time a cat spawns except the first one.


Local Prediction & Interpolation:
Modified files:
	MoveList.cpp: Modified AddMove, only return true when timestamp is larger than last timestamp.

	RoboCat.h, RoboCat.cpp: 
		Added boolean varible to check whether the movement is authority. 
		Write one more bit for mIsAuthority.

	RoboCatClient.h, RoboCatClient.cpp: 
		Added PredictRemoteCat and PredictLocalCat functions, and call one of them when Read depended on whether the playID matches the playerID in the client.
		Read one more bit for mIsAuthority.
		For remote cat, get the time that needs to be interpolated by calculate the difference of receiving two packets. According to old physical infomation to predict and interpolate the remote cat.
		For local cat, if it is not authority, back to the previous location and re-process inputs.
		In Update, predict the local cat's movement if it is authority before reading last packet.