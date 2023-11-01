#pragma once
#include "items.h"

class CArchipelago : public CBaseEntity
{
	public:
		CArchipelago();
		~CArchipelago();
		void Spawn() override;
		void Precache() override;
		void APStartup();
		void APLogic();
		void ItemsClear();
		void ItemRecv(int itemid, bool notify);
		void Checked(int location);
		
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