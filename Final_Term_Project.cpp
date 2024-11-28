#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <limits>
using namespace std;

// ���� Class ����
class Hero;

// �⺻ ��������Ʈ Ŭ����
class Sprite {
protected:
    int x, y;  // ���� ��ġ: x�� ��(row), y�� ��(column)
    char shape;
public:
    Sprite(int x, int y, char shape) : x(x), y(y), shape(shape) {}
    virtual ~Sprite() {}
    virtual void move(char d) = 0;
    virtual void moveRandom() {} // �� ������ �����ϰ� �����̴� �� ������
    char getShape() const { return shape; }
    int getX() const { return x; }
    int getY() const { return y; }
    void setX(int newX) { x = newX; }
    void setY(int newY) { y = newY; }
    void setShape(char newShape) { shape = newShape; }
    bool checkCollision(const Sprite* other) const { // ��������Ʈ �� �浹 Ȯ��
        return (x == other->getX() && y == other->getY());
    }
    virtual bool isBoss() const { return false; }
};

// ������ ���̽� Ŭ����
class Item : public Sprite {
public:
    Item(int x, int y, char shape) : Sprite(x, y, shape) {}
    virtual void use(Hero* hero) = 0;
    virtual Item* clone() const = 0;
    virtual ~Item() {} // ���� �Ҹ��� �߰�
    void move(char d) override {} // �������� �������� ����
};


class Hero : public Sprite {
private:
    int hp;
    int attack;
    int defense;
    int attackBoostDuration; // ���� �ν�Ʈ ���� �ϼ�
    vector<Item*> inventory; // ������ �κ��丮

public:
    Hero(int x, int y) : Sprite(x, y, 'H'), hp(100), attack(20), defense(10), attackBoostDuration(0) {}
    void draw() const { cout << 'H'; }
    void move(char d) override {
        // �� ���� ����
        if (d == 'a') { y -= 1; } // ����
        else if (d == 'w') { x -= 1; } // ��
        else if (d == 's') { x += 1; } // �Ʒ�
        else if (d == 'd') { y += 1; } // ������
    }
    int getHP() const { return hp; }
    int getAttack() const {
        return (attackBoostDuration > 0) ? attack + 10 : attack;
    }
    int getDefense() const { return defense; }
    void takeDamage(int dmg) { hp -= dmg; }
    bool isAlive() const { return hp > 0; }
    void heal(int amount) {
        hp += amount;
        if (hp > 100) hp = 100;
    }
    void boostAttack(int duration) { attackBoostDuration = duration; }
    void decrementBoost() { if (attackBoostDuration > 0) attackBoostDuration--; }
    bool hasAttackBoost() const { return attackBoostDuration > 0; }

    // �κ��丮
    void addItem(Item* item) { inventory.push_back(item); }

    void listInventory() const {
        if (inventory.empty()) {
            cout << "�κ��丮�� ��� �ֽ��ϴ�.\n";
            return;
        }
        cout << "�κ��丮:\n";
        // ������ �������� ���� ����
        int healthPotions = 0;
        int attackPotions = 0;
        for (const auto& item : inventory) {
            if (item->getShape() == 'P') healthPotions++;
            else if (item->getShape() == 'A') attackPotions++;
        }
        int index = 1;
        if (healthPotions > 0) {
            cout << index++ << ". ü�� ���� x" << healthPotions << "\n";
        }
        if (attackPotions > 0) {
            cout << index++ << ". ���� ���� x" << attackPotions << "\n";
        }
    }

    void useInventoryItem() {
        if (inventory.empty()) {
            cout << "�κ��丮�� �������� �����ϴ�.\n";
            return;
        }
        listInventory();
        cout << "����� �������� ��ȣ�� �Է��ϼ���: ";
        int choice;
        cin >> choice;
        if (cin.fail() || choice < 1) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "�߸��� �����Դϴ�.\n";
            return;
        }

