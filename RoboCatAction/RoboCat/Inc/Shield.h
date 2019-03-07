class Shield : public GameObject
{
public:

	CLASS_IDENTIFICATION('SHLD', GameObject)

		enum EYarnReplicationState
	{
		ESRS_Pose = 1 << 0,

		ESRS_AllState = ESRS_Pose
	};

	static	GameObject*	StaticCreate() { return new Shield(); }

	virtual uint32_t	GetAllStateMask()	const override { return ESRS_AllState; }

	virtual uint32_t	Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const override;

	//void		InitEveryMinute();

	//virtual void Update() override;

	virtual bool HandleCollisionWithCat(RoboCat* inCat) override;

protected:
	Shield();

};

typedef shared_ptr< Shield >	ShieldPtr;