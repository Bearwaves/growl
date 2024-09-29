#pragma once

#include "growl/core/game/game.h"
#include "growl/core/graphics/batch.h"
#include "growl/ui/widget.h"

namespace UIExample {

class Game : public Growl::Game {
public:
	Growl::Error init() override;
	Growl::Error dispose() override;
	void tick(double delta_time) override;
	void render(double delta_time) override;
	void resize(const int width, const int height) override;

private:
	std::unique_ptr<Growl::Batch> batch;
	std::unique_ptr<Growl::Widget> root;
};

} // namespace UIExample
