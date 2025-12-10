#include "gtest/gtest.h"
#include "bear.h"
#include "vip.h"
#include "vihuhol.h"
#include "npc.h"
#include "battleManager.h"
#include <memory>

// Утилита для создания NPC без подписки на глобальные наблюдатели
std::shared_ptr<NPC> create_npc(NpcType type, int x, int y) {
    switch (type) {
        case BearType:
            return std::make_shared<Bear>(x, y);
        case VipType:
            return std::make_shared<Vip>(x, y);
        case VihuholType:
            return std::make_shared<Vihuhol>(x, y);
        default:
            return nullptr;
    }
}

// ====================================================================
// Тесты для is_close
// ====================================================================

TEST(NpcTest, IsCloseDistance) {
    // NPC 1: (0, 0)
    auto npc1 = create_npc(BearType, 0, 0); 
    
    // NPC 2: (3, 4) - Дистанция: sqrt(3^2 + 4^2) = 5
    auto npc2 = create_npc(VipType, 3, 4); 
    
    // Дистанция 5: Должно быть true
    EXPECT_TRUE(npc1->is_close(npc2, 5));
    
    // Дистанция 4.9: Должно быть false
    EXPECT_FALSE(npc1->is_close(npc2, 4));

    // NPC 3: (6, 8) - Дистанция: sqrt(6^2 + 8^2) = 10
    auto npc3 = create_npc(VihuholType, 6, 8); 
    
    // Дистанция 10: Должно быть true
    EXPECT_TRUE(npc1->is_close(npc3, 10));

    // Дистанция 9: Должно быть false
    EXPECT_FALSE(npc1->is_close(npc3, 9));

    // NPC 4: (100, 0) - Дистанция 100
    auto npc4 = create_npc(BearType, 100, 0); 

    // Дистанция 100: Должно быть true
    EXPECT_TRUE(npc1->is_close(npc4, 100));

    // Дистанция 99: Должно быть false
    EXPECT_FALSE(npc1->is_close(npc4, 99));

    // С самим собой: всегда true (дистанция 0)
    EXPECT_TRUE(npc1->is_close(npc1, 0));
}

// ====================================================================
// Тесты для логики боя (Visitor Pattern)
// ====================================================================

// Структура правил боя:
//   - Медведь vs. Выпь: Медведь побеждает (true)
//   - Медведь vs. Выхухоль: Медведь побеждает (true)
//   - Медведь vs. Медведь: Ничья (false)
//   - Выпь vs. Медведь: Ничья (false)
//   - Выпь vs. Выпь: Ничья (false)
//   - Выпь vs. Выхухоль: Ничья (false)
//   - Выхухоль vs. Медведь: Выхухоль побеждает (true)
//   - Выхухоль vs. Выпь: Ничья (false)
//   - Выхухоль vs. Выхухоль: Ничья (false)


// Тесты: Атакует Медведь (Bear)
TEST(FightTest, BearAttacks) {
    auto bear = create_npc(BearType, 0, 0);
    auto vip = create_npc(VipType, 0, 0);
    auto vihuhol = create_npc(VihuholType, 0, 0);

    EXPECT_TRUE(vip->accept(bear));
    
    // Атакуем наоборот: Выпь - defender, Медведь - attacker.
    // Медведь атакует Выпь. Выпь (defender) принимает Медведя (visitor).
    // По *файлам*, если Медведь атакует Выпь, то побеждает Медведь.
    // Логика в файлах:
    // bear.cpp: bool Bear::fight(std::shared_ptr<Vip> other) { return true; } // Медведь (attacker) побеждает
    // vip.cpp: bool Vip::fight(std::shared_ptr<Bear> other) { return false; } // Выпь (defender) проигрывает
    // vihuhol.cpp: bool Vihuhol::fight(std::shared_ptr<Bear> other) { return true; } // Выхухоль (defender) проигрывает
    
    // В функции fight в main.cpp: success = defender->accept(attacker);
    // success = defender->accept(attacker);
    // defender->accept(attacker) вызывает attacker->fight(defender_ptr). 
    // Возвращается результат БОЯ АТАКУЮЩЕГО ПРОТИВ ЗАЩИЩАЮЩЕГОСЯ.
    // 'success' означает, что ЗАЩИЩАЮЩИЙСЯ УБИТ (добавляется в dead_list).

    // 1. Медведь атакует Выпь
    // vip->accept(bear) -> bear->fight(vip_ptr) -> true (Bear побеждает, Vip умирает)
    EXPECT_TRUE(vip->accept(bear)); 

    // 2. Медведь атакует Выхухоль
    // vihuhol->accept(bear) -> bear->fight(vihuhol_ptr) -> true (Bear побеждает, Vihuhol умирает)
    EXPECT_TRUE(vihuhol->accept(bear));

    // 3. Медведь атакует Медведя
    // bear->accept(bear) -> bear->fight(bear_ptr) -> false (Ничья, никто не умирает)
    auto another_bear = create_npc(BearType, 1, 1);
    EXPECT_FALSE(bear->accept(another_bear)); 
    EXPECT_FALSE(another_bear->accept(bear)); 
}

