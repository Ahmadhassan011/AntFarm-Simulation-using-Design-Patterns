#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <random>
#include <map>

// ========================== Warrior Class ==========================
class Warrior {
private:
    int health;
    int attackPower;
    int defense;

public:
    Warrior(int health, int attackPower, int defense)
        : health(health), attackPower(attackPower), defense(defense) {}

    bool isAlive() const {
        return health > 0;
    }

    void attack(Warrior& opponent) {
        int damage = attackPower - opponent.defense;
        if (damage > 0) {
            opponent.health -= damage;
        }
    }

    int getHealth() const { return health; }
    int getAttackPower() const { return attackPower; }
    int getDefense() const { return defense; }
};

// ========================== Queen Class ==========================
class Queen {
private:
    bool alive;

public:
    Queen() : alive(true) {}

    bool isAlive() const { return alive; }
    void kill() { alive = false; }
};

// ========================== AntRoom (Factory) ==========================
class AntRoom {
public:
    virtual void performTick() = 0;
    virtual ~AntRoom() = default;
};

class RestingRoom : public AntRoom {
private:
    int capacity;
    int currentAnts;
    int foodConsumptionPerAnt;

public:
    RestingRoom(int capacity, int foodConsumptionPerAnt)
        : capacity(capacity), currentAnts(0), foodConsumptionPerAnt(foodConsumptionPerAnt) {}

    void performTick() override {
        // No output here to reduce verbosity
    }

    bool addAnt() {
        if (currentAnts < capacity) {
            currentAnts++;
            return true;
        }
        return false;
    }

    int getFoodConsumptionPerAnt() const {
        return foodConsumptionPerAnt;
    }

    int getCurrentAnts() const {
        return currentAnts;
    }

    void resetCurrentAnts() {
        currentAnts = 0; // Reset for the next tick if the conditions need it
    }
};

class SpawningRoom : public AntRoom {
public:
    void performTick() override {
        // No output here to reduce verbosity
    }
};

// ========================== AntFarm (Builder + Template) ==========================
class AntFarm {
private:
    int id; // Unique ID for the colony
    int x, y;
    std::string species;
    int food;
    int workerTicksAvailable;
    std::vector<std::shared_ptr<AntRoom>> rooms;
    std::vector<Warrior> warriors;
    std::shared_ptr<Queen> queen;
    bool active;
    int antKills; // Total ant kills by this colony
    int colonyKills; // Total colony kills by this colony
    int ticksAlive; // Total ticks alive

public:
    AntFarm(int id, int x, int y, const std::string& species)
        : id(id), x(x), y(y), species(species), food(0), workerTicksAvailable(0), active(true), antKills(0), colonyKills(0), ticksAlive(0) {
        queen = std::make_shared<Queen>();
    }

