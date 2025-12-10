#include "npc.h"
#include <queue>

struct BattleTask {
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
};

extern std::queue<BattleTask> battleTasks;
extern std::mutex battleTasksMutex;

void completeBattle(const BattleTask& task);