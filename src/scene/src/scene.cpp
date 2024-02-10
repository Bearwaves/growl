#include "growl/scene/scene.h"
#include "growl/core/api/api.h"
#include "growl/core/error.h"
#include "growl/core/scripting/class.h"
#include "growl/scene/node.h"

using Growl::API;
using Growl::Class;
using Growl::ClassSelf;
using Growl::Error;
using Growl::Node;

Error Growl::initSceneGraph(API& api) {
	auto node_cls_result = api.scripting().createClass("Node", false);
	if (!node_cls_result) {
		return std::move(node_cls_result.error());
	}
	auto& node_cls = *node_cls_result;
	node_cls->addConstructor<std::string_view>(
		[](ClassSelf* self, void* ctx,
		   const std::vector<std::any>& args) -> Result<std::any> {
			auto& node_name = std::any_cast<const std::string&>(args.at(0));
			Node* n = new Node(node_name.c_str());
			self->setField("__ptr", n);
			return std::any();
		},
		nullptr);

	node_cls->addDestructor(
		[](ClassSelf* self, void* ctx,
		   const std::vector<std::any>& args) -> Result<std::any> {
			Node* n =
				static_cast<Node*>(const_cast<void*>(self->getField("__ptr")));
			delete n;
			return std::any();
		},
		nullptr);

	node_cls->addMethod<void, float>(
		"setX",
		[](ClassSelf* self, void* ctx,
		   const std::vector<std::any>& args) -> Result<std::any> {
			Node* n =
				static_cast<Node*>(const_cast<void*>(self->getField("__ptr")));
			n->setX(std::any_cast<float>(args.at(0)));
			return std::any();
		},
		nullptr);

	node_cls->addMethod<void, float>(
		"setY",
		[](ClassSelf* self, void* ctx,
		   const std::vector<std::any>& args) -> Result<std::any> {
			Node* n =
				static_cast<Node*>(const_cast<void*>(self->getField("__ptr")));
			n->setY(std::any_cast<float>(args.at(0)));
			return std::any();
		},
		nullptr);

	node_cls->addMethod<float>(
		"getX",
		[](ClassSelf* self, void* ctx,
		   const std::vector<std::any>& args) -> Result<std::any> {
			Node* n =
				static_cast<Node*>(const_cast<void*>(self->getField("__ptr")));
			return std::any(n->getX());
		},
		nullptr);

	node_cls->addMethod<float>(
		"getY",
		[](ClassSelf* self, void* ctx,
		   const std::vector<std::any>& args) -> Result<std::any> {
			Node* n =
				static_cast<Node*>(const_cast<void*>(self->getField("__ptr")));
			return std::any(n->getY());
		},
		nullptr);

	return nullptr;
}
