class RoboCatClient : public RoboCat
{
public:
	static	GameObjectPtr	StaticCreate()		{ return GameObjectPtr( new RoboCatClient() ); }

	virtual void	Update();
	virtual void	HandleDying() override;

	virtual void	Read( InputMemoryBitStream& inInputStream ) override;

protected:
	RoboCatClient();
	void			PredictLocalCat(uint32_t inReadState, float oldRotation, Vector3 oldLocation, Vector3 oldVelocity);
	void			PredictRemoteCat(uint32_t inReadState, float oldRotation, Vector3 oldLocation, Vector3 oldVelocity);
	void			InterpolateMovement(float oldRotation, Vector3 oldLocation, Vector3 oldVelocity);

private:
	float			mTimeLocationBecameOutOfSync;
	float			mTimeVelocityBecameOutOfSync;

	float			mTimeReceivedLastPacket;
	float			mTimeBeInterpolated;
	int				mReceiveAuthoritiedNumber;
	
	SpriteComponentPtr	mSpriteComponent;
};