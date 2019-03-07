class ShieldClient : public Shield
{
public:
	static	GameObjectPtr	StaticCreate() { return GameObjectPtr(new ShieldClient()); }

	virtual void		Read(InputMemoryBitStream& inInputStream) override;

protected:
	ShieldClient();

private:

	SpriteComponentPtr	mSpriteComponent;
};