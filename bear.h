#pragma once
#include "npc.h"

struct Bear : public NPC
{
    Bear(int x, int y);
    Bear(std::istream &is);

    void print() override;
    void save(std::ostream &os) override;

    bool accept(std::shared_ptr <NPC> visitor) override;

    bool fight(std::shared_ptr<Bear> other) override;
    bool fight(std::shared_ptr<Vip> other) override;
    bool fight(std::shared_ptr<Vihuhol> other) override;

    friend std::ostream &operator<<(std::ostream &os, Bear &bear);
};