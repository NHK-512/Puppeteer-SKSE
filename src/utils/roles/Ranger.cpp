// roles/Ranger.cpp
#include "Ranger.h"
#include <thread>
#include <chrono>
#include <RE/Skyrim.h>

void Ranger::Assign(RE::Actor* npc) 
{
    if (npc) 
    {
        auto id = npc->GetFormID();
        CONSOLE_LOG("The NPC {} is assigned with Ranger.", npc->GetDisplayFullName());
    }
}

void Ranger::KeepDistanceAwayPlayer(RE::Actor* ranger, const std::vector<RE::Actor*>& vanguards, RE::Actor* player)
{
    if (!player || vanguards.empty() || !ranger) return;	

    const auto playerPos = player->GetPosition();

    // Find the furthest vanguard distance
    float maxVanguardDistance = 0.0f;
    for (auto* vg : vanguards) {
        if (!vg) continue;
        float dist = vg->GetPosition().GetDistance(playerPos);
        if (dist > maxVanguardDistance)
            maxVanguardDistance = dist;
    }

    // Check each ranger
    float dist = ranger->GetPosition().GetDistance(playerPos);
	if (dist < maxVanguardDistance) {
		ranger->InitiateFlee(player, 0, 1, 1, nullptr, nullptr, maxVanguardDistance * 10, maxVanguardDistance * 15);
	}
}

//void Ranger::PrioritizeDistance(RE::Actor* ranger, RE::Actor* player, float desiredDistance)
//{
//	if (!ranger || !player || ranger->IsDead() || player->IsDead()) {
//		return;
//	}
//
//	RE::NiPoint3 rangerPos = ranger->GetPosition();
//	RE::NiPoint3 playerPos = player->GetPosition();
//	float distance = rangerPos.GetDistance(playerPos);
//
//	static std::unordered_map<RE::FormID, float> originalConfidences;
//
//
//	auto aiOwner = ranger->AsActorValueOwner();
//	if (!aiOwner) {
//		return;
//	}
//
//	auto rangerID = ranger->GetFormID();
//
//	// Store original confidence if not already stored
//	if (!originalConfidences.contains(rangerID)) {
//		originalConfidences[rangerID] = aiOwner->GetActorValue(RE::ActorValue::kConfidence);
//	}
//
//	//auto originalConfidence = aiOwner->GetActorValue(RE::ActorValue::kConfidence);
//
//	// Set to Coward to trigger fleeing behavior
//	if (distance < desiredDistance) {
//		aiOwner->SetActorValue(RE::ActorValue::kConfidence, static_cast<float>(RE::ACTOR_CONFIDENCE::kCowardly));
//		CONSOLE_LOG("Ranger {} is fleeing from player.", rangerID);
//	}
//	//aiOwner->SetActorValue(RE::ActorValue::kConfidence, static_cast<float>(RE::ACTOR_CONFIDENCE::kCowardly));
//	//CONSOLE_LOG("Ranger {} fleeing to maintain distance. Dist: {:.2f}", ranger->GetFormID(), distance);
//
//	// Start a background task to monitor distance
//	std::thread([ranger, player, desiredDistance]() {
//		using namespace std::chrono_literals;
//
//		for (int i = 0; i < 20; ++i) {  // check for up to 10 seconds
//			std::this_thread::sleep_for(500ms);
//
//			if (!ranger->Is3DLoaded() || ranger->IsDead())
//				break;
//
//			auto currentDistance = ranger->GetPosition().GetDistance(player->GetPosition());
//			if (currentDistance >= desiredDistance) {
//				auto aiOwner = ranger->AsActorValueOwner();
//				if (aiOwner) {
//					auto it = originalConfidences.find(ranger->GetFormID());
//					if(it != originalConfidences.end())
//						aiOwner->SetActorValue(RE::ActorValue::kConfidence, it->second);
//
//					CONSOLE_LOG("Ranger {} stopped fleeing. Distance met. New dist: {:.2f}", ranger->GetFormID(), currentDistance);
//				}
//				break;
//			}
//		}
//	}).detach();
//}
