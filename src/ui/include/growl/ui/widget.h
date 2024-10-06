#pragma once

#include "growl/scene/node.h"
#include "growl/ui/pack.h"

namespace Growl {

class Widget : public Node {
public:
	Widget(std::string&& name);

	virtual Node* addChild(std::unique_ptr<Node> node) override;

	template <class T, class... Args>
	Packer<T> addWithLayout(Args&&... args) {
		Node* n = addChild(std::make_unique<T>(std::forward<Args>(args)...));
		return Packer{
			static_cast<T*>(n), &(pack_info.at(pack_info.size() - 1))};
	}

	void invalidate();
	void invalidateHierarchy();
	void validate();

protected:
	virtual void layout() = 0;
	virtual void
	onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) override;

	bool invalidated = true;
	std::vector<PackInfo> pack_info;
};

Widget* nodeAsWidget(Node* node);

} // namespace Growl
