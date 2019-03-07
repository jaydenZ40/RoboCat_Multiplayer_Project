#include <RoboCatPCH.h>

Shield::Shield()
{
	SetScale(GetScale() * 0.25f);
	SetCollisionRadius(0.125f);
}

uint32_t Shield::Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const
{
	uint32_t writtenState = 0;

	if (inDirtyState & ESRS_Pose)
	{
		inOutputStream.Write((bool)true);

		Vector3 location = GetLocation();
		inOutputStream.Write(location.mX);
		inOutputStream.Write(location.mY);

		inOutputStream.Write(GetRotation());

		writtenState |= ESRS_Pose;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}

	return writtenState;
}



bool Shield::HandleCollisionWithCat(RoboCat* inCat)
{
	(void)inCat;

	return false;
}


//void Shield::InitEveryMinute()
//{
//	SetColor();
//	SetPlayerId(inShooter->GetPlayerId());
//
//	Vector3 forward = inShooter->GetForwardVector();
//	SetVelocity(inShooter->GetVelocity() + forward * mMuzzleSpeed);
//	SetLocation(inShooter->GetLocation() /* + forward * 0.55f */);
//
//	SetRotation(inShooter->GetRotation());
//}
//
//void Shield::Update()
//{
//
//	float deltaTime = Timing::sInstance.GetDeltaTime();
//
//
//	//we'll let the cats handle the collisions
//}