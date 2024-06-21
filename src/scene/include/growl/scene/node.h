#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "growl/core/api/api.h"
#include "growl/core/error.h"
#include "growl/core/input/processor.h"
#include "growl/core/scripting/script.h"
#include <memory>
#include <string>
#include <vector>

namespace Growl {

class Batch;

class Node : public InputProcessor {

	friend Error initSceneGraph(API& api);

public:
	Node(std::string&& label) {
		this->label = std::move(label);
	}

	virtual ~Node() = default;
	std::string& getLabel();

	GROWL_SCRIPT_VAR(float, X, x);
	GROWL_SCRIPT_VAR(float, Y, y);
	GROWL_SCRIPT_VAR(float, Width, w);
	GROWL_SCRIPT_VAR(float, Height, h);
	GROWL_SCRIPT_VAR(float, Rotation, rotation);

	Node* addChild(std::unique_ptr<Node> node);
	void draw(Batch& batch, float parent_alpha);
	void drawChildren(Batch& batch, float parent_alpha);

	virtual bool onEvent(const InputEvent& event) override;
	bool hit(float x, float y);

	Error bindScript(API& api, Script& script);

protected:
	Node* getParent() {
		return parent;
	}

	virtual void onPopulateDebugUI(Batch& batch) {}
	virtual void
	onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform);

	// Input
	bool onMouseEvent(const InputMouseEvent& event) override;
	bool onMouseEventRaw(const InputMouseEvent& event);

private:
	API* api;
	std::unique_ptr<Script> script;
	std::string label;
	Node* parent = nullptr;
	float x = 0;
	float y = 0;
	float w = 0;
	float h = 0;
	float rotation = 0;
	std::vector<std::unique_ptr<Node>> children;
	glm::mat4x4 local_transform;
	std::unique_ptr<Object> bound_script_obj = nullptr;

	void computeLocalTransform();
	void populateDebugUI(Batch& batch);
};

} // namespace Growl
