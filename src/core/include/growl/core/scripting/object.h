#pragma once

namespace Growl {

// An Object is a reference to something that exists in the scripting
// language's execution state or VM, which we can't normally access from C.
class Object {
public:
	Object() = default;
	virtual ~Object() = default;

	// Reference is move-only
	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;
	Object(Object&&) = default;
	Object& operator=(Object&&) = default;
};

} // namespace Growl
