/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#pragma once

#include "effects.h"
#include "weaponinfo.h"

#include "itemdef.h"

class CBasePlayer;
class CBasePlayerWeapon;

void DeactivateSatchels(CBasePlayer* pOwner);

// Contact Grenade / Timed grenade / Satchel Charge
class CGrenade : public CBaseMonster
{
public:
	void Spawn() override;

	typedef enum
	{
		SATCHEL_DETONATE = 0,
		SATCHEL_RELEASE
	} SATCHELCODE;

	static CGrenade* ShootTimed(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity, float time);
	static CGrenade* ShootContact(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity);
	static CGrenade* ShootSatchelCharge(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity);
	static void UseSatchelCharges(entvars_t* pevOwner, SATCHELCODE code);

	void Explode(Vector vecSrc, Vector vecAim);
	void Explode(TraceResult* pTrace, int bitsDamageType);
	void EXPORT Smoke();

	void EXPORT BounceTouch(CBaseEntity* pOther);
	void EXPORT SlideTouch(CBaseEntity* pOther);
	void EXPORT ExplodeTouch(CBaseEntity* pOther);
	void EXPORT DangerSoundThink();
	void EXPORT PreDetonate();
	void EXPORT Detonate();
	void EXPORT DetonateUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	void EXPORT TumbleThink();

	virtual void BounceSound();
	int BloodColor() override { return DONT_BLEED; }
	void Killed(entvars_t* pevAttacker, int iGib) override;

	bool m_fRegisteredSound; // whether or not this grenade has issued its DANGER sound to the world sound list yet.
};


#define ITEM_FLAG_SELECTONEMPTY 1
#define ITEM_FLAG_NOAUTORELOAD 2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY 4
#define ITEM_FLAG_LIMITINWORLD 8
#define ITEM_FLAG_EXHAUSTIBLE 16 // A player can totally exhaust their ammo supply and lose this weapon

#define WEAPON_IS_ONTARGET 0x40

typedef struct
{
	int iSlot;
	int iPosition;
	const char* pszAmmo1; // ammo 1 type
	int iMaxAmmo1;		  // max ammo 1
	const char* pszAmmo2; // ammo 2 type
	int iMaxAmmo2;		  // max ammo 2
	const char* pszName;
	int iMaxClip;
	int iId;
	int iFlags;
	int iWeight; // this value used to determine this weapon's importance in autoselection.
} ItemInfo;

struct AmmoInfo
{
	const char* pszName;
	int iId;

	/**
	*	@brief For exhaustible weapons. If provided, and the player does not have this weapon in their inventory yet it will be given to them.
	*/
	const char* WeaponName = nullptr;
};

inline int giAmmoIndex = 0;

void AddAmmoNameToAmmoRegistry(const char* szAmmoname, const char* weaponName);

// Items that the player has in their inventory that they can use
class CBasePlayerItem : public CBaseAnimating
{
public:
	void SetObjectCollisionBox() override;

	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;

	static TYPEDESCRIPTION m_SaveData[];

	virtual bool CanAddToPlayer(CBasePlayer* player) { return true; } // return true if the item you want the item added to the player inventory

	virtual void AddToPlayer(CBasePlayer* pPlayer);
	virtual bool AddDuplicate(CBasePlayerItem* pItem) { return false; } // return true if you want your duplicate removed from world
	void EXPORT DestroyItem();
	void EXPORT DefaultTouch(CBaseEntity* pOther); // default weapon touch
	void EXPORT FallThink();					   // when an item is first spawned, this think is run to determine when the object has hit the ground.
	void EXPORT Materialize();					   // make a weapon visible and tangible
	void EXPORT AttemptToMaterialize();			   // the weapon desires to become visible and tangible, if the game rules allow for it
	CBaseEntity* Respawn() override;			   // copy a weapon
	void FallInit();
	void CheckRespawn();
	virtual bool GetItemInfo(ItemInfo* p) { return false; } // returns false if struct not filled out
	virtual bool CanDeploy() { return true; }
	virtual bool Deploy() // returns is deploy was successful
	{
		return true;
	}

