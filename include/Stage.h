#ifndef STAGE_H
#define STAGE_H

#include "CompositeNode.h"

class Stage : public CompositeNode {
	bool parallel_;
	
	protected:
		std::string label() const override;
	public:
		Stage(std::string id, std::string name, bool parallel = false);
		bool isParallel() const;
		void activate() override;
};

#endif // STAGE_H
