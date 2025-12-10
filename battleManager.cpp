#include "battleManager.h"

set_t fight(const set_t &array, size_t distance)
{
    set_t dead_list;

    for (const auto &attacker : array) {
        for (const auto &defender : array) {
            if ((attacker != defender) && (dead_list.find(defender) == dead_list.end()) && (attacker->is_close(defender, distance))) {
                bool success = defender->accept(attacker);

                if (success) {
                    dead_list.insert(defender);
                }
            }
        }
    }

    return dead_list;
}