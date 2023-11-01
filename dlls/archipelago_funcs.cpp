#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "nodes.h"
#include "soundent.h"
#include "client.h"
#include "decals.h"
#include "skill.h"
#include "effects.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"
#include "teamplay_gamerules.h"
#include "trains.h"
#include "monsters.h"
#include "shake.h"
#include "game.h"
#include "pm_shared.h"
#include "hltv.h"
#include "UserMessages.h"
#include "ap_ents.h"

#include <limits>
#include <algorithm>
#include <chrono>
#include <thread>
#include <string>

#include "Archipelago.h"

static AP_RoomInfo APHL_Room;
bool ap_is_connected = false;
bool ap_initialized = false;
char ap_savename[100];

cvar_t ap_ip = {"ap_ip", "exampleip.gg:5555", FCVAR_SERVER};
cvar_t ap_game = {"ap_game", "idunno", FCVAR_SERVER};
cvar_t ap_slotname = {"ap_slotname", "Who am I?", FCVAR_SERVER};
cvar_t ap_password = {"ap_password", "", FCVAR_SERVER};

/*
////////////////////////////
	PURPOSE:
Manage AP functionality.
////////////////////////////
*/
LINK_ENTITY_TO_CLASS(apmanager, CArchipelago);

void CArchipelago::Spawn()
{
	if (CVAR_GET_FLOAT("sv_pausable") == 1)
		CVAR_SET_FLOAT("sv_pausable", 0);
	Precache();
	APStartup();
	SetThink(&CArchipelago::APLogic);
	pev->nextthink = gpGlobals->time + 0.1;
}

void CArchipelago::APStartup()
{
	std::string ipVal(ap_ip.value);
	std::string gameVal(ap_game.value);
	std::string slotVal(ap_slotname.value);
	std::string passVal(ap_password.value);
	
	AP_Init(ipVal, gameVal, slotVal, passVal);
		AP_SetDeathLinkSupported(true);
		AP_SetItemClearCallback(CArchipelago::ItemsClear);
		AP_SetItemRecvCallback(CArchipelago::ItemRecv);
		AP_SetLocationCheckedCallback(CArchipelago::Checked);
	AP_Start();
	
	auto start_time = std::chrono::steady_clock::now();
	while (true)
	{
		bool can_break = false;
		switch (AP_GetConnectionStatus())
		{
			case AP_ConnectionStatus::Authenticated:
			{
				ALERT(at_console, "AP: Connected!\n");
				AP_GetRoomInfo(&APHL_Room);
				
				ap_is_connected = true;
				sprintf(ap_savename, "AP_%s", APHL_Room.seed_name.c_str());
				//ap_savename = "AP_" + APHL_Room.seed_name;
				
				can_break = true;
				
				break;
			}
			case AP_ConnectionStatus::ConnectionRefused:
				return ALERT(at_console, "AP: Failed to connect...\n");
		}
		
		if (can_break)
			break;
		
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (std::chrono::steady_clock::now() - start_time > std::chrono::seconds(10))
			return ALERT(at_console, "AP: Failed to connect...\n");
		
	}
	
}

/*
////////////////////////////
	PURPOSE:
AP item functionality.
////////////////////////////
*/
LINK_ENTITY_TO_CLASS(item_ap, CArchipelagoPickup);

void CArchipelagoPickup::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_archi.mdl");
	CItem::Spawn();
}

void CArchipelagoPickup::Precache()
{
	PRECACHE_MODEL("models/w_archi.mdl");
	PRECACHE_SOUND("ap/item.wav");
}

bool CArchipelagoPickup::MyTouch(CBasePlayer* pPlayer)
{
	if (pPlayer->pev->deadflag != DEAD_NO)
		return false;
	
	AP_SendItem(pev->health);
	
	EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "ap/item.wav", 1, ATTN_NORM);
}