        // Map ������ ������ ������ ��ȯ
        // 1: ü�� ����, 2: ���� ���� ��
        int healthPotions = 0;
        int attackPotions = 0;
        for (const auto& item : inventory) {
            if (item->getShape() == 'P') healthPotions++;
            else if (item->getShape() == 'A') attackPotions++;
        }

        if (choice == 1 && healthPotions > 0) {
            // ü�� ���� ����ϱ�
            for (size_t i = 0; i < inventory.size(); ++i) {
                if (inventory[i]->getShape() == 'P') {
                    Item* item = inventory[i];
                    item->use(this);
                    delete item;
                    inventory.erase(inventory.begin() + i);
                    break;
                }
            }
        }
        else if (choice == 2 && attackPotions > 0) {
            // ���� ���� ����ϱ�
            for (size_t i = 0; i < inventory.size(); ++i) {
                if (inventory[i]->getShape() == 'A') {
                    Item* item = inventory[i];
                    item->use(this);
                    delete item;
                    inventory.erase(inventory.begin() + i);
                    break;
                }
            }
        }
        else {
            cout << "�߸��� �����Դϴ�.\n";
        }
    }
};


// ü�� ���� Ŭ����
class HealthPotion : public Item {
private:
    int healAmount;
public:
    HealthPotion(int x, int y) : Item(x, y, 'P'), healAmount(50) {}
    void use(Hero* hero) override;
    Item* clone() const override { return new HealthPotion(*this); }
};


// ���� ���� Ŭ����
class AttackPotion : public Item {
private:
    int boostDuration;
public:
    AttackPotion(int x, int y) : Item(x, y, 'A'), boostDuration(3) {}
    void use(Hero* hero) override;
    Item* clone() const override { return new AttackPotion(*this); }
};


void HealthPotion::use(Hero* hero) {
    hero->heal(healAmount);
    cout << "ü�� ������ ����Ͽ� " << healAmount << " HP�� ȸ���߽��ϴ�.\n";
}

void AttackPotion::use(Hero* hero) {
    hero->boostAttack(boostDuration);
    cout << "���� ������ ����Ͽ� ���ݷ��� 10��ŭ �����ϰ� " << boostDuration << "�� ���� �����˴ϴ�.\n";
}