	virtual bool CanHolster() { return true; } // can this weapon be put away right now?
	virtual void Holster();
	virtual void UpdateItemInfo() {}

	virtual void ItemPreFrame() {}	// called each frame by the player PreThink
	virtual void ItemPostFrame() {} // called each frame by the player PostThink

	virtual void Drop();
	virtual void Kill();
	virtual void AttachToPlayer(CBasePlayer* pPlayer);

	virtual int PrimaryAmmoIndex() { return -1; }
	virtual int SecondaryAmmoIndex() { return -1; }

	virtual bool UpdateClientData(CBasePlayer* pPlayer) { return false; }

	virtual CBasePlayerWeapon* GetWeaponPtr() { return NULL; }

	virtual void GetWeaponData(weapon_data_t& data) {}

	virtual void SetWeaponData(const weapon_data_t& data) {}

	virtual void DecrementTimers() {}

	static inline ItemInfo ItemInfoArray[MAX_WEAPONS];
	static inline AmmoInfo AmmoInfoArray[MAX_AMMO_SLOTS];

	CBasePlayer* m_pPlayer;
	CBasePlayerItem* m_pNext;
	int m_iId; // WEAPON_???

	virtual int iItemSlot() { return 0; } // return 0 to MAX_ITEMS_SLOTS, used in hud

	int iItemPosition() { return ItemInfoArray[m_iId].iPosition; }
	const char* pszAmmo1() { return ItemInfoArray[m_iId].pszAmmo1; }
	int iMaxAmmo1() { return ItemInfoArray[m_iId].iMaxAmmo1; }
	const char* pszAmmo2() { return ItemInfoArray[m_iId].pszAmmo2; }
	int iMaxAmmo2() { return ItemInfoArray[m_iId].iMaxAmmo2; }
	const char* pszName() { return ItemInfoArray[m_iId].pszName; }
	int iMaxClip() { return ItemInfoArray[m_iId].iMaxClip; }
	int iWeight() { return ItemInfoArray[m_iId].iWeight; }
	int iFlags() { return ItemInfoArray[m_iId].iFlags; }

	// int		m_iIdPrimary;										// Unique Id for primary ammo
	// int		m_iIdSecondary;										// Unique Id for secondary ammo

	//Hack so deploy animations work when weapon prediction is enabled.
	bool m_ForceSendAnimations = false;
};


// inventory items that
class CBasePlayerWeapon : public CBasePlayerItem
{
public:
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;

	static TYPEDESCRIPTION m_SaveData[];

	// generic weapon versions of CBasePlayerItem calls
	void AddToPlayer(CBasePlayer* pPlayer) override;
	bool AddDuplicate(CBasePlayerItem* pItem) override;

	virtual bool ExtractAmmo(CBasePlayerWeapon* pWeapon);	  //{ return true; }			// Return true if you can add ammo to yourself when picked up
	virtual bool ExtractClipAmmo(CBasePlayerWeapon* pWeapon); // { return true; }			// Return true if you can add ammo to yourself when picked up

	// generic "shared" ammo handlers
	bool AddPrimaryAmmo(CBasePlayerWeapon* origin, int iCount, char* szName, int iMaxClip, int iMaxCarry);
	bool AddSecondaryAmmo(int iCount, char* szName, int iMaxCarry);

	void UpdateItemInfo() override {} // updates HUD state

	bool m_iPlayEmptySound;
	bool m_fFireOnEmpty; // True when the gun is empty and the player is still holding down the
						 // attack key(s)
	virtual bool PlayEmptySound();
	virtual void ResetEmptySound();

	virtual void SendWeaponAnim(int iAnim, int body = 0);

