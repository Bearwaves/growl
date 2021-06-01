#include "test_app_game.h"

#include <iostream>

using Growl::TestAppGame;

void TestAppGame::create() {
	std::cout << "Game create" << std::endl;
}

void TestAppGame::render() {
	getAPI().graphics->clear(0, 0, 0);
}

void TestAppGame::resize(const int width, const int height) {}

TestAppGame::~TestAppGame() {
	std::cout << "Game destroy" << std::endl;
}