// �� Ŭ����
class Enemy : public Sprite {
protected:
    int hp;
    int attack;
    int defense;
public:
    Enemy(int x, int y, int hp = 50, int attack = 10, int defense = 0) : Sprite(x, y, 'E'), hp(hp), attack(attack), defense(defense) {}
    // ������ �����ϰ� ������
    virtual void move(char d) override {}
    virtual void moveRandom() override {
        // ��, �Ʒ�, ����, ������ ������ �ֱ�
        int direction = rand() % 5;
        if (direction == 0) x -= 1;
        else if (direction == 1) x += 1;
        else if (direction == 2) y -= 1;
        else if (direction == 3) y += 1;

    }
    int getHP() const { return hp; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    void takeDamage(int dmg) { hp -= dmg; }
    bool isAlive() const { return hp > 0; }
    virtual bool isBoss() const override { return false; }
    virtual int getDamage() const { return attack; } // �⺻ ���ݷ� ��ȯ
};

// FastEnemy class
class FastEnemy : public Enemy {
public:
    FastEnemy(int x, int y) : Enemy(x, y, 30, 10, 0) { shape = 'F'; } // attack =10, defense=0
    int getDamage() const override { return 5; } // FastEnemy�� 5�� ���ظ� ����
};

// StrongEnemy class
class StrongEnemy : public Enemy {
public:
    StrongEnemy(int x, int y) : Enemy(x, y, 80, 10, 0) { shape = 'S'; } // attack =10, defense=0
    int getDamage() const override { return 10; } // StrongEnemy�� 10�� ���ظ� ����
};

// Boss class
class Boss : public Enemy {
public:
    Boss(int x, int y) : Enemy(x, y, 200, 10, 0) { shape = 'B'; }
    bool isBoss() const override { return true; }
    int getDamage() const override { return 15; } // Boss�� 15�� ���ظ� ����
    void moveRandom() override { /* Do nothing, boss doesn't move */ } // ������ �������� ����
};

// Treasure class
class Treasure : public Sprite {
public:
    Treasure(int x, int y) : Sprite(x, y, 'T') {}
    void move(char d) override {} // Treasures don't move
};

// Obstacle class
class Obstacle : public Sprite {
public:
    Obstacle(int x, int y) : Sprite(x, y, 'O') {}
    void move(char d) override {} // Obstacles don't move
};

// Board class
class Board {
    char* board;
    int width, height;
public:
    Board(int w, int h) : width(w), height(h) {
        board = new char[width * height];
        clearBoard();
    }
    ~Board() {
        delete[] board;
    }
    void setValue(int r, int c, char shape) {
        if (r >= 0 && r < height && c >= 0 && c < width)
            board[r * width + c] = shape;
    }
    void printBoard() const {
        // Print top border
        for (int i = 0; i < width + 2; i++) cout << '#';
        cout << endl;
        for (int i = 0; i < height; i++) {
            cout << '#'; // Left border
            for (int j = 0; j < width; j++) {
                char current = board[i * width + j];
                if (current != 'H' && current != 'E' && current != 'F' && current != 'S' && current != 'B' && current != 'T' && current != 'P' && current != 'A' && current != 'O') {
                    cout << '.';
                }
                else {
                    cout << current;
                }
            }
            cout << '#' << endl; // Right border
        }
        // Print bottom border
        for (int i = 0; i < width + 2; i++) cout << '#';
        cout << endl;
    }
    void clearBoard() {
        for (int i = 0; i < width * height; i++) {
            board[i] = '.'; // Default empty
        }
    }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

// Function to draw a line
void drawLine(char x) {
    cout << endl;
    for (int i = 0; i < 100; i++) {
        cout << x;
    }
    cout << endl;
}

// Function to check if a position is occupied by any Sprite except specified
bool isOccupied(int x, int y, const vector<Sprite*>& list, Sprite* exclude = nullptr) {
    for (auto& e : list) {
        if (e == exclude) continue;
        if (e->getX() == x && e->getY() == y)
            return true;
    }
    return false;
}

// Function to handle combat between Hero and Enemy
bool combat(Hero* hero, Enemy* enemy) {
    // Display ASCII Art for Combat
    cout << "\n=== ���� ����! ===\n";
    if (enemy->isBoss()) {
        cout << "           ____                \n";
        cout << "          /___/\\               \n";
        cout << "          \\   \\ \\              \n";
        cout << "           \\___\\_\\             \n";
        cout << "           / / / /             \n";
        cout << "          /_/ /_/              \n";
    }
    else if (enemy->getShape() == 'F') { // FastEnemy
        cout << "      ______\n";
        cout << "     /      \\\n";
        cout << "    |  F    |\n";
        cout << "     \\______/ \n";
    }
    else if (enemy->getShape() == 'S') { // StrongEnemy
        cout << "      ______\n";
        cout << "     /      \\\n";
        cout << "    |  S    |\n";
        cout << "     \\______/ \n";
    }
    else { // Normal Enemy
        cout << "        ,     ,\n";
        cout << "       /(.-\"\"-.)\\\n";
        cout << "   |\\  \\/      \\/  /|\n";
        cout << "   | \\ / =.  .= \\ / |\n";
        cout << "   \\( \\   o\\/o   / )/\n";
        cout << "    \\_, '-/  \\-' ,_/\n";
        cout << "      /   \\__/   \\\n";
        cout << "      \\ \\__/\\__/ /\n";
        cout << "    ___\\ \\|--|/ /___\n";
        cout << "  /`    \\      /    `\\\n";
        cout << " /       '----'       \\\n";
    }

    cout << "����� HP: " << hero->getHP() << " | �� HP: " << enemy->getHP() << endl;

    bool defending = false;

    while (hero->isAlive() && enemy->isAlive()) {
        // Hero's turn
        cout << "\n�ൿ�� �����ϼ���:\n1. ����\n2. ���\n3. ������ ���\n4. ����\n���� (1-4): ";
        int choice;
        cin >> choice;

        if (choice == 1) { // Attack
            int damage = hero->getAttack(); // ���ݷ� ���� �ݿ�
            if (damage < 0) damage = 0;
            enemy->takeDamage(damage);
            cout << "������ " << damage << "�� ���ظ� �������ϴ�. �� HP: " << enemy->getHP() << endl;
        }
        else if (choice == 2) { // Defend
            defending = true;
            cout << "���� ���ݿ� ���� ����Ͽ� ������ ���ظ� 50% ���ҽ�ŵ�ϴ�.\n";
        }
        else if (choice == 3) { // Use Item
            hero->useInventoryItem();
        }
        else if (choice == 4) { // Run
            // Simple run chance: 50%
            if (rand() % 2 == 0) {
                cout << "���������� �����ƽ��ϴ�.\n";
                return false; // Ran away successfully
            }
            else {
                cout << "����ġ�� �� �����߽��ϴ�!\n";
            }
        }
        else {
            cout << "�߸��� �����Դϴ�. ���� ��ŵ�Ǿ����ϴ�.\n";
        }

        // Check if enemy is defeated
        if (!enemy->isAlive()) {
            cout << "���� �����ƽ��ϴ�!\n";
            return true; // Enemy defeated
        }

        // Enemy's turn
        if (defending) {
            int damage = enemy->getDamage() / 2; // 50% damage
            defending = false;
            if (damage < 0) damage = 0;
            hero->takeDamage(damage);
            cout << "���� ��ſ��� " << damage << "�� ���ظ� �������ϴ�. ����� HP: " << hero->getHP() << endl;
        }
        else { // Normal attack
            int damage = enemy->getDamage(); // ���� ���� ����
            if (damage < 0) damage = 0;
            hero->takeDamage(damage);
            cout << "���� ��ſ��� " << damage << "�� ���ظ� �������ϴ�. ����� HP: " << hero->getHP() << endl;
        }

        // Check if hero is defeated
        if (!hero->isAlive()) {
            cout << "����� ������ �й��߽��ϴ�...\n";
            return true; // Hero defeated
        }
    }
    return true;
}

// Function to handle stage setup
void setupStage(int stage, vector<Sprite*>& list, Board& board, int width, int height) {
    // Clear previous stage sprites except Hero and inventory items
    // Preserve Hero and inventory items
    vector<Sprite*> newList;
    Hero* hero = nullptr;
    // Transfer Hero and Items to newList
    for (auto& sprite : list) {
        // Preserve Hero
        if (sprite->getShape() == 'H') {
            hero = dynamic_cast<Hero*>(sprite);
            newList.push_back(sprite);
        }
        // Preserve Items
        else if (sprite->getShape() == 'P' || sprite->getShape() == 'A') {
            newList.push_back(sprite);
        }
        // Otherwise, delete
        else {
            delete sprite;
        }
    }
    list = newList;

    // Initialize Hero position only if not present
    if (!hero) {
        hero = new Hero(0, 0);
        list.push_back(hero);
    }

    // Stage 4: Only Boss in the center
    if (stage == 4) {
        // Calculate center position
        int centerX = height / 2;
        int centerY = width / 2;
        // Ensure no overlap with Hero or Items
        if (!isOccupied(centerX, centerY, list)) {
            list.push_back(new Boss(centerX, centerY));
        }
        else {
            // Find another position
            bool placed = false;
            for (int x = 0; x < height && !placed; x++) {
                for (int y = 0; y < width && !placed; y++) {
                    if (!isOccupied(x, y, list)) {
                        list.push_back(new Boss(x, y));
                        placed = true;
                    }
                }
            }
        }
        return; // No other sprites in Stage 4
    }

    // Number of treasures and enemies increases with stage
    int numTreasures = 2 + stage;
    int numEnemies = 2 + stage * 2;
    int numObstacles = 2 + stage;

    // Add Treasures
    for (int i = 0; i < numTreasures; i++) {
        int tx = rand() % height;
        int ty = rand() % width;
        // Ensure no overlap
        if (!isOccupied(tx, ty, list)) {
            list.push_back(new Treasure(tx, ty));
        }
        else {
            // Find another position
            bool placed = false;
            for (int x = 0; x < height && !placed; x++) {
                for (int y = 0; y < width && !placed; y++) {
                    if (!isOccupied(x, y, list)) {
                        list.push_back(new Treasure(x, y));
                        placed = true;
                    }
                }
            }
        }
    }

    // Add Enemies
    for (int i = 0; i < numEnemies; i++) {
        int ex = rand() % height;
        int ey = rand() % width;
        if (!isOccupied(ex, ey, list)) {
            // ������������ �� ���� ����
            int enemyType;
            if (stage == 1) {
                enemyType = rand() % 2; // 0: Normal, 1: Fast
            }
            else {
                enemyType = rand() % 3; // 0: Normal, 1: Fast, 2: Strong
            }

            if (enemyType == 0) {
                list.push_back(new Enemy(ex, ey)); // Normal Enemy
            }
            else if (enemyType == 1) {
                list.push_back(new FastEnemy(ex, ey)); // FastEnemy
            }
            else {
                list.push_back(new StrongEnemy(ex, ey)); // StrongEnemy
            }
        }
        else {
            // Find another position
            bool placed = false;
            for (int x = 0; x < height && !placed; x++) {
                for (int y = 0; y < width && !placed; y++) {
                    if (!isOccupied(x, y, list)) {
                        // ������������ �� ���� ����
                        int enemyType;
                        if (stage == 1) {
                            enemyType = rand() % 2; // 0: Normal, 1: Fast
                        }
                        else {
                            enemyType = rand() % 3; // 0: Normal, 1: Fast, 2: Strong
                        }

                        if (enemyType == 0) {
                            list.push_back(new Enemy(x, y)); // Normal Enemy
                        }
                        else if (enemyType == 1) {
                            list.push_back(new FastEnemy(x, y)); // FastEnemy
                        }
                        else {
                            list.push_back(new StrongEnemy(x, y)); // StrongEnemy
                        }
                        placed = true;
                    }
                }
            }
        }
    }

    // Add Obstacles
    for (int i = 0; i < numObstacles; i++) {
        int ox = rand() % height;
        int oy = rand() % width;
        if (!isOccupied(ox, oy, list)) {
            list.push_back(new Obstacle(ox, oy));
        }
        else {
            // Find another position
            bool placed = false;
            for (int x = 0; x < height && !placed; x++) {
                for (int y = 0; y < width && !placed; y++) {
                    if (!isOccupied(x, y, list)) {
                        list.push_back(new Obstacle(x, y));
                        placed = true;
                    }
                }
            }
        }
    }

    // Add Items
    // For simplicity, let's add one of each item per stage
    // HealthPotion
    bool placedHP = false;
    for (int x = 0; x < height && !placedHP; x++) {
        for (int y = 0; y < width && !placedHP; y++) {
            if (!isOccupied(x, y, list)) {
                list.push_back(new HealthPotion(x, y));
                placedHP = true;
            }
        }
    }
    // AttackPotion
    bool placedAP = false;
    for (int x = 0; x < height && !placedAP; x++) {
        for (int y = 0; y < width && !placedAP; y++) {
            if (!isOccupied(x, y, list)) {
                list.push_back(new AttackPotion(x, y));
                placedAP = true;
            }
        }
    }

    // Clear the board
    board.clearBoard();
    for (auto& e : list) {
        board.setValue(e->getX(), e->getY(), e->getShape()); // x�� ��, y�� ���� ����
    }
}

// Main game function
int main() {
    srand(static_cast<unsigned int>(time(0))); // Seed for random movement
    vector<Sprite*> list; // Use vectors to store all Sprites
    int width, height;
    int totalStages = 4; // Stage 4 �߰�
    int currentStage = 1;

    cout << "������ ũ�⸦ �Է��ϼ��� [�ּ� 10x10]: " << endl;
    do {
        cout << "����: ";
        cin >> width;
        cout << "����: ";
        cin >> height;
        if (width < 10 || height < 10) {
            cout << "�ּ� ũ��� 10x10�Դϴ�. �ٽ� �Է����ּ���." << endl;
        }
    } while (width < 10 || height < 10);

    Board board(width, height);

    // Initialize Hero
    Hero* hero = new Hero(0, 0);
    list.push_back(hero);

    // Setup first stage
    setupStage(currentStage, list, board, width, height);

    // Game variables
    bool gameOver = false;
    int score = 0;

    // The rule of the game
    drawLine('*');
    cout << "��ǥ: ��� ������ ȹ���ϰ� ������ �й����� ������." << endl;
    cout << "�̵� Ű: a(����), d(������), s(�Ʒ�), w(��)" << endl;
    cout << "���� ��: ����, ���, ������ ���, ���� �� ���� �����մϴ�." << endl;
    cout << "ü�� ����(P)�� ���� ����(A)�� ȹ���Ͽ� ������ ��������.\n";
    drawLine('*');
    cout << endl;

    // Main game loop for stages
    while (!gameOver && currentStage <= totalStages) {
        cout << "=== Stage " << currentStage << " ===\n";
        // Redraw the board
        board.clearBoard();
        for (auto& e : list) {
            board.setValue(e->getX(), e->getY(), e->getShape()); // x�� ��, y�� ���� ����
        }
        board.printBoard();

        // Decrement attack boost duration
        hero->decrementBoost();

        // Display status
        cout << "����: " << score << " | Stage: " << currentStage << " | HP: " << hero->getHP();
        if (hero->hasAttackBoost()) {
            cout << " | ���ݷ� ����: ON (" << hero->getAttack() << ")";
        }
        cout << endl;

        // Take in the user's input
        char direction;
        cout << "�̵��� ������ �����ϼ��� (a, s, w, d): ";
        cin >> direction;
        // Validate input
        if (direction != 'a' && direction != 's' && direction != 'w' && direction != 'd') {
            cout << "�߸��� �Է��Դϴ�! a, s, w, d Ű�� ����ϼ���." << endl;
            continue;
        }

        // Move the Hero
        int oldX = hero->getX();
        int oldY = hero->getY();
        hero->move(direction);

        // Boundary checking
        if (hero->getX() < 0) hero->setX(0);
        if (hero->getX() >= height) hero->setX(height - 1);
        if (hero->getY() < 0) hero->setY(0);
        if (hero->getY() >= width) hero->setY(width - 1);

        // Check collisions
        for (size_t i = 0; i < list.size(); ) {
            Sprite* e = list[i];
            if (e == hero) { ++i; continue; }

            if (hero->checkCollision(e)) {
                if (e->getShape() == 'E' || e->getShape() == 'F' || e->getShape() == 'S' || e->getShape() == 'B') { // Enemy types
                    Enemy* enemy = dynamic_cast<Enemy*>(e);
                    if (enemy) {
                        bool outcome = combat(hero, enemy);
                        if (outcome) { // Enemy defeated or Hero defeated
                            if (enemy->isAlive()) { // Hero defeated
                                // Set game over
                                gameOver = true;
                                break;
                            }
                            else { // Enemy defeated
                                if (enemy->isBoss()) {
                                    score += 300;
                                    cout << "������ �����ƽ��ϴ�! +300 ��.\n";
                                    // Check if Stage 4 was completed
                                    if (currentStage == 4) {
                                        gameOver = true;
                                        break;
                                    }
                                }
                                else if (e->getShape() == 'F') { // FastEnemy
                                    score += 30;
                                    cout << "���� ���� �����ƽ��ϴ�! +30 ��.\n";
                                }
                                else if (e->getShape() == 'S') { // StrongEnemy
                                    score += 100;
                                    cout << "���� ���� �����ƽ��ϴ�! +100 ��.\n";
                                }
                                else { // Normal Enemy
                                    score += 50;
                                    cout << "���� �����ƽ��ϴ�! +50 ��.\n";
                                }
                                // Remove the enemy from the list
                                delete e;
                                list.erase(list.begin() + i);
                            }
                        }
                        else { // Ran away
                            // Move Hero back to old position
                            hero->setX(oldX);
                            hero->setY(oldY);
                            cout << "���� ��ġ�� �ǵ��ư��ϴ�.\n";
                            ++i;
                        }
                    }
                    else {
                        ++i;
                    }
                }
                else if (e->getShape() == 'T') { // Treasure
                    score += 100;
                    cout << "������ ȹ���߽��ϴ�! +100 ��." << endl;
                    // Remove the treasure from the list
                    delete e;
                    list.erase(list.begin() + i);
                }
                else if (e->getShape() == 'P' || e->getShape() == 'A') { // Items
                    Item* item = dynamic_cast<Item*>(e);
                    if (item) {
                        cout << "�������� �߰��߽��ϴ�: " << ((item->getShape() == 'P') ? "ü�� ����" : "���� ����") << "!\n";
                        cout << "����Ͻðڽ��ϱ�? (y/n): ";
                        char useChoice;
                        cin >> useChoice;
                        if (useChoice == 'y' || useChoice == 'Y') {
                            item->use(hero);
                            // Remove the item from the list
                            delete item;
                            list.erase(list.begin() + i);
                        }
                        else {
                            // Add to inventory
                            Item* itemCopy = item->clone();
                            hero->addItem(itemCopy);
                            cout << "�������� �κ��丮�� �߰��Ǿ����ϴ�.\n";
                            // Remove the item from the list
                            delete e;
                            list.erase(list.begin() + i);
                        }
                    }
                }
                else if (e->getShape() == 'O') { // Obstacle
                    // Hit an obstacle, move back
                    hero->setX(oldX);
                    hero->setY(oldY);
                    cout << "��ֹ��� �ε������ϴ�! �̵��� �������ϴ�." << endl;
                    ++i;
                }
                else {
                    ++i;
                }
            }
            else {
                ++i;
            }
        }

        // Check if Hero is defeated
        if (!hero->isAlive()) {
            cout << "����� ������ �й��߽��ϴ�! ���� ����.\n";
            gameOver = true;
            break;
        }

        // Check if all treasures are collected (only for stages 1-3)
        if (currentStage < 4) {
            bool allTreasuresCollected = true;
            for (auto& e : list) {
                if (e->getShape() == 'T') {
                    allTreasuresCollected = false;
                    break;
                }
            }

            if (allTreasuresCollected) {
                cout << "Stage " << currentStage << "�� ��� ������ ȹ���߽��ϴ�!\n";
                // Proceed to next stage
                currentStage++;
                if (currentStage > totalStages) {
                    break; // Game completed
                }
                // Setup next stage
                setupStage(currentStage, list, board, width, height);
                continue;
            }
        }

        // Move Enemies randomly (only for stages 1-3)
        if (currentStage < 4) {
            for (auto& e : list) {
                if (e->getShape() == 'E' || e->getShape() == 'F' || e->getShape() == 'S' || e->getShape() == 'B') { // Enemy types
                    e->moveRandom();
                    // Boundary checking for enemies
                    if (e->getX() < 0) e->setX(0);
                    if (e->getX() >= height) e->setX(height - 1);
                    if (e->getY() < 0) e->setY(0);
                    if (e->getY() >= width) e->setY(width - 1);
                }
            }

            // Check collisions after enemies move
            for (size_t i = 0; i < list.size(); ) {
                Sprite* e = list[i];
                if (e == hero) { ++i; continue; }
                if (e->getShape() == 'E' || e->getShape() == 'F' || e->getShape() == 'S' || e->getShape() == 'B') { // Enemy types
                    if (hero->checkCollision(e)) {
                        Enemy* enemy = dynamic_cast<Enemy*>(e);
                        if (enemy) {
                            bool outcome = combat(hero, enemy);
                            if (outcome) { // Enemy defeated or Hero defeated
                                if (enemy->isAlive()) { // Hero defeated
                                    // Set game over
                                    gameOver = true;
                                    break;
                                }
                                else { // Enemy defeated
                                    if (enemy->isBoss()) {
                                        score += 300;
                                        cout << "������ �����ƽ��ϴ�! +300 ��.\n";
                                        // Check if Stage 4 was completed
                                        if (currentStage == 4) {
                                            gameOver = true;
                                            break;
                                        }
                                    }
                                    else if (e->getShape() == 'F') { // FastEnemy
                                        score += 30;
                                        cout << "���� ���� �����ƽ��ϴ�! +30 ��.\n";
                                    }
                                    else if (e->getShape() == 'S') { // StrongEnemy
                                        score += 100;
                                        cout << "���� ���� �����ƽ��ϴ�! +100 ��.\n";
                                    }
                                    else { // Normal Enemy
                                        score += 50;
                                        cout << "���� �����ƽ��ϴ�! +50 ��.\n";
                                    }
                                    // Remove the enemy from the list
                                    delete e;
                                    list.erase(list.begin() + i);
                                }
                            }
                            else { // Ran away
                                // Move Hero back to old position
                                hero->setX(oldX);
                                hero->setY(oldY);
                                cout << "���� ��ġ�� �ǵ��ư��ϴ�.\n";
                                ++i;
                            }
                        }
                        else {
                            ++i;
                        }
                    }
                    else {
                        ++i;
                    }
                }
                else {
                    ++i;
                }
            }

            // Check if Hero is defeated after enemy movement
            if (!hero->isAlive()) {
                cout << "����� ������ �й��߽��ϴ�! ���� ����.\n";
                gameOver = true;
                break;
            }

            // Check if all treasures are collected after enemies moved
            bool allTreasuresCollected = true;
            for (auto& e : list) {
                if (e->getShape() == 'T') {
                    allTreasuresCollected = false;
                    break;
                }
            }

            if (allTreasuresCollected) {
                cout << "Stage " << currentStage << "�� ��� ������ ȹ���߽��ϴ�!\n";
                // Proceed to next stage
                currentStage++;
                if (currentStage > totalStages) {
                    break; // Game completed
                }
                // Setup next stage
                setupStage(currentStage, list, board, width, height);
                continue;
            }
        }

        // Check if hero is still alive and in Stage 4 (boss must be defeated to escape)
        if (currentStage == 4 && hero->isAlive()) {
            // Ensure that the boss has been defeated
            bool bossDefeated = true;
            for (auto& e : list) {
                if (e->isBoss()) {
                    Enemy* enemy = dynamic_cast<Enemy*>(e);
                    if (enemy && enemy->isAlive()) {
                        bossDefeated = false;
                        break;
                    }
                }
            }
            if (bossDefeated) {
                cout << "\n=== ���� �Ϸ�! ===\n";
                cout << "�����մϴ�! ������ ����ġ�� ���������� Ż���߽��ϴ�.\n";
                cout << "���� ����: " << score << endl;
                gameOver = true;
            }
        }

        // Increment score for each move (only for stages 1-3)
        if (currentStage < 4) {
            score += 10;
        }

        drawLine('-');
    }

    // Final stage outcome if not already handled
    if (currentStage > totalStages && !gameOver) {
        cout << "\n=== ���� �Ϸ�! ===\n";
        cout << "�����մϴ�! ��� ���������� �Ϸ��߽��ϴ�.\n";
        cout << "���� ����: " << score << endl;
    }
    if (gameOver && currentStage <= totalStages) {
        cout << "���� ����: " << score << endl;
    }

    // Clean up memory
    for (auto& e : list) {
        delete e;
    }
    list.clear();

    return 0;
}
