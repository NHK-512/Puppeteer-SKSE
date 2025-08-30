#include "ActorUtils.h"


//RE::NiPoint3 posInFrontOfPlayer(RE::PlayerCharacter* player)
//{
//    RE::NiPoint3 frontPos = {0, 0, 0};
//    player = RE::PlayerCharacter::GetSingleton();
//    if (player) {
//        auto pos = player->GetPosition();  // NiPoint3
//        auto rot = player->data.angle;     // x = pitch, z = yaw
//
//        float distance = 200.0f;
//
//        // Forward direction based on yaw
//        float yaw = rot.z;
//        float dx = std::cos(yaw) * distance;
//        float dy = std::sin(yaw) * distance;
//
//        frontPos = {
//            pos.x + dx,
//            pos.y + dy,
//            pos.z
//        };
//
//        return frontPos;
//    }
//    else
//        return frontPos;
//
//}

bool ActorUtils::dmgTaken(RE::PlayerCharacter* player, RE::Actor* npc)
{
	if (!player)
		player = RE::PlayerCharacter::GetSingleton();

	if (npc->IsDead())
		return 0;

	if (npc->HasBeenAttacked())
	{
		CONSOLE_LOG("NPC {} has been attacked", npc->GetFormID());
		return 1;
	}
		

	return 0;
}

RE::Actor* ActorUtils::getClosestActorToActor(RE::Actor* targetActor, std::vector<RE::Actor*> otherActors)
{
	auto fromPos = targetActor->GetPosition();
	RE::NiPoint3 toPos;
	float dist = FLT_MAX;
	RE::Actor* closestActor = *otherActors.begin();

	for (int i = 0; i < otherActors.size(); i++)
	{
		if (!otherActors[i])
			continue;

		toPos = otherActors[i]->GetPosition();
		if (fromPos.GetDistance(toPos) < dist)
		{
			dist = fromPos.GetDistance(toPos);
			closestActor = otherActors[i];
		}
	}

	return closestActor;
}

std::vector<RE::Actor*> ActorUtils::extractActorsFromRoles(std::unordered_map<RE::FormID, char> roles, char roleType)
{
	std::vector<RE::Actor*> outVct;
	RE::Actor* actor;

	if (roles.size() <= 1)
		return outVct;

	for (std::unordered_map<RE::FormID, char>::iterator i = roles.begin(); i != roles.end(); i++)
	{
		if (i->second == roleType)
		{
			actor = RE::TESForm::LookupByID<RE::Actor>(i->first);
			outVct.push_back(actor);
		}
	}

	return outVct;
}

void ActorUtils::FlushDeadActorsFromRoles(std::unordered_map<RE::FormID, char>& roles)
{
	if (roles.empty())
		return;

	std::erase_if(roles, [](const auto& actr) {
		return RE::TESForm::LookupByID<RE::Actor>(actr.first)->IsDead();
		}
	);
}