#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "growl/core/error.h"
#include "growl/core/input/processor.h"
#include "growl/core/scripting/script.h"
#include <memory>
#include <string>
#include <vector>

namespace Growl {

class Batch;

class Node : public InputProcessor {
public:
	Node(std::string&& label) {
		this->label = std::move(label);
	}

	virtual ~Node() = default;
	std::string& getLabel();

	SCRIPTED_GETSET(float, X, x);
	SCRIPTED_GETSET(float, Y, y);
	SCRIPTED_GETSET(float, Width, w);
	SCRIPTED_GETSET(float, Height, h);
	SCRIPTED_GETSET(float, Rotation, rotation);

	Node* addChild(std::unique_ptr<Node> node);
	void draw(Batch& batch, float parent_alpha);
	void drawChildren(Batch& batch, float parent_alpha);

	virtual void onEvent(InputEvent& event) override;
	bool hit(float x, float y);

	Error bindScript(ScriptingAPI& api, Script& script);

protected:
	virtual void onPopulateDebugUI(Batch& batch) {}
	virtual void onDraw(Batch& batch, float parent_alpha);

private:
	ScriptingAPI* scripting_api;
	std::string label;
	Node* parent;
	float x = 0;
	float y = 0;
	float w = 0;
	float h = 0;
	float rotation = 0;
	std::vector<std::unique_ptr<Node>> children;
	glm::mat4x4 local_transform;
	glm::mat4x4 old_transform;
	std::unique_ptr<Object> bound_script_obj;

	void computeLocalTransform();
	void applyTransform(Batch& batch);
	void resetTransform(Batch& batch);
	void populateDebugUI(Batch& batch);
};

} // namespace Growl
