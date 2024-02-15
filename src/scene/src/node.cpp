#include "growl/scene/node.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "growl/core/error.h"
#include "growl/core/graphics/batch.h"
#include "growl/core/input/event.h"
#include "growl/core/scripting/script.h"
#include <string>
#ifdef GROWL_IMGUI
#include "imgui.h"
#endif

using Growl::Batch;
using Growl::Error;
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
	applyTransform(batch);
	if (!parent) {
		populateDebugUI(batch);
	}
	onDraw(batch, parent_alpha);
	resetTransform(batch);
}

void Node::onDraw(Batch& batch, float parent_alpha) {
	drawChildren(batch, parent_alpha);
}

void Node::drawChildren(Batch& batch, float parent_alpha) {
	for (auto& child : children) {
		child->draw(batch, parent_alpha);
	}
}

void Node::onEvent(InputEvent& event) {
	InputProcessor::onEvent(event);
	for (auto& child : children) {
		child->onEvent(event);
	}
}

bool Node::hit(float x, float y) {
	glm::vec4 internal_coordinates =
		glm::inverse(local_transform) * glm::vec4(x, y, 0, 1);
	return !(
		internal_coordinates.x < 0 || internal_coordinates.y < 0 ||
		internal_coordinates.x >= this->w || internal_coordinates.y >= this->h);
}

Error Node::bindScript(ScriptingAPI& api, Script& script) {
	scripting_api = &api;
	auto res = api.execute(script);
	if (!res) {
		return std::move(res.error());
	}
	auto obj = std::move(std::get<std::unique_ptr<Object>>(*res));
	if (auto err = api.setField(*obj, "__ptr", static_cast<void*>(this)); err) {
		return err;
	}
	if (auto err = api.setClass(*obj, "Node"); err) {
		return err;
	}
	bound_script_obj = std::move(obj);

	return nullptr;
}

void Node::computeLocalTransform() {
	auto translate = glm::translate(glm::identity<glm::mat4x4>(), {x, y, 0});
	auto rotation_origin =
		glm::translate(glm::identity<glm::mat4x4>(), {w / 2, h / 2, 0});
	auto rotate =
		glm::rotate(glm::identity<glm::mat4x4>(), rotation, {0, 0, 1});
	local_transform =
		translate * rotation_origin * rotate * glm::inverse(rotation_origin);
	if (parent) {
		local_transform = parent->local_transform * local_transform;
	}
}

void Node::applyTransform(Batch& batch) {
	old_transform = batch.getTransform();
	batch.setTransform(local_transform);
}

void Node::resetTransform(Batch& batch) {
	batch.setTransform(old_transform);
}