	bool CanDeploy() override;
	virtual bool IsUseable();
	bool DefaultDeploy(const char* szViewModel, const char* szWeaponModel, int iAnim, const char* szAnimExt, int body = 0);
	bool DefaultReload(int iClipSize, int iAnim, float fDelay, int body = 0);

	void ItemPostFrame() override; // called each frame by the player PostThink
	// called by CBasePlayerWeapons ItemPostFrame()
	virtual void PrimaryAttack() {}						  // do "+ATTACK"
	virtual void SecondaryAttack() {}					  // do "+ATTACK2"
	virtual void Reload() {}							  // do "+RELOAD"
	virtual void WeaponIdle() {}						  // called when no buttons pressed
	bool UpdateClientData(CBasePlayer* pPlayer) override; // sends hud info to client dll, if things have changed
	void RetireWeapon();

	// Can't use virtual functions as think functions so this wrapper is needed.
	void EXPORT CallDoRetireWeapon()
	{
		DoRetireWeapon();
	}

	virtual void DoRetireWeapon();
	virtual bool ShouldWeaponIdle() { return false; }
	void Holster() override;
	virtual bool UseDecrement() { return false; }

	int PrimaryAmmoIndex() override;
	int SecondaryAmmoIndex() override;

	void PrintState();

	CBasePlayerWeapon* GetWeaponPtr() override { return this; }
	float GetNextAttackDelay(float delay);

	float m_flPumpTime;
	int m_fInSpecialReload;		   // Are we in the middle of a reload for the shotguns
	float m_flNextPrimaryAttack;   // soonest time ItemPostFrame will call PrimaryAttack
	float m_flNextSecondaryAttack; // soonest time ItemPostFrame will call SecondaryAttack
	float m_flTimeWeaponIdle;	   // soonest time ItemPostFrame will call WeaponIdle
	int m_iPrimaryAmmoType;		   // "primary" ammo index into players m_rgAmmo[]
	int m_iSecondaryAmmoType;	   // "secondary" ammo index into players m_rgAmmo[]
	int m_iClip;				   // number of shots left in the primary weapon clip, -1 it not used
	int m_iClientClip;			   // the last version of m_iClip sent to hud dll
	int m_iClientWeaponState;	   // the last version of the weapon state sent to hud dll (is current weapon, is on target)
	bool m_fInReload;			   // Are we in the middle of a reload;

	int m_iDefaultAmmo; // how much ammo you get when you pick up this weapon as placed by a level designer.

	// hle time creep vars
	float m_flPrevPrimaryAttack;
	float m_flLastFireTime;
};


class CBasePlayerAmmo : public CBaseEntity
{
public:
	void Spawn() override;
	void EXPORT DefaultTouch(CBaseEntity* pOther); // default weapon touch
	virtual bool AddAmmo(CBaseEntity* pOther) { return true; }

	CBaseEntity* Respawn() override;
	void EXPORT Materialize();
};


inline DLL_GLOBAL short g_sModelIndexLaser; // holds the index for the laser beam
constexpr DLL_GLOBAL const char* g_pModelNameLaser = "sprites/laserbeam.spr";

inline DLL_GLOBAL short g_sModelIndexLaserDot;	 // holds the index for the laser beam dot
inline DLL_GLOBAL short g_sModelIndexFireball;	 // holds the index for the fireball
inline DLL_GLOBAL short g_sModelIndexSmoke;		 // holds the index for the smoke cloud
inline DLL_GLOBAL short g_sModelIndexWExplosion; // holds the index for the underwater explosion
inline DLL_GLOBAL short g_sModelIndexBubbles;	 // holds the index for the bubbles model
inline DLL_GLOBAL short g_sModelIndexBloodDrop;	 // holds the sprite index for blood drops
inline DLL_GLOBAL short g_sModelIndexBloodSpray; // holds the sprite index for blood spray (bigger)

