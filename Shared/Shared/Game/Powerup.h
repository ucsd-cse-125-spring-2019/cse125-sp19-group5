#pragma once

#include "../Player.h"

class Powerup {
	protected:
	Player &player;
	float deactivateTime;

	public:
	static const string TYPE;

	Powerup(Player &player);

	// Call this when the player receives the powerup to set up duration.
	void activate();

	// Returns how long the powerup lasts in seconds. Return a negative
	// number to have the powerup last forever.
	virtual float getDuration() const;

	// Called when a player activates this power up.
	virtual void onActivate();

	// Called when the time for the powerup has expired.
	virtual void onDeactivate();

	// Called every tick while the player has the powerup.
	virtual void update();

	// Called to determine whether or not the powerup should be deactivated.
	virtual bool shouldDeactivate() const;

	virtual ~Powerup() = default;
};