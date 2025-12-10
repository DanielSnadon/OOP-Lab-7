#include "bear.h"
#include "vip.h"
#include "vihuhol.h"

Vihuhol::Vihuhol(int x, int y) : NPC(VihuholType, x, y) {
    speed = 5;
    killRange = 20;
}
Vihuhol::Vihuhol(std::istream &is) : NPC(VihuholType, is) {
    speed = 5;
    killRange = 20;
}

void Vihuhol::print()
{
    std::cout << *this;
}

void Vihuhol::save(std::ostream &os)
{
    os << std::endl << VihuholType << std::endl;
    NPC::save(os);
}

bool Vihuhol::accept(std::shared_ptr<NPC> visitor)
{
    return visitor->fight(std::dynamic_pointer_cast<Vihuhol>(shared_from_this()));
}

bool Vihuhol::fight(std::shared_ptr<Bear> other)
{
    fight_notify(other, true);
    return true;
}

bool Vihuhol::fight(std::shared_ptr<Vip> other)
{
    fight_notify(other, false);
    return false;
}

bool Vihuhol::fight(std::shared_ptr<Vihuhol> other)
{
    fight_notify(other, false);
    return false;
}

std::ostream &operator<<(std::ostream &os, Vihuhol &vihuhol)
{
    os << "Выхухоль: " << *static_cast<NPC *>(&vihuhol);
    return os;
}