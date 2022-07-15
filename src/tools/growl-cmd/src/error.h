#pragma once

#include "growl/core/assets/error.h"

namespace Growl {

enum class AssetsIncludeErrorCode { None, WrongType, LoadFailed };

class AssetsIncludeError : public Growl::AssetsError {
public:
	AssetsIncludeError(AssetsIncludeErrorCode code)
		: AssetsError("")
		, code{code} {}
	AssetsIncludeError(std::string message)
		: AssetsError(message)
		, code{AssetsIncludeErrorCode::LoadFailed} {}

	const AssetsIncludeErrorCode getCode() const {
		return code;
	}

private:
	AssetsIncludeErrorCode code;
};

} // namespace Growl
