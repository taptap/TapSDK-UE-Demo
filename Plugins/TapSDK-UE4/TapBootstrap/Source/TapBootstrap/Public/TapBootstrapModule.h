// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

#define TapUEBootstrap_VERSION_NUMBER "31804001"
#define TapUEBootstrap_VERSION "3.18.4"


class TAPBOOTSTRAP_API FTapBootstrapModule : public IModuleInterface
{
public:
	
protected:
    virtual void StartupModule() override;
    
    virtual void ShutdownModule() override;

};

