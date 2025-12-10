#include "npc.h"
#include "bear.h"
#include "vip.h"
#include "vihuhol.h"
#include "battleManager.h"

#include <ctime>

class TextObserver : public IFightObserver
{
public:
    void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender, bool win)
    {
        if (win) {
            std::cout << std::endl;
            attacker->print();
            std::cout << " --> ";
            defender->print();
        }
    }
};

class FileObserver : public IFightObserver
{
private:
    std::ofstream logFile;

public:
    FileObserver(const std::string& fileName) {
        logFile.open(fileName, std::ios_base::app);
    }

    void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender, bool win)
    {
        if (win && logFile.is_open()) {
            logFile << *attacker << " убивает " << *defender << std::endl;
        }
    }
};

std::shared_ptr<TextObserver> textObs = std::make_shared<TextObserver>();
std::shared_ptr<FileObserver> fileObs = std::make_shared<FileObserver>("log.txt");

std::shared_ptr<NPC> factory(std::istream &is)
{
    std::shared_ptr<NPC> result;
    int type{0};
    if (is >> type)
    {
        switch (type)
        {
            case BearType:
                result = std::make_shared<Bear>(is);
                break;
            case VipType:
                result = std::make_shared<Vip>(is);
                break;
            case VihuholType:
                result = std::make_shared<Vihuhol>(is);
                break;
            default:
                break;
        }
    }

    if (result) {
        result->subscribe(textObs);
        result->subscribe(fileObs);
    }

    return result;
}

std::shared_ptr<NPC> factory(NpcType type, int x, int y)
{
    std::shared_ptr<NPC> result;

    switch (type) {
        case BearType:
            result = std::make_shared<Bear>(x, y);
            break;
        case VipType:
            result = std::make_shared<Vip>(x, y);
            break;
        case VihuholType:
            result = std::make_shared<Vihuhol>(x, y);
            break;
        default:
            break;
    }

    if (result) {
        result->subscribe(textObs);
        result->subscribe(fileObs);
    }

    return result;
}

void save(const set_t &array, const std::string &fileName)
{
    std::ofstream fs(fileName);
    fs << array.size() << std::endl;
    for (auto &n : array) {
        n->save(fs);
    }
    fs.flush();
    fs.close();
}

set_t load(const std::string &fileName)
{
    set_t result;
    std::ifstream is(fileName);
    if (is.good() && is.is_open())
    {
        int count;
        is >> count;

        for (int i = 0; i < count; ++i) {
            result.insert(factory(is));
        }

        is.close();
    }

    return result;
}

std::ostream &operator<<(std::ostream &os, const set_t &array)
{
    for (auto &n : array) {
        n->print();
    }
    return os;
}

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    set_t array;

    std::cout << "Генерация NPC..." << std::endl;
    for (size_t i = 0; i < 50; ++i) {
        int type = std::rand() % 3 + 1;
        int x = std::rand() % 501;
        int y = std::rand() % 501;
        array.insert(factory(NpcType(type), x, y));
    }

    std::cout << "Сохранение..." << std::endl;
    save(array, "npc.txt");

    array.clear();

    std::cout << "Загрузка..." << std::endl;
    array = load("npc.txt");

    std::cout << "Запуск симуляции сражения..." << std::endl;
    for (size_t distance = 20; (distance < 500) && !array.empty(); distance += 10) {
        
        auto dead_list = fight(array, distance);
        for (auto &d : dead_list) {
            array.erase(d);
        }
        std::cout << "Статистика раунда (Дистанция: " << distance << ")" << std::endl
                  << "Убито: " << dead_list.size() << std::endl
                  << std::endl;
    }

    std::cout << "Финальный список выживших:" << array;
}