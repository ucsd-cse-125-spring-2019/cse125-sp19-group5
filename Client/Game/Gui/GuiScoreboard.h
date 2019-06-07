#pragma once

#include "../Renderer/Gui/Gui.h"
#include "../Renderer/Gui/GuiRect.h"
#include "../Renderer/Gui/GuiText.h"

class GuiScoreboard : public GuiRect {
	public:
	float alpha = 1.0f;

	GuiScoreboard();
	void addScore(int team, const string &name, int score);

	private:
	float curAlpha = 0.0f;
	int team0ScoreVal = 0, team1ScoreVal = 0;
	GuiText *title;
	GuiText *team0Score;
	GuiText *team1Score;
	GuiText *team00, *team01, *team10, *team11;
	GuiText *score00, *score01, *score10, *score11;
};