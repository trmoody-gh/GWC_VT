#pragma once

#include <Windows.h>

#include "GWCAManager.h"
#include <GWCA\Utilities\Hooker.h>
#include <GWCA\Constants\Skills.h>
#include <GWCA\GameEntities\Skill.h>


namespace GW {

	class EffectMgr : public GWCAManager<EffectMgr> {
		friend class GWCAManager<EffectMgr>;

	public:

		// Get full array of effects and buffs for player and heroes.
		GW::AgentEffectsArray GetPartyEffectArray();

		// Get array of effects on the player.
		GW::EffectArray GetPlayerEffectArray();

		// Get array of buffs on the player.
		GW::BuffArray GetPlayerBuffArray();

		// Drop buffid buff.
		void DropBuff(DWORD buffId);

		// Gets effect struct of effect on player with SkillID, returns Effect::Nil() if no match.
		GW::Effect GetPlayerEffectById(Constants::SkillID SkillID);

		// Gets Buff struct of Buff on player with SkillID, returns Buff::Nil() if no match.
		GW::Buff GetPlayerBuffBySkillId(Constants::SkillID SkillID);

		// Returns current level of intoxication, 0-5 scale.
		// If > 0 then skills that benefit from drunk will work.
		DWORD GetAlcoholLevel() const { return alcohol_level_; }

		// Have fun with this ;))))))))))
		void GetDrunkAf(DWORD Intensity, DWORD Tint);

	private:

		typedef void(__fastcall *PPEFunc_t)(DWORD Intensity, DWORD Tint);
		static void __fastcall AlcoholHandler(DWORD Intensity, DWORD Tint);

		EffectMgr();
		void RestoreHooks() override;

		static PPEFunc_t ppe_retour_func_;
		static DWORD alcohol_level_;
		Hook hk_post_process_effect_;
	};
}
