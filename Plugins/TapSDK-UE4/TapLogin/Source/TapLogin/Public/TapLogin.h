// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#define TapUELogin_VERSION_NUMBER "31804001"
#define TapUELogin_VERSION "3.18.4"

class FTapLoginModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


};
