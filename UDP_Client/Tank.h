#pragma once
#include "Entity.h"
#include "Networking.h"

class Tank : public Entity
{
public:
	Tank();
	Tank(const unsigned short& slot);
	virtual void Update(const float& dt) override;
	virtual void Render(sf::RenderTarget* target) override;
	virtual void SetDirection(const Networking::PlayerDirection& direction);
	virtual const bool TimeOutReached() const;
	virtual const bool LeaveCondition() const;
	virtual ~Tank();
	float time_since_heard_from_client = 0;
	unsigned short slot;
	ClientStage stage;
	sf::Text text;
private:
	
	

};