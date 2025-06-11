#pragma once

#include "growl/core/error.h"
#include <future>
#include <memory>

namespace Growl {

template <typename T>
struct Future : public std::future<Result<T>> {
	Future() noexcept
		: std::future<Result<T>>() {}

	~Future() {
		if (this->valid() && !isReady()) {
			cancel();
		}
	}

	Future(const Future&) = delete;
	Future& operator=(const Future&) = delete;
	Future(Future&&) = default;
	Future& operator=(Future&&) = default;

	Future(
		std::future<Result<T>> f,
		std::shared_ptr<std::atomic<bool>> cancellation_token =
			nullptr) noexcept
		: std::future<Result<T>>{std::move(f)}
		, cancellation_token{std::move(cancellation_token)} {}

	Future(
		std::future<Result<T>> f,
		std::function<void()> cancellation_func) noexcept
		: std::future<Result<T>>{std::move(f)}
		, cancellation_func{std::move(cancellation_func)} {}

	bool isReady() {
		return this->wait_for(std::chrono::seconds(0)) ==
			   std::future_status::ready;
	}

	void cancel() {
		if (cancellation_token) {
			cancellation_token->store(true);
		} else if (cancellation_func) {
			cancellation_func();
		}
	}

private:
	std::shared_ptr<std::atomic_bool> cancellation_token = nullptr;
	std::function<void()> cancellation_func;
};

} // namespace Growl
