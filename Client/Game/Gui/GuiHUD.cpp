#include "GuiHUD.h"

GuiHUD::GuiHUD() {
	const string TEXT_FONT = "Arial";

	setColor(vec4(0.0f));
	setPosition(vec2(0.0f, 0.0f));
	setSize(vec2(1.0f, 1.0f));

	timeBox = Gui::create<GuiRect>(this);
	timeBox->setPosition(vec2(0.4f, 0.9f));
	timeBox->setSize(vec2(0.2f, 0.1f));
	timeBox->setColor(vec4(0.2f, 0.2f, 0.2f, 1.0f));

	timeText = Gui::create<GuiText>(timeBox);
	timeText->setPosition(vec2(0.0f));
	timeText->setAlignment(TextAlign::CENTER);
	timeText->setSize(vec2(1.0f));
	timeText->setText("5:00");
	timeText->setFont(TEXT_FONT);

	leftTeamScoreBox = Gui::create<GuiRect>(this);
	leftTeamScoreBox->setPosition(vec2(0.3f, 0.9f));
	leftTeamScoreBox->setSize(vec2(0.1, 0.1f));
	leftTeamScoreBox->setColor(vec4(0.9f, 0.1f, 0.1f, 1.0f));

	leftTeamScoreText = Gui::create<GuiText>(leftTeamScoreBox);
	leftTeamScoreText->setPosition(vec2(0.0f));
	leftTeamScoreText->setAlignment(TextAlign::CENTER);
	leftTeamScoreText->setSize(vec2(1.0f));
	leftTeamScoreText->setText("0");
	leftTeamScoreText->setFont(TEXT_FONT);

	rightTeamScoreBox = Gui::create<GuiRect>(this);
	rightTeamScoreBox->setPosition(vec2(0.6f, 0.9f));
	rightTeamScoreBox->setSize(vec2(0.1, 0.1f));
	rightTeamScoreBox->setColor(vec4(0.1f, 0.1f, 0.9f, 1.0f));

	rightTeamScoreText = Gui::create<GuiText>(rightTeamScoreBox);
	rightTeamScoreText->setPosition(vec2(0.0f));
	rightTeamScoreText->setAlignment(TextAlign::CENTER);
	rightTeamScoreText->setSize(vec2(1.0f));
	rightTeamScoreText->setText("0");
	rightTeamScoreText->setFont(TEXT_FONT);
}

void GuiHUD::setTime(string time) {
	timeText->setText(time);
}

void GuiHUD::setTime(long msLeft) {
	long hrsLeft = msLeft / 3600000;
	msLeft -= 3600000 * hrsLeft;

	long minsLeft = msLeft / 60000;
	msLeft -= 60000 * minsLeft;

	long secsLeft = msLeft / 1000;
	msLeft -= 1000 * secsLeft;

	string hrs, mins, secs;
	hrsLeft < 10 ? hrs = "0" + std::to_string(hrsLeft) : hrs = std::to_string(hrsLeft);
	minsLeft < 10 ? mins = "0" + std::to_string(minsLeft) : mins = std::to_string(minsLeft);
	secsLeft < 10 ? secs = "0" + std::to_string(secsLeft) : secs = std::to_string(secsLeft);

	if (hrsLeft > 0) {
		GuiHUD::setTime(hrs + ":" + mins + ":" + secs);
	}
	else {
		GuiHUD::setTime(mins + ":" + secs);
	}
}

void GuiHUD::setLeftTeamScore(int score) {
	leftTeamScoreText->setText(std::to_string(score));
}

void GuiHUD::setRightTeamScore(int score) {
	rightTeamScoreText->setText(std::to_string(score));
}