// Тесты: Атакует Выпь (Vip)
TEST(FightTest, VipAttacks) {
    auto vip = create_npc(VipType, 0, 0);
    auto bear = create_npc(BearType, 0, 0);
    auto vihuhol = create_npc(VihuholType, 0, 0);

    // 1. Выпь атакует Медведя
    // bear->accept(vip) -> vip->fight(bear_ptr) -> false (Vip проигрывает, Bear не умирает)
    EXPECT_FALSE(bear->accept(vip)); 

    // 2. Выпь атакует Выхухоль
    // vihuhol->accept(vip) -> vip->fight(vihuhol_ptr) -> false (Vip проигрывает, Vihuhol не умирает)
    EXPECT_FALSE(vihuhol->accept(vip));

    // 3. Выпь атакует Выпь
    // vip->accept(vip) -> vip->fight(vip_ptr) -> false (Ничья, никто не умирает)
    auto another_vip = create_npc(VipType, 1, 1);
    EXPECT_FALSE(vip->accept(another_vip)); 
}

// Тесты: Атакует Выхухоль (Vihuhol)
TEST(FightTest, VihuholAttacks) {
    auto vihuhol = create_npc(VihuholType, 0, 0);
    auto bear = create_npc(BearType, 0, 0);
    auto vip = create_npc(VipType, 0, 0);

    // 1. Выхухоль атакует Медведя
    // bear->accept(vihuhol) -> vihuhol->fight(bear_ptr) -> true (Vihuhol побеждает, Bear умирает)
    EXPECT_TRUE(bear->accept(vihuhol)); 

    // 2. Выхухоль атакует Выпь
    // vip->accept(vihuhol) -> vihuhol->fight(vip_ptr) -> false (Vihuhol проигрывает, Vip не умирает)
    EXPECT_FALSE(vip->accept(vihuhol));

    // 3. Выхухоль атакует Выхухоль
    // vihuhol->accept(vihuhol) -> vihuhol->fight(vihuhol_ptr) -> false (Ничья, никто не умирает)
    auto another_vihuhol = create_npc(VihuholType, 1, 1);
    EXPECT_FALSE(vihuhol->accept(another_vihuhol)); 
}

// ====================================================================
// Тесты для функции fight (из main.cpp)
// ====================================================================

// Чтобы протестировать fight, нам нужно создать тестовый класс наблюдателя, 
// так как fight_notify вызывается внутри логики боя NPC.
class TestObserver : public IFightObserver {
public:
    int fights_count = 0;
    std::shared_ptr<NPC> last_winner = nullptr;
    std::shared_ptr<NPC> last_loser = nullptr;
    
    void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender, bool win) override {
        fights_count++;
        if (win) {
            last_winner = attacker;
            last_loser = defender;
        }
    }
};

TEST(SimulationTest, FightLogic) {
    // 1. Инициализация NPC
    // Медведь (Bear) в (0, 0)
    auto bear = create_npc(BearType, 0, 0); 
    // Выпь (Vip) в (5, 0). Дистанция 5.
    auto vip = create_npc(VipType, 5, 0);
    // Выхухоль (Vihuhol) в (100, 100). Дистанция > 20.
    auto vihuhol = create_npc(VihuholType, 100, 100);

    set_t array;
    array.insert(bear);
    array.insert(vip);
    array.insert(vihuhol);

    // Создаем тестовый наблюдатель и подписываем NPC
    auto observer = std::make_shared<TestObserver>();
    bear->subscribe(observer);
    vip->subscribe(observer);
    vihuhol->subscribe(observer);

    // 2. Раунд с дистанцией 5:
    // Только Bear и Vip могут сражаться (дистанция 5 <= 5)
    // - Bear vs Vip: Vip->accept(Bear) -> Bear::fight(Vip) -> true (Vip умирает)
    // - Vip vs Bear: Bear->accept(Vip) -> Vip::fight(Bear) -> false (Никто не умирает)
    
    // Ожидаемый результат: Vip в dead_list.
    size_t distance = 5;
    auto dead_list = fight(array, distance);
    
    // Проверка dead_list
    EXPECT_EQ(dead_list.size(), 1);
    EXPECT_EQ(*dead_list.begin(), vip);
    
    // Проверка наблюдателя (ожидается 2 боя: Bear->Vip и Vip->Bear)
    EXPECT_EQ(observer->fights_count, 2);
    // Проверка победителя/проигравшего (последний успешный бой был Bear->Vip)
    EXPECT_EQ(observer->last_winner, bear);
    EXPECT_EQ(observer->last_loser, vip);
    
    // Очищаем array от мертвых
    for (auto &d : dead_list) {
        array.erase(d);
    }
    
    // 3. Раунд с дистанцией 200:
    // Должны сражаться Bear vs Vihuhol (дистанция 141.4 <= 200)
    // - Bear vs Vihuhol: Vihuhol->accept(Bear) -> Bear::fight(Vihuhol) -> true (Vihuhol умирает)
    // - Vihuhol vs Bear: Bear->accept(Vihuhol) -> Vihuhol::fight(Bear) -> true (Bear умирает)
    distance = 200;
    dead_list = fight(array, distance);
    
    // Проверка dead_list
    EXPECT_EQ(dead_list.size(), 2);

    // Очищаем array от мертвых
    for (auto &d : dead_list) {
        array.erase(d);
    }
    
    // Должен остаться только Bear, который был убит Выхухолью в этом раунде, но:
    // 1. Bear vs Vihuhol: Vihuhol убит.
    // 2. Vihuhol vs Bear: Vihuhol уже в dead_list, поэтому этот бой не произойдет.

    // Должен остаться: array пустой
    EXPECT_TRUE(array.empty());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}