#ifndef SUBPROCESS_H
#define SUBPROCESS_H

#include "CompositeNode.h"

class SubProcess : public CompositeNode {
	int sourceVersion_;
	protected:
		std::string label() const override;
	public:
		SubProcess(std::string id, std::string name, int sourceVersion = 1);
		int getSourceVersion() const;
		bool isBlocked() const;
		void activate() override;
};

#endif // SUBPROCESS_H
