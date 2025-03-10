#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "growl/core/api/api.h"
#include "growl/core/entity.h"
#include "growl/core/error.h"
#include "growl/core/graphics/color.h"
#include "growl/core/input/processor.h"
#include "growl/core/scripting/script.h"
#include "growl/scene/debug.h"
#include <memory>
#include <string>
#include <vector>

namespace Growl {

class Batch;

class Node : public InputProcessor, public Entity {

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

	virtual Node* addChild(std::unique_ptr<Node> node);

	template <class T>
	typename std::enable_if<
		std::is_base_of<Node, T>::value, std::unique_ptr<T>>::type
	removeNode(T* search) {
		for (int i = 0; i < children.size(); i++) {
			if (children.at(i).get() == search) {
				std::unique_ptr<Node> removed = std::move(children.at(i));
				removeChild(i);
				return std::unique_ptr<T>(static_cast<T*>(removed.release()));
				;
			}
			if (auto removed = children.at(i)->removeNode(search)) {
				return removed;
			}
		}
		return nullptr;
	}

	virtual void clear();

	void tick(double delta_time);
	void draw(Batch& batch, float parent_alpha);

	void setColor(Color color) {
		this->color = color;
	}

	virtual bool onEvent(const InputEvent& event) override;
	virtual bool onPostEvent(const InputEvent& event, bool children_handled);
	bool hit(float x, float y);
	bool hit(glm::vec4& local_coords);
	glm::vec4 worldToLocalCoordinates(float x, float y);

	Error bindScript(API& api, Script& script);

	void setDebugRendering(DebugRendering debug);

	void setClickListener(std::function<bool(float, float)> listener);

	// A higher Z priority means the node will be drawn later in the order
	// of children.
	void setZPriority(float z);

protected:
	Node* getParent() {
		return parent;
	}
	std::vector<std::unique_ptr<Node>>& getChildren() {
		return children;
	}
	virtual void removeChild(int i);

	virtual void onTick(double delta_time);
	virtual void onPopulateDebugUI(Batch& batch) {}
	virtual void
	onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform);

	// Input
	virtual bool onMouseEvent(const InputMouseEvent& event) override;
	bool onMouseEventRaw(const InputMouseEvent& event);
	virtual bool onKeyboardEvent(const InputKeyboardEvent& event) override;
	bool onKeyboardEventRaw(const InputKeyboardEvent& event);
	virtual bool onTouchEvent(const InputTouchEvent& event) override;
	bool onTouchEventRaw(const InputTouchEvent& event);
	virtual bool onControllerEvent(const InputControllerEvent& event) override;
	bool onControllerEventRaw(const InputControllerEvent& event);

	bool clickListenerDown() {
		return click_listener_down;
	}
	std::function<bool(float x, float y)>& getClickListener() {
		return click_listener;
	}

	void cancelEvent();

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
	float z = 0;
	std::vector<std::unique_ptr<Node>> children;
	std::vector<Node*> children_z_order;
	glm::mat4x4 local_transform;
	std::unique_ptr<ScriptingRef> bound_script_obj = nullptr;
	DebugRendering debug_rendering = DebugRendering::OFF;
	bool debug_mouseover = false;
	int depth = 0;
	std::optional<Color> color;
	std::function<bool(float x, float y)> click_listener;
	bool click_listener_down = false;
	bool event_cancelled = false;

	void computeLocalTransform();
	void drawChildren(Batch& batch, float parent_alpha);
	void populateDebugUI(Batch& batch);
	void setDepth(int depth);
	bool processClick(float x, float y, PointerEventType type);
	void reorderZ();
};

} // namespace Growl
