#pragma once
#include <Shared/Game/Powerup.h>

class StunDebuff : public Powerup {
public:
	using Powerup::Powerup;
	static const POWERUP_TYPES TYPE;

	float getDuration() const override;
	void onActivate() override;
	void onDeactivate() override;
};
