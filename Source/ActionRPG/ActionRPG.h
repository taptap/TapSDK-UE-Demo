// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// ----------------------------------------------------------------------------------------------------------------
// This header is included by all headers in the project so it's a good place to declare common includes
// We include EngineMinimal and the subset of engine headers used by most of our classes
// We don't want to include "Engine.h" as that pulls in too many classes we don't need and slows compile time
// ----------------------------------------------------------------------------------------------------------------

#include "EngineMinimal.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "Common/RPGTypes.h"

ACTIONRPG_API DECLARE_LOG_CATEGORY_EXTERN(LogActionRPG, Log, All);

#if UE_SERVER
#define CONNECT_ADDRESS TEXT("39.98.246.18:10103")
#else
#define CONNECT_ADDRESS TEXT("39.98.246.18:10104")
#endif