#pragma once

#include <cstddef>

namespace Growl {

class Entity {
public:
	std::size_t getUserData() {
		return user_data;
	}

protected:
	Entity() = default;

	Entity(std::size_t user_data)
		: user_data{user_data} {}

	void setUserData(std::size_t user_data) {
		this->user_data = user_data;
	}

private:
	std::size_t user_data = 0;
};

} // namespace Growl
