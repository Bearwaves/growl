#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include <memory>
#include <vector>

namespace Growl {

class Batch;

class Node {
public:
	float getX();
	float getY();
	float getWidth();
	float getHeight();
	float getRotation();
	void setX(float x);
	void setY(float y);
	void setWidth(float w);
	void setHeight(float h);
	void setRotation(float rads);

	void addChild(std::unique_ptr<Node> node);
	void drawChildren(Batch& batch, float parent_alpha);
	virtual void draw(Batch& batch, float parent_alpha);

private:
	float x;
	float y;
	float w;
	float h;
	float rotation;
	std::vector<std::unique_ptr<Node>> children;
	glm::mat4x4 local_transform;
	glm::mat4x4 old_transform;

	void computeLocalTransform();
	void applyTransform(Batch& batch);
	void resetTransform(Batch& batch);
};

} // namespace Growl
