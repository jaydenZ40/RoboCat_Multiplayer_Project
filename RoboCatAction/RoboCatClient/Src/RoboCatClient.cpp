#include <RoboCatClientPCH.h>


RoboCatClient::RoboCatClient() :
	mTimeLocationBecameOutOfSync( 0.f ),
	mTimeVelocityBecameOutOfSync( 0.f ),
	mReceiveAuthoritiedNumber(NULL)
{
	mSpriteComponent.reset( new SpriteComponent( this ) );
	mSpriteComponent->SetTexture( TextureManager::sInstance->GetTexture( "cat" ) );
}

void RoboCatClient::HandleDying()
{
	RoboCat::HandleDying();

	//and if we're local, tell the hud so our health goes away!
	if( GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId() )
	{
		HUD::sInstance->SetPlayerHealth( 0 );
	}
}

void RoboCatClient::Update()
{
	if (GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId() && mIsAuthority)
	{
		// Problem here! cannot figure out how to get the simulate time, maybe fixed timestep will help 

		//float deltaTime =  - Timing::sInstance.GetTimef() + InputManager::sInstance->GetNextTimeToSampleInput();
		float deltaTime = 0.016f; // kTimeBetweenInputSamples
		//float deltaTime = Timing::sInstance.GetDeltaTime();
		ProcessInput(deltaTime, InputManager::sInstance->GetState());
		SimulateMovement(deltaTime);

	}
}

void RoboCatClient::PredictLocalCat(uint32_t inReadState, float oldRotation, Vector3 oldLocation, Vector3 oldVelocity)
{
	if ((inReadState & ECRS_Pose) != 0)
	{
		const MoveList& list = InputManager::sInstance->GetMoveList();

		if (!mIsAuthority)// different from the unauthorized state, back to previous location and re-process inputs
		{
			for (int i = 0; i < list.GetMoveCount(); i++)
			{

				SetRotation(oldRotation);
				SetLocation(oldLocation);
				SetVelocity(oldVelocity);

				float deltaTime = list[i].GetDeltaTime();
				ProcessInput(deltaTime, list[i].GetInputState());
				SimulateMovement(deltaTime);
			}
		}

		mLastMoveTimestamp = Timing::sInstance.GetTimef();

		mIsAuthority = true;
	}
}

void RoboCatClient::PredictRemoteCat(uint32_t inReadState, float oldRotation, Vector3 oldLocation, Vector3 oldVelocity)
{
	if ((inReadState & ECRS_Pose) != 0)
	{
		float intervalTime = mTimeBeInterpolated;
		while (intervalTime > 0)
		{
			InterpolateMovement(oldRotation, oldLocation, oldVelocity);
			intervalTime -= Timing::sInstance.GetDeltaTime();

		}
	}
}


void RoboCatClient::Read( InputMemoryBitStream& inInputStream )
{
	bool stateBit;
	
	uint32_t readState = 0;

	inInputStream.Read( stateBit );
	if( stateBit )
	{
		uint32_t playerId;
		inInputStream.Read( playerId );
		SetPlayerId( playerId );
		readState |= ECRS_PlayerId;
	}

	float oldRotation = GetRotation();
	Vector3 oldLocation = GetLocation();
	Vector3 oldVelocity = GetVelocity();

	float replicatedRotation;
	Vector3 replicatedLocation;
	Vector3 replicatedVelocity;

	inInputStream.Read( stateBit );
	if( stateBit )
	{
		inInputStream.Read( replicatedVelocity.mX );
		inInputStream.Read( replicatedVelocity.mY );

		SetVelocity( replicatedVelocity );

		inInputStream.Read( replicatedLocation.mX );
		inInputStream.Read( replicatedLocation.mY );

		SetLocation( replicatedLocation );

		inInputStream.Read( replicatedRotation );
		SetRotation( replicatedRotation );

		readState |= ECRS_Pose;
	}

	inInputStream.Read( stateBit );
	if( stateBit )
	{
		inInputStream.Read( stateBit );
		mThrustDir = stateBit ? 1.f : -1.f;
	}
	else
	{
		mThrustDir = 0.f;
	}

	inInputStream.Read( stateBit );
	if( stateBit )
	{
		Vector3 color;
		inInputStream.Read( color );
		SetColor( color );
		readState |= ECRS_Color;
	}

	inInputStream.Read( stateBit );
	if( stateBit )
	{
		mHealth = 0;
		inInputStream.Read( mHealth, 4 );
		readState |= ECRS_Health;
	}

	if( GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId() )
	{
		//did we get health? if so, tell the hud!
		if( ( readState & ECRS_Health ) != 0 )
		{
			HUD::sInstance->SetPlayerHealth( mHealth );
		}
	}	

	inInputStream.Read(stateBit);
	if (stateBit)
	{
		mShield = 0;
		inInputStream.Read(mShield, 4);
		readState |= ECRS_Shield;
	}

	if (GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId())
	{
		//did we get health? if so, tell the hud!
		if ((readState & ECRS_Shield) != 0)
		{
			HUD::sInstance->SetPlayerShield(mShield);
		}
	}

	inInputStream.Read(mIsAuthority);

	mTimeBeInterpolated = Timing::sInstance.GetTimef() - mTimeReceivedLastPacket;
	mTimeReceivedLastPacket = Timing::sInstance.GetTimef();

	bool isLocal = (GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId());
	if (isLocal)
	{
		PredictLocalCat(readState, oldRotation, oldLocation, oldVelocity);
	}
	else
	{
		PredictRemoteCat(readState, oldRotation, oldLocation, oldVelocity);
	}
}

void RoboCatClient::InterpolateMovement(float oldRotation, Vector3 oldLocation, Vector3 oldVelocity)
{
	Vector3 interpolateVelocity;
	float interpolateRotation;
	interpolateVelocity.mX = (GetLocation().mX - oldLocation.mX) / mTimeBeInterpolated;
	interpolateVelocity.mY = (GetLocation().mY - oldLocation.mY) / mTimeBeInterpolated;
	interpolateVelocity.mZ = (GetLocation().mZ - oldLocation.mZ) / mTimeBeInterpolated;
	interpolateRotation = (GetRotation() - oldRotation) / mTimeBeInterpolated;

	SetLocation(oldLocation + interpolateVelocity * Timing::sInstance.GetDeltaTime());
	SetRotation(oldRotation + interpolateRotation * Timing::sInstance.GetDeltaTime());

	ProcessCollisions();
}