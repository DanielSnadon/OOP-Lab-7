#include "npc.h"
#include "bear.h"
#include "vip.h"
#include "vihuhol.h"
#include "battleManager.h"
#include <atomic>
#include <ctime>
#include <thread>

using namespace std::chrono_literals;
std::atomic<bool> stopFlag = false;

class TextObserver : public IFightObserver
{
public:
    void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender, bool win)
    {
        if (win) {
            std::cout << std::endl;
            attacker->print();
            std::cout << " 🗡️-->💀 ";
            defender->print();
        } else {
            std::cout << std::endl;
            attacker->print();
            std::cout << " 🗡️-->🛡️ ";
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

// NEW ПОТОКИ

void moveThread(set_t &npcs)
{
    while (!stopFlag) {
        {
            std::shared_lock<std::shared_mutex> lock(npcMutex);

            for (const auto &attacker : npcs) {
                if (attacker->is_alive()) {

                    attacker->move(npcs);

                    for (const auto &defender : npcs) {
                        if (attacker.get() != defender.get() && defender->is_alive() && attacker->is_close(defender)) {
                            std::lock_guard<std::mutex> tasks_lock(battleTasksMutex);
                            battleTasks.push({attacker, defender});
                        }
                    }
                }
            }
        }
        
        std::this_thread::sleep_for(1000ms);
    }
}

void battleThread()
{
    while (!stopFlag) {
        BattleTask currentTask;
        bool taskFound = false;

        {
            std::lock_guard<std::mutex> tasks_lock(battleTasksMutex);

            if (!battleTasks.empty()) {
                currentTask = battleTasks.front();
                battleTasks.pop();
                taskFound = true;
            }
        }

        if (taskFound) {
            completeBattle(currentTask);
        } else {
            std::this_thread::sleep_for(10ms);
        }
    }
}

void printThread(set_t &npcs)
{
    const int grid{20};
    const int stepX{(int)MAP_SIZE / grid};
    const int stepY{(int)MAP_SIZE / grid};
    std::array<char, grid * grid> fields;

    while (!stopFlag) {
        std::shared_lock<std::shared_mutex> lock_npc(npcMutex);

        fields.fill(' ');

        for (const std::shared_ptr<NPC> &npc : npcs)
        {
            if (npc->is_alive())
            {
                const auto [x, y] = npc->position();
                int i = x / stepX;
                int j = y / stepY;

                if (i >= 0 && i < grid && j >= 0 && j < grid) {
                    char c = '_';
                    switch (npc->get_type())
                    {
                        case BearType:
                            c = 'B';
                            break;
                        case VipType:
                            c = 'V';
                            break;
                        case VihuholType:
                            c = 'X';
                            break;
                        default:
                            break;
                    }
                    fields[i + grid * j] = c;
                }
            }
        }

        lock_npc.unlock();

        {
            std::lock_guard<std::mutex> lock_cout(coutMutex);

            std::cout << "\n         Игровое поле        \n";
            for (int j = 0; j < grid; ++j) {
                for (int i = 0; i < grid; ++i) {
                    char c = fields[i + j * grid];
                    std::cout << "[" << c << "]";
                }
                std::cout << std::endl;
            }
            
        }
        
        std::this_thread::sleep_for(1000ms);
    }
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
    set_t npcs;
    auto observer = std::make_shared<TextObserver>();

    {
        std::lock_guard<std::shared_mutex> lock(npcMutex);
        std::cout << "Генерация NPC..." << std::endl;
        for (size_t i = 0; i < NPC_COUNT; ++i) {
            int type = rand() % 3 + 1;

            int x = std::rand() % (MAP_SIZE + 1);
            int y = std::rand() % (MAP_SIZE + 1);

            auto npc = factory(NpcType(type), x, y);
            if (npc) {
                npc->subscribe(observer);
                npcs.insert(npc);
            }
        }
    }

    std::cout << "Начало симуляции..." << std::endl;
    std::thread moveThr(moveThread, std::ref(npcs));
    std::thread battleThr(battleThread);
    std::thread printThr(printThread, std::ref(npcs));

    std::this_thread::sleep_for(std::chrono::seconds(GAME_LENGTH));

    stopFlag = true;

    if (moveThr.joinable()) {
        moveThr.join();
    }
    if (battleThr.joinable()) {
        battleThr.join();
    }
    if (printThr.joinable()) {
        printThr.join();
    }

    std::cout << "Симуляция завершена. Список выживших:\n" << std::endl;

    std::shared_lock<std::shared_mutex> lock(npcMutex);
    for (const auto &npc : npcs) {
        if (npc->is_alive()) {
            std::cout << std::endl;
            npc->print();
        }
    }

    return 0;
}