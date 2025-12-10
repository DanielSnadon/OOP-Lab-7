#include "bear.h"
#include "vip.h"
#include "vihuhol.h"

Vip::Vip(int x, int y) : NPC(VipType, x, y) {}
Vip::Vip(std::istream &is) : NPC(VipType, is) {}

void Vip::print()
{
    std::cout << *this;
}

void Vip::save(std::ostream &os)
{
    os << std::endl << VipType << std::endl;
    NPC::save(os);
}

bool Vip::accept(std::shared_ptr<NPC> visitor)
{
    return visitor->fight(std::dynamic_pointer_cast<Vip>(shared_from_this()));
}

bool Vip::fight(std::shared_ptr<Bear> other)
{
    fight_notify(other, false);
    return false;
}

bool Vip::fight(std::shared_ptr<Vip> other)
{
    fight_notify(other, false);
    return false;
}

bool Vip::fight(std::shared_ptr<Vihuhol> other)
{
    fight_notify(other, false);
    return false;
}

std::ostream &operator<<(std::ostream &os, Vip &vip)
{
    os << "Выпь: " << *static_cast<NPC *>(&vip) << std::endl;
    return os;
}