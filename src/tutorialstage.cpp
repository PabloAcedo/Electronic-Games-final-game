#include "tutorialstage.h"
#include "input.h"
#include "scene.h"
#include "game.h"

const char* tut_name;

Tutorialstage::Tutorialstage() : Stage("tutorial") {

}


Tutorialstage::~Tutorialstage()
{
}

void Tutorialstage::render() {

	if (Scene::instance->gamePad)
		tut_name = "data/Definitivo/gui/buttonsConfig.png";
	else
		tut_name = "data/Definitivo/gui/buttonsConfigPC.png";

	//BG
	Scene::instance->renderGUI(Game::instance->window_width / 2, Game::instance->window_height / 2,
		Game::instance->window_width, Game::instance->window_height,
		tut_name, Vector4(0, 0, 1, 1));
}

void Tutorialstage::update(double dt) {

	if (Input::wasKeyPressed(SDL_SCANCODE_Z) || Input::isButtonPressed(X_BUTTON)) {
		if (Stage::s_stages["intro"]->mode == CONTROLS) {
			Stage::currentStage->changeStage("intro");
		}
		else {
			Stage::currentStage->changeStage("pause");
		}
	}

}
