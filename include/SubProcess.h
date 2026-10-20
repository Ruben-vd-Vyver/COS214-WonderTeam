#include "CompositeNode.h"

class SubProcess : public CompositeNode {
	protected:
		std::string label() const override;
	public:
		SubProcess(std::string id, std::string name);
		bool isBlocked() const;
		void activate() override;
};
