#include "growl/scene/node.h"
#include "glm/ext/matrix_transform.hpp"
#include "growl/core/graphics/batch.h"
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
	computeLocalTransform();
}

void Node::setY(float y) {
	this->y = y;
	computeLocalTransform();
}

void Node::setWidth(float w) {
	this->w = w;
	computeLocalTransform();
}

void Node::setHeight(float h) {
	this->h = h;
	computeLocalTransform();
}

void Node::setRotation(float rads) {
	this->rotation = rads;
	computeLocalTransform();
}

void Node::addChild(std::unique_ptr<Node> node) {
	children.emplace_back(std::move(node));
}

void Node::draw(Batch& batch, float parent_alpha) {
	drawChildren(batch, parent_alpha);
}

void Node::drawChildren(Batch& batch, float parent_alpha) {
	applyTransform(batch);
	for (auto& child : children) {
		child->draw(batch, parent_alpha);
	}
	resetTransform(batch);
}

void Node::computeLocalTransform() {
	auto translate = glm::translate(glm::identity<glm::mat4x4>(), {x, y, 0});

	auto rotation_origin =
		glm::translate(glm::identity<glm::mat4x4>(), {w / 2, h / 2, 0});
	auto rotate =
		glm::rotate(glm::identity<glm::mat4x4>(), rotation, {0, 0, 1});
	local_transform =
		translate * rotation_origin * rotate * glm::inverse(rotation_origin);
}

void Node::applyTransform(Batch& batch) {
	old_transform = batch.getTransform();
	batch.setTransform(old_transform * local_transform);
}

void Node::resetTransform(Batch& batch) {
	batch.setTransform(old_transform);
}