extern void ClearMultiDamage();
extern void ApplyMultiDamage(entvars_t* pevInflictor, entvars_t* pevAttacker);
extern void AddMultiDamage(entvars_t* pevInflictor, CBaseEntity* pEntity, float flDamage, int bitsDamageType);

extern void DecalGunshot(TraceResult* pTrace, int iBulletType);
extern void SpawnBlood(Vector vecSpot, int bloodColor, float flDamage);
extern int DamageDecal(CBaseEntity* pEntity, int bitsDamageType);
extern void RadiusDamage(Vector vecSrc, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType);

typedef struct
{
	CBaseEntity* pEntity;
	float amount;
	int type;
} MULTIDAMAGE;

inline MULTIDAMAGE gMultiDamage;


#define LOUD_GUN_VOLUME 1000
#define NORMAL_GUN_VOLUME 600
#define QUIET_GUN_VOLUME 200

#define BRIGHT_GUN_FLASH 512
#define NORMAL_GUN_FLASH 256
#define DIM_GUN_FLASH 128

#define BIG_EXPLOSION_VOLUME 2048
#define NORMAL_EXPLOSION_VOLUME 1024
#define SMALL_EXPLOSION_VOLUME 512

#define WEAPON_ACTIVITY_VOLUME 64

#define VECTOR_CONE_1DEGREES Vector(0.00873, 0.00873, 0.00873)
#define VECTOR_CONE_2DEGREES Vector(0.01745, 0.01745, 0.01745)
#define VECTOR_CONE_3DEGREES Vector(0.02618, 0.02618, 0.02618)
#define VECTOR_CONE_4DEGREES Vector(0.03490, 0.03490, 0.03490)
#define VECTOR_CONE_5DEGREES Vector(0.04362, 0.04362, 0.04362)
#define VECTOR_CONE_6DEGREES Vector(0.05234, 0.05234, 0.05234)
#define VECTOR_CONE_7DEGREES Vector(0.06105, 0.06105, 0.06105)
#define VECTOR_CONE_8DEGREES Vector(0.06976, 0.06976, 0.06976)
#define VECTOR_CONE_9DEGREES Vector(0.07846, 0.07846, 0.07846)
#define VECTOR_CONE_10DEGREES Vector(0.08716, 0.08716, 0.08716)
#define VECTOR_CONE_15DEGREES Vector(0.13053, 0.13053, 0.13053)
#define VECTOR_CONE_20DEGREES Vector(0.17365, 0.17365, 0.17365)

//=========================================================
// CWeaponBox - a single entity that can store weapons
// and ammo.
//=========================================================
class CWeaponBox : public CBaseEntity
{
	void Precache() override;
	void Spawn() override;
	void Touch(CBaseEntity* pOther) override;
	bool KeyValue(KeyValueData* pkvd) override;
	bool IsEmpty();
	int GiveAmmo(int iCount, const char* szName, int iMax, int* pIndex = NULL);
	void SetObjectCollisionBox() override;

public:
	void EXPORT Kill();
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];

	bool HasWeapon(CBasePlayerItem* pCheckItem);
	bool PackWeapon(CBasePlayerItem* pWeapon);
	bool PackAmmo(int iszName, int iCount);

	CBasePlayerItem* m_rgpPlayerItems[MAX_ITEM_TYPES]; // one slot for each

	int m_rgiszAmmo[MAX_AMMO_SLOTS]; // ammo names
	int m_rgAmmo[MAX_AMMO_SLOTS];	 // ammo quantities

	int m_cAmmoTypes; // how many ammo types packed into this box (if packed by a level designer)
};

#ifdef CLIENT_DLL
bool bIsMultiplayer();
void LoadVModel(const char* szViewModel, CBasePlayer* m_pPlayer);
#endif

