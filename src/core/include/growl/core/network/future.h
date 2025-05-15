#pragma once

#include "growl/core/error.h"
#include <future>

namespace Growl {

template <typename T>
struct Future : public std::future<Result<T>> {
	Future() noexcept
		: std::future<Result<T>>() {}

	Future(std::future<Result<T>> f) noexcept
		: std::future<Result<T>>{std::move(f)} {}

	bool isReady() {
		return this->wait_for(std::chrono::seconds(0)) ==
			   std::future_status::ready;
	}
};

} // namespace Growl
