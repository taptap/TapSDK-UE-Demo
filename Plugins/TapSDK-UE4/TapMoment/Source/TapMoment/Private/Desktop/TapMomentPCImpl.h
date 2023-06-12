#pragma once
#include "TapMomentImpl.h"

class FTapMomentPCImpl: public FTapMomentImpl {
public:
	virtual void Init(const FTapMomentConfig& InConfig) override;
	virtual void OpenWebTopic() override;

};