class CGlock : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int iItemSlot() override { return 2; }
	bool GetItemInfo(ItemInfo* p) override;

	void PrimaryAttack() override;
	void SecondaryAttack() override;
	void GlockFire(float flSpread, float flCycleTime, bool fUseAutoAim);
	bool Deploy() override;
	void Reload() override;
	void WeaponIdle() override;

	bool UseDecrement() override
	{
#if defined(CLIENT_WEAPONS)
		return true;
#else
		return false;
#endif
	}

private:
	int m_iShell;


	unsigned short m_usFireGlock1;
	unsigned short m_usFireGlock2;
};

class CCrowbar : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int iItemSlot() override { return 1; }
	void EXPORT SwingAgain();
	void EXPORT Smack();
	bool GetItemInfo(ItemInfo* p) override;

	void PrimaryAttack() override;
	bool Swing(bool fFirst);
	bool Deploy() override;
	void Holster() override;
	int m_iSwing;
	TraceResult m_trHit;

	bool UseDecrement() override
	{
#if defined(CLIENT_WEAPONS)
		return true;
#else
		return false;
#endif
	}

private:
	unsigned short m_usCrowbar;
};

class CPython : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int iItemSlot() override { return 2; }
	bool GetItemInfo(ItemInfo* p) override;
	void PrimaryAttack() override;
	void SecondaryAttack() override;
	bool Deploy() override;
	void Holster() override;
	void Reload() override;
	void WeaponIdle() override;

	bool UseDecrement() override
	{
#if defined(CLIENT_WEAPONS)
		return true;
#else
		return false;
#endif
	}

private:
	unsigned short m_usFirePython;
};

class CMP5 : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int iItemSlot() override { return 3; }
	bool GetItemInfo(ItemInfo* p) override;

	void PrimaryAttack() override;
	void SecondaryAttack() override;
	bool Deploy() override;
	void Reload() override;
	void WeaponIdle() override;
	float m_flNextAnimTime;
	int m_iShell;

	bool UseDecrement() override
	{
#if defined(CLIENT_WEAPONS)
		return true;
#else
		return false;
#endif
	}

private:
	unsigned short m_usMP5;
	unsigned short m_usMP52;
};

class CCrossbow : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int iItemSlot() override { return 3; }
	bool GetItemInfo(ItemInfo* p) override;

	void FireBolt();
	void FireSniperBolt();
	void PrimaryAttack() override;
	void SecondaryAttack() override;
	bool Deploy() override;
	void Holster() override;
	void Reload() override;
	void WeaponIdle() override;

	bool UseDecrement() override
	{
#if defined(CLIENT_WEAPONS)
		return true;
#else
		return false;
#endif
	}

private:
	unsigned short m_usCrossbow;
	unsigned short m_usCrossbow2;
};

class CShotgun : public CBasePlayerWeapon
{
public:
#ifndef CLIENT_DLL
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];
#endif


	void Spawn() override;
	void Precache() override;
	int iItemSlot() override { return 3; }
	bool GetItemInfo(ItemInfo* p) override;

	void PrimaryAttack() override;
	void SecondaryAttack() override;
	bool Deploy() override;
	void Reload() override;
	void WeaponIdle() override;
	void ItemPostFrame() override;
	int m_fInReload; //TODO: not used, remove
	float m_flNextReload;
	int m_iShell;

	bool UseDecrement() override
	{
#if defined(CLIENT_WEAPONS)
		return true;
#else
		return false;
#endif
	}

private:
	unsigned short m_usDoubleFire;
	unsigned short m_usSingleFire;
};

class CLaserSpot : public CBaseEntity
{
	void Spawn() override;
	void Precache() override;

	int ObjectCaps() override { return FCAP_DONT_SAVE; }

public:
	void Suspend(float flSuspendTime);
	void EXPORT Revive();

	static CLaserSpot* CreateSpot();
};

