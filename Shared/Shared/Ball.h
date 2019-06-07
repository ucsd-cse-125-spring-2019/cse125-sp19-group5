#pragma once
#include <set>
#include "SphereGameObject.h"
#include "Bullet.h"
#include "Goal.h"
#include "Paddle.h"
#include "Player.h"
#include "Wall.h"

#define GROUNDED_TICKS_THRESHOLD 10

class Ball : public SphereGameObject {
public:
	using SphereGameObject::SphereGameObject;
	GAMEOBJECT_TYPES getGameObjectType() const;
	void updateOnServerTick();
	bool getGoalScored();

	// collision interactions below
	void onCollision(GameObject * gameObject);
	void onCollision(Ball * ball);
	void onCollision(Bullet * bullet);
	void onCollision(Goal * goal);
	void onCollision(Paddle * paddle);
	void onCollision(Player * player);
	void onCollision(Wall * wall);

	int static getRandIndex(int size) { return std::rand() % size; };
	float static getRandPercent() { return static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX); };

private:
	bool goalScored = false;
	std::set<Ball *> currentBallCollisions;
	int ticksSinceGrounded = 0;
	bool isGrounded = false;

	vector<string> soundHit{ "Sounds/ping_pong_hit_1.wav", "Sounds/ping_pong_hit_2.wav" , "Sounds/ping_pong_hit_3.wav" };
	const float SOUND_HIT_CD = 500.0f;
	float soundHitTimer = 0.0f;

	vector<string> soundBounce{ "Sounds/ping_pong_bounce_1.wav", "Sounds/ping_pong_bounce_2.wav" , "Sounds/ping_pong_bounce_3.wav" };
	const float SOUND_BOUNCE_CD = 200.0f;
	float soundBounceTimer = 0.0f;

	string soundCrowdCheer = "Sounds/crowd_cheer.wav";

	string soundPlayerOof = "Sounds/player_oof.wav";
	string soundPlayerSuperOof = "Sounds/player_ooooof.wav";
	const float SOUND_OOF_CD = 200.0f;
	float soundOofTimer = 0.0f;
};