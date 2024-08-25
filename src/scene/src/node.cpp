#include "growl/scene/node.h"
#include "glm/ext/matrix_transform.hpp"
#include "growl/core/api/api.h"
#include "growl/core/error.h"
#include "growl/core/graphics/batch.h"
#include "growl/core/input/event.h"
#include "growl/core/scripting/script.h"
#include "growl/scene/scene.h"
#include <string>
#ifdef GROWL_IMGUI
#include "glm/ext/scalar_constants.hpp"
#include "imgui.h"
#include "imgui_stdlib.h"
#endif

using Growl::API;
using Growl::Batch;
using Growl::Error;
using Growl::InputMouseEvent;
using Growl::Node;
using Growl::Script;

std::string& Node::getLabel() {
	return label;
}

Node* Node::addChild(std::unique_ptr<Node> node) {
	node->parent = this;
	children.emplace_back(std::move(node));
	return children.back().get();
}

void Node::populateDebugUI(Batch& batch) {
#ifdef GROWL_IMGUI
	imGuiBeginSceneWindow();
	if (ImGui::TreeNodeEx(getLabel().c_str(), ImGuiTreeNodeFlags_Framed)) {
		if (ImGui::TreeNodeEx("Node", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SliderFloat(
				"X", &x, 0.0f,
				parent ? parent->getWidth() : batch.getTargetWidth(), "%.2f");
			ImGui::SliderFloat(
				"Y", &y, 0.0f,
				parent ? parent->getHeight() : batch.getTargetHeight(), "%.2f");
			ImGui::SliderFloat(
				"Width", &w, 0.0f,
				parent ? parent->getWidth() : batch.getTargetWidth(), "%.2f");
			ImGui::SliderFloat(
				"Height", &h, 0.0f,
				parent ? parent->getHeight() : batch.getTargetHeight(), "%.2f");
			ImGui::SliderFloat(
				"Rotation", &rotation, 0.0f, 2 * glm::pi<float>(), "%.2f");

			onPopulateDebugUI(batch);
			ImGui::TreePop();
		}

		if (script && ImGui::TreeNode("Script")) {
			ImGui::InputTextMultiline(
				"##source", &(script->getSource()),
				ImVec2(-1, ImGui::GetTextLineHeight() * 16),
				ImGuiInputTextFlags_AllowTabInput);
			if (ImGui::Button("Save")) {
				if (auto err = bindScript(*api, *script)) {
					api->system().log(
						"Node::ImGui", "Failed to rebind script: {}",
						err->message());
				}
			}
			ImGui::TreePop();
		}

		if (!children.empty() && ImGui::TreeNode("Children")) {
			for (auto& child : children) {
				child->populateDebugUI(batch);
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
	imGuiEndSceneWindow();
#endif
}

void Node::tick(double delta_time) {
	if (!bound_script_obj) {
		return onTick(delta_time);
	}

	std::vector<ScriptingParam> params;
	params.push_back(delta_time);
	if (auto result = api->scripting().executeMethod<void, double>(
			bound_script_obj.get(), "onTick", params);
		!result) {
		api->system().log(
			LogLevel::Warn, "Node::onTick", "Failed to execute method: {}",
			result.error()->message());
	}
}

void Node::onTick(double delta_time) {
	for (auto& child : children) {
		child->tick(delta_time);
	}
}

void Node::draw(Batch& batch, float parent_alpha) {
	computeLocalTransform();
	if (!parent) {
		populateDebugUI(batch);
	}
	onDraw(batch, parent_alpha, local_transform);
}

void Node::onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) {
	for (auto& child : children) {
		child->draw(batch, parent_alpha);
	}
}

bool Node::onEvent(const InputEvent& event) {
	if (InputProcessor::onEvent(event)) {
		return true;
	}
	bool handled = false;
	for (auto& child : children) {
		handled |= child->onEvent(event);
	}
	return handled;
}

bool Node::hit(float x, float y) {
	glm::vec4 internal_coordinates =
		glm::inverse(local_transform) * glm::vec4(x, y, 0, 1);
	return !(
		internal_coordinates.x < 0 || internal_coordinates.y < 0 ||
		internal_coordinates.x >= this->w || internal_coordinates.y >= this->h);
}

Error Node::bindScript(API& api, Script& script) {
	this->api = &api;
	this->script = std::make_unique<Script>(script);
	auto res = api.scripting().execute(*this->script);
	if (!res) {
		return std::move(res.error());
	}
	auto obj = std::move(std::get<std::unique_ptr<ScriptingRef>>(*res));
	if (auto err = api.scripting().setField(
			obj.get(), "__ptr", static_cast<void*>(this))) {
		return err;
	}
	if (auto err = api.scripting().setClass(obj.get(), "Node")) {
		return err;
	}
	bound_script_obj = std::move(obj);

	return nullptr;
}

void Node::computeLocalTransform() {
	auto translate =
		glm::translate(glm::identity<glm::mat4x4>(), {getX(), getY(), 0});
	auto rotation_origin = glm::translate(
		glm::identity<glm::mat4x4>(), {getWidth() / 2, getHeight() / 2, 0});
	auto rotate =
		glm::rotate(glm::identity<glm::mat4x4>(), getRotation(), {0, 0, 1});
	local_transform =
		translate * rotation_origin * rotate * glm::inverse(rotation_origin);
	if (parent) {
		local_transform = parent->local_transform * local_transform;
	}
}

bool Node::onMouseEvent(const InputMouseEvent& event) {
	if (!bound_script_obj) {
		return onMouseEventRaw(event);
	}

	std::vector<ScriptingParam> ctor_args;
	ctor_args.push_back(&event);
	auto ctor_result =
		api->scripting().executeConstructor<const InputMouseEvent*>(
			"InputMouseEvent", ctor_args);
	if (!ctor_result) {
		api->system().log(
			LogLevel::Warn, "Node::onMouseEvent",
			"Failed to execute constructor: {}",
			ctor_result.error()->message());
		return false;
	}

	std::vector<ScriptingParam> v;
	v.push_back(std::move(ctor_result.get()));
	auto exec_res =
		api->scripting().executeMethod<bool, std::unique_ptr<ScriptingRef>>(
			bound_script_obj.get(), "onMouseEvent", v);
	if (!exec_res) {
		api->system().log(
			LogLevel::Warn, "Node::onMouseEvent",
			"Failed to execute method: {}", exec_res.error()->message());
		return false;
	}
	return std::get<bool>(*exec_res);
}

bool Node::onMouseEventRaw(const InputMouseEvent& event) {
	return InputProcessor::onMouseEvent(event);
}

bool Node::onKeyboardEvent(const InputKeyboardEvent& event) {
	if (!bound_script_obj) {
		return onKeyboardEventRaw(event);
	}

	std::vector<ScriptingParam> ctor_args;
	ctor_args.push_back(&event);
	auto ctor_result =
		api->scripting().executeConstructor<const InputKeyboardEvent*>(
			"InputKeyboardEvent", ctor_args);
	if (!ctor_result) {
		api->system().log(
			LogLevel::Warn, "Node::onKeyboardEvent",
			"Failed to execute constructor: {}",
			ctor_result.error()->message());
		return false;
	}

	std::vector<ScriptingParam> v;
	v.push_back(std::move(ctor_result.get()));
	auto exec_res =
		api->scripting().executeMethod<bool, std::unique_ptr<ScriptingRef>>(
			bound_script_obj.get(), "onKeyboardEvent", v);
	if (!exec_res) {
		api->system().log(
			LogLevel::Warn, "Node::onKeyboardEvent",
			"Failed to execute method: {}", exec_res.error()->message());
		return false;
	}
	return std::get<bool>(*exec_res);
}

bool Node::onKeyboardEventRaw(const InputKeyboardEvent& event) {
	return InputProcessor::onKeyboardEvent(event);
}

bool Node::onTouchEvent(const InputTouchEvent& event) {
	if (!bound_script_obj) {
		return onTouchEventRaw(event);
	}

	std::vector<ScriptingParam> ctor_args;
	ctor_args.push_back(&event);
	auto ctor_result =
		api->scripting().executeConstructor<const InputTouchEvent*>(
			"InputTouchEvent", ctor_args);
	if (!ctor_result) {
		api->system().log(
			LogLevel::Warn, "Node::onTouchEvent",
			"Failed to execute constructor: {}",
			ctor_result.error()->message());
		return false;
	}

	std::vector<ScriptingParam> v;
	v.push_back(std::move(ctor_result.get()));
	auto exec_res =
		api->scripting().executeMethod<bool, std::unique_ptr<ScriptingRef>>(
			bound_script_obj.get(), "onTouchEvent", v);
	if (!exec_res) {
		api->system().log(
			LogLevel::Warn, "Node::onTouchEvent",
			"Failed to execute method: {}", exec_res.error()->message());
		return false;
	}
	return std::get<bool>(*exec_res);
}

bool Node::onTouchEventRaw(const InputTouchEvent& event) {
	return InputProcessor::onTouchEvent(event);
}

bool Node::onControllerEvent(const InputControllerEvent& event) {
	if (!bound_script_obj) {
		return onControllerEventRaw(event);
	}

	std::vector<ScriptingParam> ctor_args;
	ctor_args.push_back(&event);
	auto ctor_result =
		api->scripting().executeConstructor<const InputControllerEvent*>(
			"InputControllerEvent", ctor_args);
	if (!ctor_result) {
		api->system().log(
			LogLevel::Warn, "Node::onControllerEvent",
			"Failed to execute constructor: {}",
			ctor_result.error()->message());
		return false;
	}

	std::vector<ScriptingParam> v;
	v.push_back(std::move(ctor_result.get()));
	auto exec_res =
		api->scripting().executeMethod<bool, std::unique_ptr<ScriptingRef>>(
			bound_script_obj.get(), "onControllerEvent", v);
	if (!exec_res) {
		api->system().log(
			LogLevel::Warn, "Node::onControllerEvent",
			"Failed to execute method: {}", exec_res.error()->message());
		return false;
	}
	return std::get<bool>(*exec_res);
}

bool Node::onControllerEventRaw(const InputControllerEvent& event) {
	return InputProcessor::onControllerEvent(event);
}