class CRpg : public CBasePlayerWeapon
{
public:
#ifndef CLIENT_DLL
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];
#endif

	void Spawn() override;
	void Precache() override;
	void Reload() override;
	int iItemSlot() override { return 4; }
	bool GetItemInfo(ItemInfo* p) override;

	bool Deploy() override;
	bool CanHolster() override;
	void Holster() override;

	void PrimaryAttack() override;
	void SecondaryAttack() override;
	void WeaponIdle() override;

	void UpdateSpot();
	bool ShouldWeaponIdle() override { return true; }

	CLaserSpot* m_pSpot;
	bool m_fSpotActive;
	int m_cActiveRockets; // how many missiles in flight from this launcher right now?

	bool UseDecrement() override
	{
#if defined(CLIENT_WEAPONS)
		return true;
#else
		return false;
#endif
	}

private:
	unsigned short m_usRpg;
};

class CRpgRocket : public CGrenade
{
public:
	~CRpgRocket() override;

	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];
	void Spawn() override;
	void Precache() override;
	void EXPORT FollowThink();
	void EXPORT IgniteThink();
	void EXPORT RocketTouch(CBaseEntity* pOther);
	static CRpgRocket* CreateRpgRocket(Vector vecOrigin, Vector vecAngles, CBaseEntity* pOwner, CRpg* pLauncher);

	int m_iTrail;
	float m_flIgniteTime;
	EHANDLE m_pLauncher; // handle back to the launcher that fired me.
};

#define GAUSS_PRIMARY_CHARGE_VOLUME 256 // how loud gauss is while charging
#define GAUSS_PRIMARY_FIRE_VOLUME 450	// how loud gauss is when discharged

class CGauss : public CBasePlayerWeapon
{
public:
#ifndef CLIENT_DLL
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];
#endif

	void Spawn() override;
	void Precache() override;
	int iItemSlot() override { return 4; }
	bool GetItemInfo(ItemInfo* p) override;

	bool Deploy() override;
	void Holster() override;

	void PrimaryAttack() override;
	void SecondaryAttack() override;
	void WeaponIdle() override;

	void StartFire();
	void Fire(Vector vecOrigSrc, Vector vecDirShooting, float flDamage);
	float GetFullChargeTime();
	int m_iBalls;
	int m_iGlow;
	int m_iBeam;
	int m_iSoundState; // don't save this

	// was this weapon just fired primary or secondary?
	// we need to know so we can pick the right set of effects.
	bool m_fPrimaryFire;

	bool UseDecrement() override
	{
#if defined(CLIENT_WEAPONS)
		return true;
#else
		return false;
#endif
	}

private:
	void SendStopEvent(bool sendToHost);

private:
	unsigned short m_usGaussFire;
	unsigned short m_usGaussSpin;
};

enum EGON_FIRESTATE
{
	FIRE_OFF,
	FIRE_CHARGE
};

enum EGON_FIREMODE
{
	FIRE_NARROW,
	FIRE_WIDE
};

#define EGON_PRIMARY_VOLUME 450
#define EGON_BEAM_SPRITE "sprites/xbeam1.spr"
#define EGON_FLARE_SPRITE "sprites/XSpark1.spr"
#define EGON_SOUND_OFF "weapons/egon_off1.wav"
#define EGON_SOUND_RUN "weapons/egon_run3.wav"
#define EGON_SOUND_STARTUP "weapons/egon_windup2.wav"

class CEgon : public CBasePlayerWeapon
{
public:
#ifndef CLIENT_DLL
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];
#endif

	void Spawn() override;
	void Precache() override;
	int iItemSlot() override { return 4; }
	bool GetItemInfo(ItemInfo* p) override;

	bool Deploy() override;
	void Holster() override;

	void UpdateEffect(const Vector& startPoint, const Vector& endPoint, float timeBlend);

	void CreateEffect();
	void DestroyEffect();

	void EndAttack();
	void Attack();
	void PrimaryAttack() override;
	bool ShouldWeaponIdle() override { return true; }
	void WeaponIdle() override;

	float m_flAmmoUseTime; // since we use < 1 point of ammo per update, we subtract ammo on a timer.

	float GetPulseInterval();
	float GetDischargeInterval();

	void Fire(const Vector& vecOrigSrc, const Vector& vecDir);

	bool HasAmmo();

	void UseAmmo(int count);

	CBeam* m_pBeam;
	CBeam* m_pNoise;
	CSprite* m_pSprite;

	bool UseDecrement() override
	{
#if defined(CLIENT_WEAPONS)
		return true;
#else
		return false;
#endif
	}

	unsigned short m_usEgonStop;

