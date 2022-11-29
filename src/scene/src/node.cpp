#include "growl/scene/node.h"
#include "glm/ext/matrix_transform.hpp"
#include "growl/core/graphics/batch.h"
#include "growl/core/input/event.h"
#include <cmath>

using Growl::Batch;
using Growl::Node;

float Node::getX() {
	return x;
}

float Node::getY() {
	return y;
}

float Node::getWidth() {
	return w;
}

float Node::getHeight() {
	return h;
}

float Node::getRotation() {
	return rotation;
}

void Node::setX(float x) {
	this->x = x;
}

void Node::setY(float y) {
	this->y = y;
}

void Node::setWidth(float w) {
	this->w = w;
}

void Node::setHeight(float h) {
	this->h = h;
}

void Node::setRotation(float rads) {
	this->rotation = rads;
}

Node* Node::addChild(std::unique_ptr<Node> node) {
	node->parent = this;
	children.emplace_back(std::move(node));
	return children.back().get();
}

void Node::draw(Batch& batch, float parent_alpha) {
	computeLocalTransform();
	drawChildren(batch, parent_alpha);
}

void Node::drawChildren(Batch& batch, float parent_alpha) {
	applyTransform(batch);
	for (auto& child : children) {
		child->draw(batch, parent_alpha);
	}
	resetTransform(batch);
}

void Node::onMouseEvent(InputMouseEvent& event) {
	for (auto& child : children) {
		child->onMouseEvent(event);
	}
}

bool Node::hit(float x, float y) {
	glm::vec4 internal_coordinates = glm::vec4(x, y, 0, 1);
	if (parent) {
		internal_coordinates =
			glm::inverse(parent->local_transform) * internal_coordinates;
	}
	return !(
		internal_coordinates.x < this->x || internal_coordinates.y < this->y ||
		internal_coordinates.x >= this->x + this->w ||
		internal_coordinates.y >= this->y + this->h);
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
