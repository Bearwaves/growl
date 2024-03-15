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

	if (auto err = node_cls->addConstructor<std::string_view>(
			[](ClassSelf* self, void* ctx,
			   const std::vector<ScriptingParam>& args)
				-> Result<ScriptingParam> {
				auto& node_name = std::get<std::string_view>(args.at(0));
				Node* n = new Node(std::string(node_name));
				self->setField("__ptr", n);
				self->setField("__ptr_owned", n);
				return ScriptingParam();
			},
			nullptr);
		err) {
		return err;
	}

	if (auto err = node_cls->addDestructor(
			[](ClassSelf* self, void* ctx,
			   const std::vector<ScriptingParam>& args)
				-> Result<ScriptingParam> {
				Node* n = static_cast<Node*>(
					const_cast<void*>(self->getField("__ptr_owned")));
				if (n) {
					delete n;
				}
				return ScriptingParam();
			},
			nullptr);
		err) {
		return err;
	}

	if (auto err = node_cls->addMethod<void, float>(
			"setX",
			[](ClassSelf* self, void* ctx,
			   const std::vector<ScriptingParam>& args)
				-> Result<ScriptingParam> {
				Node* n = static_cast<Node*>(
					const_cast<void*>(self->getField("__ptr")));
				n->setX(std::get<float>(args.at(0)), true);
				return ScriptingParam();
			},
			nullptr);
		err) {
		return err;
	}

	if (auto err = node_cls->addMethod<void, float>(
			"setY",
			[](ClassSelf* self, void* ctx,
			   const std::vector<ScriptingParam>& args)
				-> Result<ScriptingParam> {
				Node* n = static_cast<Node*>(
					const_cast<void*>(self->getField("__ptr")));
				n->setY(std::get<float>(args.at(0)), true);
				return ScriptingParam();
			},
			nullptr);
		err) {
		return err;
	}

	if (auto err = node_cls->addMethod<float>(
			"getX",
			[](ClassSelf* self, void* ctx,
			   const std::vector<ScriptingParam>& args)
				-> Result<ScriptingParam> {
				Node* n = static_cast<Node*>(
					const_cast<void*>(self->getField("__ptr")));
				return ScriptingParam(n->getX(true));
			},
			nullptr);
		err) {
		return err;
	}

	if (auto err = node_cls->addMethod<float>(
			"getY",
			[](ClassSelf* self, void* ctx,
			   const std::vector<ScriptingParam>& args)
				-> Result<ScriptingParam> {
				Node* n = static_cast<Node*>(
					const_cast<void*>(self->getField("__ptr")));
				return ScriptingParam(n->getY(true));
			},
			nullptr);
		err) {
		return err;
	}

	if (auto err = node_cls->addMethod<void, float>(
			"setWidth",
			[](ClassSelf* self, void* ctx,
			   const std::vector<ScriptingParam>& args)
				-> Result<ScriptingParam> {
				Node* n = static_cast<Node*>(
					const_cast<void*>(self->getField("__ptr")));
				n->setWidth(std::get<float>(args.at(0)), true);
				return ScriptingParam();
			},
			nullptr);
		err) {
		return err;
	}

	if (auto err = node_cls->addMethod<void, float>(
			"setHeight",
			[](ClassSelf* self, void* ctx,
			   const std::vector<ScriptingParam>& args)
				-> Result<ScriptingParam> {
				Node* n = static_cast<Node*>(
					const_cast<void*>(self->getField("__ptr")));
				n->setHeight(std::get<float>(args.at(0)), true);
				return ScriptingParam();
			},
			nullptr);
		err) {
		return err;
	}

	if (auto err = node_cls->addMethod<float>(
			"getWidth",
			[](ClassSelf* self, void* ctx,
			   const std::vector<ScriptingParam>& args)
				-> Result<ScriptingParam> {
				Node* n = static_cast<Node*>(
					const_cast<void*>(self->getField("__ptr")));
				return ScriptingParam(n->getWidth(true));
			},
			nullptr);
		err) {
		return err;
	}

	if (auto err = node_cls->addMethod<float>(
			"getHeight",
			[](ClassSelf* self, void* ctx,
			   const std::vector<ScriptingParam>& args)
				-> Result<ScriptingParam> {
				Node* n = static_cast<Node*>(
					const_cast<void*>(self->getField("__ptr")));
				return ScriptingParam(n->getHeight(true));
			},
			nullptr);
		err) {
		return err;
	}

	if (auto err = node_cls->addMethod<float>(
			"getRotation",
			[](ClassSelf* self, void* ctx,
			   const std::vector<ScriptingParam>& args)
				-> Result<ScriptingParam> {
				Node* n = static_cast<Node*>(
					const_cast<void*>(self->getField("__ptr")));
				return ScriptingParam(n->getRotation(true));
			},
			nullptr);
		err) {
		return err;
	}

	if (auto err = node_cls->addMethod<void, float>(
			"setRotation",
			[](ClassSelf* self, void* ctx,
			   const std::vector<ScriptingParam>& args)
				-> Result<ScriptingParam> {
				Node* n = static_cast<Node*>(
					const_cast<void*>(self->getField("__ptr")));
				n->setRotation(std::get<float>(args.at(0)), true);
				return ScriptingParam();
			},
			nullptr);
		err) {
		return err;
	}

	return nullptr;
}
