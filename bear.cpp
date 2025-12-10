#include "bear.h"
#include "vip.h"
#include "vihuhol.h"

Bear::Bear(int x, int y) : NPC(BearType, x, y) {}
Bear::Bear(std::istream &is) : NPC(BearType, is) {}

void Bear::print()
{
    std::cout << *this;
}

void Bear::save(std::ostream &os)
{
    os << std::endl << BearType << std::endl;
    NPC::save(os);
}

bool Bear::accept(std::shared_ptr<NPC> visitor)
{
    return visitor->fight(std::dynamic_pointer_cast<Bear>(shared_from_this()));
}

bool Bear::fight(std::shared_ptr<Bear> other)
{
    fight_notify(other, false);
    return false;
}

bool Bear::fight(std::shared_ptr<Vip> other)
{
    fight_notify(other, true);
    return true;
}

bool Bear::fight(std::shared_ptr<Vihuhol> other)
{
    fight_notify(other, true);
    return true;
}

std::ostream &operator<<(std::ostream &os, Bear &bear)
{
    os << "Медведь: " << *static_cast<NPC *>(&bear) << std::endl;
    return os;
}