#pragma once

#include "growl/scene/node.h"
#include "growl/ui/pack.h"

namespace Growl {

class Menu;

class Widget : public Node {

	friend class Menu;

public:
	Widget(std::string&& name);

	virtual Node* addChild(std::unique_ptr<Node> node) override;
	virtual Node* addChild(Node* node) override;

	template <class T, class... Args>
	typename std::enable_if<std::is_base_of<Node, T>::value, Packer<T>>::type
	addWithLayout(Args&&... args) {
		Node* n = addChild(std::make_unique<T>(std::forward<Args>(args)...));
		return Packer{
			static_cast<T*>(n), &(pack_info.at(pack_info.size() - 1))};
	}

	template <class T>
	typename std::enable_if<std::is_base_of<Node, T>::value, Packer<T>>::type
	addWithLayout(std::unique_ptr<T> child) {
		Node* n = addChild(std::move(child));
		return Packer{
			static_cast<T*>(n), &(pack_info.at(pack_info.size() - 1))};
	}

	template <class T>
	typename std::enable_if<std::is_base_of<Node, T>::value, Packer<T>>::type
	addWithLayout(T* child) {
		Node* n = addChild(child);
		return Packer{
			static_cast<T*>(n), &(pack_info.at(pack_info.size() - 1))};
	}

	void clear() override;

	void invalidate();
	void invalidateHierarchy();
	void validate();

	virtual float getPrefWidth() {
		return 0;
	}

	virtual float getPrefHeight() {
		return 0;
	}

	virtual void setNavigationDirections(
		Widget* up, Widget* down, Widget* left = nullptr,
		Widget* right = nullptr) {
		this->up_nav = up;
		this->down_nav = down;
		this->left_nav = left;
		this->right_nav = right;
	}

protected:
	virtual void layout() = 0;
	virtual void
	onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) override;
	void removeChild(int i) override;

	virtual void setSelected(bool selected) {
		this->nav_selected = selected;
	}
	virtual bool isSelected() {
		return this->nav_selected;
	}

	bool invalidated = true;
	std::vector<PackInfo> pack_info;

private:
	bool nav_selected = false;
	Widget* up_nav = nullptr;
	Widget* down_nav = nullptr;
	Widget* left_nav = nullptr;
	Widget* right_nav = nullptr;
};

Widget* nodeAsWidget(Node* node);

} // namespace Growl
