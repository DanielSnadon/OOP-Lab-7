#include "gtest/gtest.h"
#include <memory>
#include <iostream>

// Подключаем необходимые заголовочные файлы
#include "npc.h"
#include "bear.h"
#include "vip.h"
#include "vihuhol.h"

// --- 1. Mock Observer ---
// Вспомогательный класс для тестирования, который записывает результат боя, 
// но не выводит его в консоль. Это позволяет проверить, что метод fight_notify 
// вызывается корректно и с правильным результатом (win/lose).
class MockObserver : public IFightObserver {
public:
    int callCount = 0;
    bool lastWin = false;
    
    void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender, bool win) override {
        callCount++;
        lastWin = win;
    }
};

// --- 2. Test Fixture ---
// Базовый класс для тестов, который содержит вспомогательные функции для создания NPC.
class FightTest : public ::testing::Test {
protected:
    // Вспомогательные функции для создания NPC через std::make_shared
    // Это важно, так как NPC наследуют std::enable_shared_from_this.
    std::shared_ptr<Bear> createBear(int x = 0, int y = 0) {
        return std::make_shared<Bear>(x, y);
    }
    std::shared_ptr<Vip> createVip(int x = 0, int y = 0) {
        return std::make_shared<Vip>(x, y);
    }
    std::shared_ptr<Vihuhol> createVihuhol(int x = 0, int y = 0) {
        return std::make_shared<Vihuhol>(x, y);
    }
};


// =====================================================================
// ТЕСТЫ ЛОГИКИ БОЯ (Visitor Pattern)
// Логика: Defender (Host) принимает Attacker (Visitor). 
// Вызывается метод Attacker::fight(DefenderType).
// Возвращается 'true', если Defender убит, 'false' - если ничья.
// =====================================================================

// --- МЕДВЕДЬ (Bear) ---
TEST_F(FightTest, BearVsVip_BearWins) {
    auto bear = createBear();
    auto vip = createVip();
    auto obs = std::make_shared<MockObserver>();
    bear->subscribe(obs);

    // Медведь атакует Выпь. Bear::fight(Vip) -> true
    bool result = vip->accept(bear);
    
    ASSERT_TRUE(result) << "Медведь должен победить Выпь.";
    ASSERT_TRUE(obs->lastWin) << "Наблюдатель должен получить уведомление о победе.";
}

TEST_F(FightTest, BearVsVihuhol_BearWins) {
    auto bear = createBear();
    auto vihuhol = createVihuhol();
    auto obs = std::make_shared<MockObserver>();
    bear->subscribe(obs);
    
    // Медведь атакует Выхухоль. Bear::fight(Vihuhol) -> true
    bool result = vihuhol->accept(bear);
    
    ASSERT_TRUE(result) << "Медведь должен победить Выхухоль.";
    ASSERT_TRUE(obs->lastWin);
}

TEST_F(FightTest, BearVsBear_Draw) {
    auto bear1 = createBear();
    auto bear2 = createBear();
    auto obs = std::make_shared<MockObserver>();
    bear1->subscribe(obs);
    
    // Медведь атакует Медведя. Bear::fight(Bear) -> false
    bool result = bear2->accept(bear1);
    
    ASSERT_FALSE(result) << "Медведи должны разойтись миром.";
    ASSERT_FALSE(obs->lastWin) << "Наблюдатель должен получить уведомление о ничьей.";
}

// --- ВЫПЬ (Vip) ---
TEST_F(FightTest, VipVsBear_Draw) {
    auto vip = createVip();
    auto bear = createBear();
    auto obs = std::make_shared<MockObserver>();
    vip->subscribe(obs);

    // Выпь атакует Медведя. Vip::fight(Bear) -> false
    bool result = bear->accept(vip);

    ASSERT_FALSE(result) << "Выпь не должна побеждать Медведя.";
    ASSERT_FALSE(obs->lastWin);
}

TEST_F(FightTest, VipVsVihuhol_Draw) {
    auto vip = createVip();
    auto vihuhol = createVihuhol();
    auto obs = std::make_shared<MockObserver>();
    vip->subscribe(obs);

    // Выпь атакует Выхухоль. Vip::fight(Vihuhol) -> false
    bool result = vihuhol->accept(vip);

    ASSERT_FALSE(result) << "Выпь не должна побеждать Выхухоль.";
    ASSERT_FALSE(obs->lastWin);
}

TEST_F(FightTest, VipVsVip_Draw) {
    auto vip1 = createVip();
    auto vip2 = createVip();
    auto obs = std::make_shared<MockObserver>();
    vip1->subscribe(obs);

    // Выпь атакует Выпь. Vip::fight(Vip) -> false
    bool result = vip2->accept(vip1);

    ASSERT_FALSE(result) << "Выпи должны разойтись миром.";
    ASSERT_FALSE(obs->lastWin);
}

// --- ВЫХУХОЛЬ (Vihuhol) ---
TEST_F(FightTest, VihuholVsBear_VihuholWins) {
    auto vihuhol = createVihuhol();
    auto bear = createBear();
    auto obs = std::make_shared<MockObserver>();
    vihuhol->subscribe(obs);

    // Выхухоль атакует Медведя. Vihuhol::fight(Bear) -> true
    bool result = bear->accept(vihuhol);

    ASSERT_TRUE(result) << "Выхухоль должен победить Медведя.";
    ASSERT_TRUE(obs->lastWin);
}

TEST_F(FightTest, VihuholVsVip_Draw) {
    auto vihuhol = createVihuhol();
    auto vip = createVip();
    auto obs = std::make_shared<MockObserver>();
    vihuhol->subscribe(obs);

    // Выхухоль атакует Выпь. Vihuhol::fight(Vip) -> false
    bool result = vip->accept(vihuhol);

    ASSERT_FALSE(result) << "Выхухоль не должна побеждать Выпь.";
    ASSERT_FALSE(obs->lastWin);
}

TEST_F(FightTest, VihuholVsVihuhol_Draw) {
    auto vihuhol1 = createVihuhol();
    auto vihuhol2 = createVihuhol();
    auto obs = std::make_shared<MockObserver>();
    vihuhol1->subscribe(obs);

    // Выхухоль атакует Выхухоль. Vihuhol::fight(Vihuhol) -> false
    bool result = vihuhol2->accept(vihuhol1);

    ASSERT_FALSE(result) << "Выхухоли должны разойтись миром.";
    ASSERT_FALSE(obs->lastWin);
}