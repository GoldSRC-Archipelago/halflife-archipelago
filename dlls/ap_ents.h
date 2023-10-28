#pragma once
#include "cbase.h"
#include "items.h"

class CArchipelago : public CBaseEntity
{
	public:
		void Spawn() override;
		void Precache() override;
		void APStartup();
		void APLogic();
		void ItemsClear();
		void ItemRecv(int itemid, bool notify);
		void Checked(int location);
		
		void DizzyTrap();
		void BoomTrap();
		
};

class CArchipelagoPickup : public CItem
{
	public:
		void Spawn() override;
		void Precache() override;
		
		bool MyTouch() override;
};