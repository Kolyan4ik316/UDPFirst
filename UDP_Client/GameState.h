#pragma once
#include "State.h"
#include "Entity.h"
#include "Tank.h"

class GameState : public State
{
public:
	GameState(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<Client> client);
	void Update(const float& dt) override;
	void Render(sf::RenderTarget* target = nullptr) override;
	void UpdateInput(const float& dt) override;
	void EndState() override;
	void UnpackMsg() override;
	void PackMsg() override;
	void OnEnable();
	void OnDisable();
	virtual ~GameState();
private:
	Tank player;
	std::vector<Tank> otherPlayers;
	char input = 0;
	std::mutex mtx;
	std::stack<std::thread> threads;
	//std::unique_ptr<std::thread> receiver;
};