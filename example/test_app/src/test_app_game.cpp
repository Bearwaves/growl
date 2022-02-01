#include "test_app_game.h"
#include <growl/util/image/image.h>

#include <iostream>

using Growl::TestAppGame;

void TestAppGame::init() {
	std::cout << "Game starting up!" << std::endl;
	Image img = loadImageFromFile("/Users/joel/Downloads/paw.jpg");
	std::cout << "Got image! W " << img.getWidth() << ", H " << img.getHeight()
			  << ", Ch " << img.getChannels() << std::endl;
}

void TestAppGame::render() {
	getAPI().graphics()->clear(1, 0, 0);
}

void TestAppGame::resize(const int width, const int height) {}

TestAppGame::~TestAppGame() {
	std::cout << "Game destroy" << std::endl;
}
