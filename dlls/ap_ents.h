#pragma once
#include "items.h"

extern cvar_t ap_ip;
extern cvar_t ap_game;
extern cvar_t ap_slotname;
extern cvar_t ap_password;

class CArchipelago : public CBaseEntity
{
	public:
		CArchipelago();
		~CArchipelago();
		void Spawn() override;
		void Precache() override;
		void APStartup();
		void APLogic();
/* 		void ItemsClear();
		void ItemRecv(int64_t itemid, bool notify);
		void Checked(int location); */
		
		void DizzyTrap();
		void BoomTrap();
		
	private:
		char MSGForm[256];
		
};

class CArchipelagoPickup : public CItem
{
	public:
		void Spawn() override;
		void Precache() override;
		
		bool MyTouch(CBasePlayer* pPlayer) override;
};