    int getId() const { return id; }
    const std::string& getSpecies() const { return species; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getFood() const { return food; } // Added method to get food

    void addRoom(std::shared_ptr<AntRoom> room) {
        rooms.push_back(room);
    }

    void giveResource(const std::string& resource, int amount) {
        if (resource == "food") {
            food += amount;
        } else if (resource == "worker") {
            workerTicksAvailable += amount;
        } else if (resource == "warrior") {
            for (int i = 0; i < amount; ++i) {
                warriors.emplace_back(100, 20, 10); // Default warrior stats
            }
        }
        std::cout << "Added " << amount << " " << resource << " to " << species << " ant farm.\n";
    }

    void performTick() {
        if (!queen->isAlive()) {
            deactivate();
            return;
        }

        ticksAlive++; // Increment ticks alive
        int totalFoodConsumption = 0;

        for (auto& room : rooms) {
            room->performTick();

            if (auto restingRoom = std::dynamic_pointer_cast<RestingRoom>(room)) {
                totalFoodConsumption += restingRoom->addAnt() ? restingRoom->getFoodConsumptionPerAnt() : 0;
            }
        }

        if (food < totalFoodConsumption) {
            deactivate();
        } else {
            food -= totalFoodConsumption;
        }
    }

    void battle(AntFarm& opponent) {
        if (warriors.empty() || opponent.warriors.empty()) {
            return; // No battle occurs if no warriors are available
        }

        Warrior& myWarrior = warriors.back();
        Warrior& enemyWarrior = opponent.warriors.back();

        myWarrior.attack(enemyWarrior);
        enemyWarrior.attack(myWarrior);

        if (!myWarrior.isAlive()) {
            warriors.pop_back();
        }
        if (!enemyWarrior.isAlive()) {
            opponent.warriors.pop_back();
            antKills += 1; // Increment ant kills for defeating an enemy warrior
            food += 10; // Gain food from defeated opponent
        }

        if (opponent.warriors.empty() && opponent.queen->isAlive()) {
            opponent.queen->kill();
            colonyKills += 1; // Increment colony kills for defeating an enemy colony
            mergeWith(opponent);
        }
    }

    void mergeWith(AntFarm& other) {
        food += other.food;
        workerTicksAvailable += other.workerTicksAvailable;
        rooms.insert(rooms.end(), other.rooms.begin(), other.rooms.end());
        warriors.insert(warriors.end(), other.warriors.begin(), other.warriors.end());
        other.deactivate();
    }

    void printSummary() {
        std::cout << "Species: " << species << "\n";
        std::cout << "Workers: " << workerTicksAvailable << "\n"; // Assuming workerTicksAvailable represents the number of workers
        std::cout << "Warriors: " << warriors.size() << "\n";
        std::cout << "Ant Kills: " << antKills << "\n"; // Total ant kills by this colony
        std::cout << "Colony kills: " << colonyKills << "\n"; // Total colony kills by this colony
        std::cout << "Ticks alive: " << ticksAlive << "\n"; // Total ticks alive
        std::cout << "Status: " << (queen->isAlive() ? "Alive" : "Dead") << "\n";
    }

    bool isActive() const {
        return active;
    }

    void deactivate() {
        active = false;
    }
};

// ========================== Meadow (Singleton) ==========================
class Meadow {
private:
    static Meadow* instance;
    std::vector<std::shared_ptr<AntFarm>> antFarms;
    int nextId; // Counter for assigning unique IDs

    Meadow() : nextId(0) {} // Initialize counter

public:
    static Meadow* getInstance() {
        if (!instance) {
            instance = new Meadow();
        }
        return instance;
    }

    void spawnAntFarm(int x, int y, const std::string& species) {
        auto it = std::find_if(antFarms.begin(), antFarms.end(),
                               [x, y](const std::shared_ptr<AntFarm>& farm) { 
                                   return farm->getX() == x && farm->getY() == y; 
                               });

        if (it != antFarms.end()) {
            std::cout << "Error: An ant farm already exists at (" << x << ", " << y << ").\n";
            return; // Prevent creation of a new farm at the same location
        }

        auto antFarm = std::make_shared<AntFarm>(nextId++, x, y, species); // Assign unique ID
        antFarms.push_back(antFarm);
        std::cout << "Spawned " << species << " ant farm at (" << x << ", " << y << ") with ID " << antFarm->getId() << "\n";
    }

    void giveResource(int id, const std::string& resource, int amount) {
        auto it = std::find_if(antFarms.begin(), antFarms.end(),
                               [id](const std::shared_ptr<AntFarm>& farm) { return farm->getId() == id; });

        if (it != antFarms.end()) {
            (*it)->giveResource(resource, amount);
        } else {
            std::cout << "Invalid colony ID.\n";
        }
    }

