#include "growl/scene/node.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "growl/core/api/api.h"
#include "growl/core/error.h"
#include "growl/core/graphics/batch.h"
#include "growl/core/input/event.h"
#include "growl/core/scripting/script.h"
#include <string>
#ifdef GROWL_IMGUI
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
	if (ImGui::TreeNodeEx(
			getLabel().c_str(),
			parent ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Framed)) {
		ImGui::SliderFloat(
			"X", &x, 0.0f, parent ? parent->getWidth() : batch.getTargetWidth(),
			"%.2f");
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
		if (script && ImGui::TreeNode("Script")) {
			ImGui::InputTextMultiline("##source", &(script->getSource()));
			if (ImGui::Button("Save")) {
				if (auto err = bindScript(*api, *script)) {
					api->system().log(
						"Node::ImGui", "Failed to rebind script: {}",
						err->message());
				}
			}
			ImGui::TreePop();
		}

		onPopulateDebugUI(batch);

		for (auto& child : children) {
			child->populateDebugUI(batch);
		}

		ImGui::TreePop();
	}
#endif
}

void Node::draw(Batch& batch, float parent_alpha) {
	computeLocalTransform();
	if (!parent) {
		populateDebugUI(batch);
	}
	onDraw(batch, parent_alpha, local_transform);
}

void Node::onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) {
	drawChildren(batch, parent_alpha);
}

void Node::drawChildren(Batch& batch, float parent_alpha) {
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
	auto obj = std::move(std::get<std::unique_ptr<ScriptingObject>>(*res));
	if (auto err =
			api.scripting().setField(*obj, "__ptr", static_cast<void*>(this))) {
		return err;
	}
	if (auto err = api.scripting().setClass(*obj, "Node")) {
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
			"Failed to execute constructor: " + ctor_result.error()->message());
		return false;
	}

	std::vector<ScriptingParam> v;
	v.push_back(ctor_result.get().get());
	auto exec_res = api->scripting().executeMethod<bool, ScriptingObject*>(
		*bound_script_obj, "onMouseEvent", v);
	if (!exec_res) {
		api->system().log(
			LogLevel::Warn, "Node::onMouseEvent",
			"Failed to execute method: " + exec_res.error()->message());
		return false;
	}
	return std::get<bool>(*exec_res);
}

bool Node::onMouseEventRaw(const InputMouseEvent& event) {
	return InputProcessor::onMouseEvent(event);
}
