#pragma once

#include <iostream>
#include <memory>
#include <cstring>
#include <string>
#include <random>
#include <fstream>
#include <set>
#include <math.h>
#include <mutex>
#include <shared_mutex>

constexpr size_t MAP_SIZE = 400;
constexpr size_t NPC_COUNT = 50;
constexpr size_t GAME_LENGTH = 30;

struct NPC;
struct Bear;
struct Vip;
struct Vihuhol;

using set_t = std::set<std::shared_ptr<NPC>>;

enum NpcType
{
    Unknown = 0,
    BearType = 1,
    VipType = 2,
    VihuholType = 3
};

extern std::mutex coutMutex;
extern std::shared_mutex npcMutex;

struct IFightObserver {
    virtual void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender, bool win) = 0;
};

struct NPC : public std::enable_shared_from_this<NPC>
{
protected:
    NpcType type;
    int x;
    int y;
    int speed{0};
    int killRange{0};
    bool alive{true};
    std::vector<std::shared_ptr<IFightObserver>> observers;

public:
    NPC(NpcType t, int _x, int _y);
    NPC(NpcType t, std::istream &is);

    NpcType get_type() const { return type; }
    std::pair<int, int> position() const { return {x, y}; }
    size_t get_speed() const { return speed; }
    size_t get_range() const { return killRange; }
    bool is_alive() const { return alive; }

    void die() { alive = false; }

    void subscribe(std::shared_ptr<IFightObserver> observer);
    void fight_notify(const std::shared_ptr<NPC> defender, bool win);
    virtual bool is_close(std::shared_ptr<NPC> other) const;

    virtual bool accept(std::shared_ptr<NPC> visitor) = 0;
    
    virtual bool fight(std::shared_ptr<Bear> other) = 0;
    virtual bool fight(std::shared_ptr<Vip> other) = 0;
    virtual bool fight(std::shared_ptr<Vihuhol> other) = 0;
    
    virtual void print() = 0;

    virtual void save(std::ostream &os);

    friend std::ostream &operator<<(std::ostream &os, NPC &npc);

    void move(const set_t &others);

    int roll_dice();

    double distance(std::shared_ptr<NPC> other) const;
};