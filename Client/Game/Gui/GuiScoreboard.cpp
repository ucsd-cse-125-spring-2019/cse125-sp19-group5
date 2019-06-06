#include "../Renderer/TextRenderer.h"
#include "../Networking/Client.h"
#include "GuiScoreboard.h"

GuiScoreboard::GuiScoreboard() {
	gTextRenderer->loadFont("ScoreboardBig", {
		200, "Fonts/Arial.ttf"
	});

	setColor(vec4(0.0f));
	setSize(vec2(1.0f));

	title = Gui::create<GuiText>(this);
	title->setPosition(vec2(0.0f, 0.8f));
	title->setSize(vec2(1.0f, 1.0f - title->getPosition().y));
	title->setColor(vec4(1.0f));
	title->setFont("Arial");
	title->setText("Red team won the game!");
	title->setAlignment(TextAlign::CENTER);

	auto team0 = Gui::create<GuiRect>(this);
	team0->setColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	team0->setPosition(vec2(0.1f, 0.4f));
	team0->setSize(vec2(0.375f, 0.4f));

	team0Score = Gui::create<GuiText>(team0);
	team0Score->setPosition(vec2(0.0f, 0.0f));
	team0Score->setSize(vec2(1.0f, 1.0f));
	team0Score->setFont("ScoreboardBig");
	team0Score->setColor(vec4(1.0f));
	team0Score->setAlignment(TextAlign::CENTER);
	team0Score->setText("1");

	auto team1 = Gui::create<GuiRect>(this);
	team1->setColor(vec4(0.0f, 0.0f, 1.0f, 1.0f));
	team1->setPosition(vec2(0.525f, 0.4f));
	team1->setSize(vec2(0.375f, 0.4f));

	team1Score = Gui::create<GuiText>(team1);
	team1Score->setPosition(vec2(0.0f, 0.0f));
	team1Score->setSize(vec2(1.0f, 1.0f));
	team1Score->setFont("ScoreboardBig");
	team1Score->setColor(vec4(1.0f));
	team1Score->setAlignment(TextAlign::CENTER);
	team1Score->setText("0");

	team00 = Gui::create<GuiText>(this);
	team00->setPosition(vec2(team0->getPosition().x, 0.2f));
	team00->setSize(vec2(0.4f, 0.1f));
	team00->setFont("Arial");
	team00->setColor(vec4(1.0f));

	team01 = Gui::create<GuiText>(this);
	team01->setPosition(vec2(team0->getPosition().x, 0.1f));
	team01->setSize(vec2(0.4f, 0.1f));
	team01->setFont("Arial");
	team01->setColor(vec4(1.0f));

	team10 = Gui::create<GuiText>(this);
	team10->setPosition(vec2(team1->getPosition().x, 0.2f));
	team10->setSize(vec2(0.4f, 0.1f));
	team10->setFont("Arial");
	team10->setColor(vec4(1.0f));

	team11 = Gui::create<GuiText>(this);
	team11->setPosition(vec2(team1->getPosition().x, 0.1f));
	team11->setSize(vec2(0.4f, 0.1f));
	team11->setFont("Arial");
	team11->setColor(vec4(1.0f));

	score00 = Gui::create<GuiText>(this);
	score00->setPosition(vec2(team0->getPosition().x, 0.2f));
	score00->setSize(vec2(0.375f, 0.1f));
	score00->setFont("Arial");
	score00->setColor(vec4(1.0f));
	score00->setAlignment(TextAlign::RIGHT);

	score01 = Gui::create<GuiText>(this);
	score01->setPosition(vec2(team0->getPosition().x, 0.1f));
	score01->setSize(vec2(0.375f, 0.1f));
	score01->setFont("Arial");
	score01->setColor(vec4(1.0f));
	score01->setAlignment(TextAlign::RIGHT);

	score10 = Gui::create<GuiText>(this);
	score10->setPosition(vec2(team1->getPosition().x, 0.2f));
	score10->setSize(vec2(0.375f, 0.1f));
	score10->setFont("Arial");
	score10->setColor(vec4(1.0f));
	score10->setAlignment(TextAlign::RIGHT);

	score11 = Gui::create<GuiText>(this);
	score11->setPosition(vec2(team1->getPosition().x, 0.1f));
	score11->setSize(vec2(0.375f, 0.1f));
	score11->setFont("Arial");
	score11->setColor(vec4(1.0f));
	score11->setAlignment(TextAlign::RIGHT);

	Network::on(
		NetMessage::SCOREBOARD_HIDE,
		[&](Connection *c, NetBuffer &buffer) {
			remove();
		}
	);
	Network::on(
		NetMessage::SCOREBOARD_SCORE,
		[&](Connection *c, NetBuffer &buffer) {
			auto team = buffer.read<int>();
			auto name = buffer.read<string>();
			auto score = buffer.read<int>();
			addScore(team, name, score);
		}
	);
	Network::on(
		NetMessage::SCOREBOARD_SCORE,
		[&](Connection *c, NetBuffer &buffer) {
			title->setText(buffer.read<string>());
		}
	);
}

void GuiScoreboard::addScore(int team, const string &name, int score) {
	GuiText *guiName = nullptr, *guiScore = nullptr;
	if (team == 0) {
		if (team00->getText() == "") {
			guiName = team00;
			guiScore = score00;
		} else {
			guiName = team01;
			guiScore = score01;
		}
		team0ScoreVal += score;
		team0Score->setText(std::to_string(team0ScoreVal));
	} else {
		if (team10->getText() == "") {
			guiName = team10;
			guiScore = score10;
		} else {
			guiName = team11;
			guiScore = score11;
		}
		team1ScoreVal += score;
		team1Score->setText(std::to_string(team1ScoreVal));
	}

	if (!guiName || !guiScore) { return; }
	guiName->setText(name);
	guiScore->setText(std::to_string(score));
}
