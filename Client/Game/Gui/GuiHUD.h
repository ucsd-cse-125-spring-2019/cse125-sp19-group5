#pragma once

#include "../Renderer/Gui/Gui.h"
#include "../Renderer/Gui/GuiRect.h"
#include "../Renderer/Gui/GuiText.h"

class GuiHUD : public GuiRect {
private:
	GuiRect * timeBox;
	GuiText * timeText;

	GuiRect * leftTeamScoreBox;
	GuiText * leftTeamScoreText;

	GuiRect * rightTeamScoreBox;
	GuiText * rightTeamScoreText;

public:
	GuiHUD();

	void setTime(string time);
	void setTime(long msLeft);
	void setLeftTeamScore(int score);
	void setRightTeamScore(int score);
};