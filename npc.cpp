#include "npc.h"
#include "bear.h"
#include "vip.h"
#include "vihuhol.h"
#include <algorithm>


std::mutex coutMutex;
std::shared_mutex npcMutex;

NPC::NPC(NpcType t, int _x, int _y) : type(t), x(_x), y(_y) {}
NPC::NPC(NpcType t, std::istream &is) : type(t)
{
    is >> x;
    is >> y;
}

void NPC::save(std::ostream &os)
{
    os << x << " ";
    os << y << " ";
}

int NPC::roll_dice()
{
    return std::rand() % 6;
}

void NPC::move(const set_t &others)
{
    if (!is_alive()) return;

    std::shared_ptr<NPC> nearest = nullptr;
    double minDist = 100000;

    for (const auto& other: others) {
        if (other->is_alive() && (other.get() != this)) {
            double dist = distance(other);
            if (nearest == nullptr || dist < minDist) {
                nearest = other;
                minDist = dist;
            }
        }
    }

    int newX = x;
    int newY = y;

    if (nearest != nullptr) {
        double dx = (double)nearest->x - x;
        double dy = (double)nearest->y - y;
        double angle = std::atan2(dy, dx);

        newX += (int)(std::cos(angle) * speed);
        newY += (int)(std::sin(angle) * speed);
    } else {

        newX = rand() % (2 * speed + 1) - speed;
        newY = rand() % (2 * (speed - abs(newX)) + 1) - (speed - abs(newX));
    }

    x = std::clamp(newX, 0, (int)MAP_SIZE);
    y = std::clamp(newY, 0, (int)MAP_SIZE);
}

double NPC::distance(std::shared_ptr<NPC> other) const
{
    return std::sqrt(std::pow(x - other->x, 2) + std::pow(y - other->y, 2));
}

bool NPC::is_close(const std::shared_ptr<NPC> other) const
{
    return distance(other) < killRange;
}

void NPC::subscribe(std::shared_ptr<IFightObserver> observer)
{
    observers.push_back(observer);
}

void NPC::fight_notify(const std::shared_ptr<NPC> defender, bool win)
{
    std::lock_guard<std::mutex> lock(coutMutex);
    for (auto &o : observers)
        o->on_fight(shared_from_this(), defender, win);
}

std::ostream &operator<<(std::ostream &os, NPC &npc)
{
    os << "{ x:" << npc.x << ", y:" << npc.y << "} ";
    return os;
}


