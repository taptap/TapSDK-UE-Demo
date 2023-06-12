#pragma once

#include "Modules/ModuleManager.h"

#define TapUEMoment_VERSION_NUMBER "31803001"
#define TapUEMoment_VERSION "3.18.3"

class FTapMomentModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


};