private:
	float m_shootTime;
	EGON_FIREMODE m_fireMode;
	float m_shakeTime;
	bool m_deployed;

	unsigned short m_usEgonFire;
};

class CHgun : public CBasePlayerWeapon
{
public:
#ifndef CLIENT_DLL
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];
#endif

	void Spawn() override;
	void Precache() override;
	int iItemSlot() override { return 4; }
	bool GetItemInfo(ItemInfo* p) override;
	void AddToPlayer(CBasePlayer* pPlayer) override;

	void PrimaryAttack() override;
	void SecondaryAttack() override;
	bool Deploy() override;
	bool IsUseable() override;
	void Holster() override;
	void Reload() override;
	void WeaponIdle() override;
	float m_flNextAnimTime;

	float m_flRechargeTime;

	int m_iFirePhase; // don't save me.

	bool UseDecrement() override
	{
#if defined(CLIENT_WEAPONS)
		return true;
#else
		return false;
#endif
	}

private:
	unsigned short m_usHornetFire;
};

class CHandGrenade : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int iItemSlot() override { return 5; }
	bool GetItemInfo(ItemInfo* p) override;

	void PrimaryAttack() override;
	bool Deploy() override;
	bool CanHolster() override;
	void Holster() override;
	void WeaponIdle() override;

	bool UseDecrement() override
	{
#if defined(CLIENT_WEAPONS)
		return true;
#else
		return false;
#endif
	}
};

class CSatchel : public CBasePlayerWeapon
{
public:
#ifndef CLIENT_DLL
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];
#endif

	void Spawn() override;
	void Precache() override;
	int iItemSlot() override { return 5; }
	bool GetItemInfo(ItemInfo* p) override;
	void AddToPlayer(CBasePlayer* pPlayer) override;
	void PrimaryAttack() override;
	void SecondaryAttack() override;
	bool AddDuplicate(CBasePlayerItem* pOriginal) override;
	bool CanDeploy() override;
	bool Deploy() override;
	bool IsUseable() override;

	void Holster() override;
	void WeaponIdle() override;
	void Throw();

	bool UseDecrement() override
	{
#if defined(CLIENT_WEAPONS)
		return true;
#else
		return false;
#endif
	}
};

class CTripmine : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int iItemSlot() override { return 5; }
	bool GetItemInfo(ItemInfo* p) override;
	void SetObjectCollisionBox() override
	{
		//!!!BUGBUG - fix the model!
		pev->absmin = pev->origin + Vector(-16, -16, -5);
		pev->absmax = pev->origin + Vector(16, 16, 28);
	}

	void PrimaryAttack() override;
	bool Deploy() override;
	void Holster() override;
	void WeaponIdle() override;

	bool UseDecrement() override
	{
#if defined(CLIENT_WEAPONS)
		return true;
#else
		return false;
#endif
	}

private:
	unsigned short m_usTripFire;
};

class CSqueak : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	void Precache() override;
	int iItemSlot() override { return 5; }
	bool GetItemInfo(ItemInfo* p) override;

	void PrimaryAttack() override;
	void SecondaryAttack() override;
	bool Deploy() override;
	void Holster() override;
	void WeaponIdle() override;
	bool m_fJustThrown;

	bool UseDecrement() override
	{
#if defined(CLIENT_WEAPONS)
		return true;
#else
		return false;
#endif
	}

private:
	unsigned short m_usSnarkFire;
};