    void tick(int ticks = 1) {
        for (int i = 0; i < ticks; ++i) {
            std::cout << "==================== Tick " << (i + 1) << " ====================\n";

            // Perform battles between colonies
            for (size_t j = 0; j < antFarms.size(); ++j) {
                for (size_t k = j + 1; k < antFarms.size(); ++k) {
                    if (antFarms[j]->isActive() && antFarms[k]->isActive()) {
                        antFarms[j]->battle(*antFarms[k]);
                    }
                }
            }

            // Perform actions for each colony
            for (auto& farm : antFarms) {
                if (farm->isActive()) {
                    farm->performTick();
                }
            }

            // Remove inactive colonies
            size_t beforeCount = antFarms.size();
            antFarms.erase(std::remove_if(antFarms.begin(), antFarms.end(),
                                           [](const std::shared_ptr<AntFarm>& farm) { return !farm->isActive(); }),
                           antFarms.end());
            size_t afterCount = antFarms.size();

            if (beforeCount != afterCount) {
                std::cout << (beforeCount - afterCount) << " colony(ies) have been removed due to inactivity.\n";
            }

            // Check if the simulation should end
            if (checkSimulationEnd()) {
                std::cout << "Simulation has ended. ";
                int activeColonies = std::count_if(antFarms.begin(), antFarms.end(),
                                                    [](const std::shared_ptr<AntFarm>& farm) { return farm->isActive(); });
                if (activeColonies == 0) {
                    std::cout << "No colonies remain.\n";
                } else if (activeColonies == 1) {
                    for (const auto& farm : antFarms) {
                        if (farm->isActive()) {
                            std::cout << "One colony remains: " << farm->getSpecies() << " at (" << farm->getX() << ", " << farm->getY() << ").\n";
                        }
                    }
                }
                break;
            }

            // Summary of active colonies after the tick
            std::cout << "Active colonies after tick " << (i + 1) << ":\n";
            for (const auto& farm : antFarms) {
                if (farm->isActive()) {
                    std::cout << "Colony ID: " << farm->getId() << ", Species: " << farm->getSpecies() << ", Food: " << farm->getFood() << "\n";
                }
            }

            std::cout << "==================== End of Tick " << (i + 1) << " ====================\n\n";
        }
    }

    bool checkSimulationEnd() {
        int activeColonies = std::count_if(antFarms.begin(), antFarms.end(),
                                           [](const std::shared_ptr<AntFarm>& farm) { return farm->isActive(); });

        return activeColonies <= 1; // Return true if simulation should end
    }

    void summary(int id) {
        auto it = std::find_if(antFarms.begin(), antFarms.end(),
                               [id](const std::shared_ptr<AntFarm>& farm) { return farm->getId() == id; });

        if (it != antFarms.end()) {
            (*it)->printSummary();
        } else {
            std::cout << "Invalid colony ID.\n";
        }
    }
};

Meadow* Meadow::instance = nullptr;

// ========================== Simulation (Mediator) ==========================
class Simulation {
private:
    Meadow* meadow;

public:
    Simulation() : meadow(Meadow::getInstance()) {}

    void runTick(int ticks = 1) {
        meadow->tick(ticks);
    }

    void spawnColony(int x, int y, const std::string& species) {
        meadow->spawnAntFarm(x, y, species);
    }

    void giveResource(int id, const std::string& resource, int amount) {
        meadow->giveResource(id, resource, amount);
    }

    void summary(int id) {
        meadow->summary(id);
    }
};

// ========================== Main Function ==========================
int main() {
    Simulation simulation;
    std::string command;

    std::cout << "Welcome to the AntFarm Simulation! Type 'help' for a list of commands.\n";

    while (true) {
        std::cout << "> "; // Command prompt
        std::cin >> command;

        if (command == "spawn") {
            int x, y;
            std::string species;
            std::cout << "Enter X coordinate: ";
            std::cin >> x;
            std::cout << "Enter Y coordinate: ";
            std::cin >> y;
            std::cout << "Enter species name: ";
            std::cin >> species;
            simulation.spawnColony(x, y, species);
        } else if (command == "give") {
            int id, amount;
            std::string resource;
            std::cout << "Enter colony ID: ";
            std::cin >> id;
            std::cout << "Enter resource type (food/worker/warrior): ";
            std::cin >> resource;
            std::cout << "Enter amount: ";
            std::cin >> amount;
            simulation.giveResource(id, resource, amount);
        } else if (command == "tick") {
            int ticks;
            std::cout << "Enter number of ticks to advance (default 1): ";
            std::cin >> ticks;
            simulation.runTick(ticks);
        } else if (command == "summary") {
            int id;
            std::cout << "Enter colony ID: ";
            std::cin >> id;
            simulation.summary(id);
        } else if (command == "exit") {
            std::cout << "Exiting simulation. Goodbye!\n";
            break;
        } else if (command == "help") {
            std::cout << "Available commands:\n";
            std::cout << "  spawn - Create a new ant colony\n";
            std::cout << "  give - Provide resources to a colony\n";
            std::cout << "  tick - Advance the simulation by a number of ticks\n";
            std::cout << "  summary - Show the status of a colony\n";
            std::cout << "  exit - Exit the simulation\n";
        } else {
            std::cout << "Invalid command. Type 'help' for a list of commands.\n";
        }
    }

    return 0;
}