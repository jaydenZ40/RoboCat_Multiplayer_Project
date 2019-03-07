#include <RoboCatServerPCH.h>


ShieldServer::ShieldServer()
{
}

void ShieldServer::HandleDying()
{
	NetworkManagerServer::sInstance->UnregisterGameObject(this);
}

bool ShieldServer::HandleCollisionWithCat(RoboCat* inCat)
{
	//kill yourself!
	SetDoesWantToDie(true);

	static_cast< RoboCatServer* >(inCat)->CollectShield();

	return false;
}