/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

#include <CDetour/detours.h>
 
TFPayloadHookTest g_TFPayloadHookTest;		/**< Global singleton for extension's main interface */
SMEXT_LINK(&g_TFPayloadHookTest);

IGameConfig *g_pGameConf;

CDetour *dt_CTFGameRules_GetPayloadToPush;
CDetour *dt_CTFGameRules_GetPayloadToBlock;

DETOUR_DECL_MEMBER2(CTFGameRules_GetPayloadToPush, void, CBaseHandle*, handle, int, team) {
	rootconsole->ConsolePrint("%s(%d)", "called CTFGameRules::GetPayloadToPush", team);
	
	// crash here
	return DETOUR_MEMBER_CALL(CTFGameRules_GetPayloadToPush)(handle, team);
}

DETOUR_DECL_MEMBER2(CTFGameRules_GetPayloadToBlock, void, CBaseHandle*, handle, int, team) {
	rootconsole->ConsolePrint("%s(%d)", "called CTFGameRules::GetPayloadToBlock", team);
	
	return DETOUR_MEMBER_CALL(CTFGameRules_GetPayloadToBlock)(handle, team);
}

bool TFPayloadHookTest::SDK_OnLoad(char *error, size_t maxlen, bool late) {
	if (!gameconfs->LoadGameConfigFile("tf2.payload_hook_test", &g_pGameConf, error, maxlen)) {
		return false;
	}
	
	CDetourManager::Init(g_pSM->GetScriptingEngine(), g_pGameConf);
	
	dt_CTFGameRules_GetPayloadToBlock = DETOUR_CREATE_MEMBER(CTFGameRules_GetPayloadToBlock, "CTFGameRules::GetPayloadToBlock(int)");
	if (dt_CTFGameRules_GetPayloadToBlock == nullptr) {
		snprintf(error, maxlen, "%s", "Failed to load CTFGameRules::GetPayloadToBlock detour");
		return false;
	}
	
	dt_CTFGameRules_GetPayloadToPush = DETOUR_CREATE_MEMBER(CTFGameRules_GetPayloadToPush, "CTFGameRules::GetPayloadToPush(int)");
	if (dt_CTFGameRules_GetPayloadToPush == nullptr) {
		snprintf(error, maxlen, "%s", "Failed to load CTFGameRules::GetPayloadToPush detour");
		return false;
	}
	
	dt_CTFGameRules_GetPayloadToPush->EnableDetour();
	return true;
}

void TFPayloadHookTest::SDK_OnUnload() {
	dt_CTFGameRules_GetPayloadToPush->DisableDetour();
	gameconfs->CloseGameConfigFile(g_pGameConf);
}
