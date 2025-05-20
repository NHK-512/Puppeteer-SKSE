#include "Leader.h"
#include "RE/Skyrim.h"

void Leader::Assign(RE::Actor* npc)
{
	if (npc)
	{
		//auto id = npc->GetFormID();
		CONSOLE_LOG("The NPC {} is assigned with Leader.", npc->GetDisplayFullName());
	}
}


void Leader::Execute(RE::Actor* actor)
{
	//hostile to player check
	if (!actor || !actor->IsHostileToActor(RE::PlayerCharacter::GetSingleton())) {
		return;
	}

	//gets and checks if player is valid
	const auto player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		return;
	}

	/*if (!actor->IsInCombat()) {
		actor->InitiateCombat(player);
		CONSOLE_LOG("Leader {:X} was forced into combat with the player.", actor->GetFormID());
	}*/

	CONSOLE_LOG("The NPC {} is executing Leader behavior.", actor->GetDisplayFullName());


	// makes so that the leader fights more intensely
	//auto* avOwner = actor->AsActorValueOwner();
	//if (avOwner) {
	//	avOwner->SetActorValue(RE::ActorValue::kAggression, 3.0f);  // Frenzied
	//	avOwner->SetActorValue(RE::ActorValue::kConfidence, 4.0f);  // Foolhardy
	//	CONSOLE_LOG("Leader {} aggression/confidence boosted.", actor->GetDisplayFullName());
	//}

	// OPTIONAL: If close, encourage power attacks (hypothetical effect)
	//float distance = actor->GetPosition().GetDistance(player->GetPosition());
	//if (distance < 300.0f) {
	//	actor->NotifyAnimationGraph("TriggerPowerAttack"); // May not always work; depends on state
	//	CONSOLE_LOG("Leader {} is within striking distance, encouraged power attack.", actor->GetDisplayFullName());
	//}

}