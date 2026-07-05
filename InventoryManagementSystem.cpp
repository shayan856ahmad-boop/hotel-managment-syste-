// Group Members:
// Mansoor Sadat
// M. Ihtesham Alam
// Shayan Ahmad
// OOP - Inventory Management System | Semester 2

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

class Item {
protected:
    int id;
    string name;
    int quantity;
    double price;

public:
    Item(int id, string name, int quantity, double price) {
        this->id = id;
        this->name = name;
        this->quantity = quantity;
        this->price = price;
    }

    int getId() { return id; }
    string getName() { return name; }
    int getQuantity() { return quantity; }
    double getPrice() { return price; }

    void setQuantity(int q) { quantity = q; }

    virtual void display() {
        cout << "ID: " << id << " | Name: " << name
             << " | Qty: " << quantity << " | Price: " << price;
    }

    virtual string getType() { return "General"; }

    virtual string toFileString() {
        return "General," + to_string(id) + "," + name + "," +
               to_string(quantity) + "," + to_string(price);
    }

    virtual ~Item() {}
};

class FoodItem : public Item {
    string expiryDate;

public:
    FoodItem(int id, string name, int quantity, double price, string expiryDate)
        : Item(id, name, quantity, price) {
        this->expiryDate = expiryDate;
    }

    void display() override {
        Item::display();
        cout << " | Type: Food | Expiry: " << expiryDate << endl;
    }

    string getType() override { return "Food"; }

    string toFileString() override {
        return "Food," + to_string(id) + "," + name + "," +
               to_string(quantity) + "," + to_string(price) + "," + expiryDate;
    }
};

class GeneralItem : public Item {
    string category;

public:
    GeneralItem(int id, string name, int quantity, double price, string category)
        : Item(id, name, quantity, price) {
        this->category = category;
    }

    void display() override {
        Item::display();
        cout << " | Type: General | Category: " << category << endl;
    }

    string getType() override { return "General"; }

    string toFileString() override {
        return "General," + to_string(id) + "," + name + "," +
               to_string(quantity) + "," + to_string(price) + "," + category;
    }
};

class Inventory {
    vector<Item*> items;
    int nextId;
    string filename;

public:
    Inventory(string filename) {
        this->filename = filename;
        nextId = 1;
        loadFromFile();
    }

    ~Inventory() {
        for (int i = 0; i < items.size(); i++) {
            delete items[i];
        }
    }

    void addItem() {
        int type, quantity;
        string name;
        double price;

        cout << "\nItem Type - 1. Food  2. General: ";
        cin >> type;
        cin.ignore();

        cout << "Name: ";
        getline(cin, name);
        cout << "Quantity: ";
        cin >> quantity;
        cout << "Price: ";
        cin >> price;
        cin.ignore();

        if (type == 1) {
            string expiry;
            cout << "Expiry Date (DD/MM/YYYY): ";
            getline(cin, expiry);
            items.push_back(new FoodItem(nextId++, name, quantity, price, expiry));
        } else {
            string category;
            cout << "Category: ";
            getline(cin, category);
            items.push_back(new GeneralItem(nextId++, name, quantity, price, category));
        }

        cout << "Item added successfully.\n";
    }

    void removeItem() {
        int id;
        cout << "\nEnter Item ID to remove: ";
        cin >> id;

        for (int i = 0; i < items.size(); i++) {
            if (items[i]->getId() == id) {
                delete items[i];
                items.erase(items.begin() + i);
                cout << "Item removed.\n";
                return;
            }
        }
        cout << "Item not found.\n";
    }

    void searchItem() {
        string name;
        cin.ignore();
        cout << "\nEnter Item Name to search: ";
        getline(cin, name);

        bool found = false;
        for (int i = 0; i < items.size(); i++) {
            if (items[i]->getName() == name) {
                items[i]->display();
                found = true;
            }
        }
        if (!found) cout << "No item found with that name.\n";
    }

    void displayAll() {
        if (items.empty()) {
            cout << "\nNo items in inventory.\n";
            return;
        }
        cout << "\n--- Inventory List ---\n";
        for (int i = 0; i < items.size(); i++) {
            items[i]->display();
        }
        checkLowStock();
    }

    void checkLowStock() {
        cout << "\n--- Low Stock Alerts (Qty <= 5) ---\n";
        bool found = false;
        for (int i = 0; i < items.size(); i++) {
            if (items[i]->getQuantity() <= 5) {
                cout << "LOW STOCK: " << items[i]->getName()
                     << " | Qty: " << items[i]->getQuantity() << endl;
                found = true;
            }
        }
        if (!found) cout << "All items have sufficient stock.\n";
    }

    void saveToFile() {
        ofstream file(filename);
        for (int i = 0; i < items.size(); i++) {
            file << items[i]->toFileString() << "\n";
        }
        file.close();
        cout << "Data saved to file.\n";
    }

    void loadFromFile() {
        ifstream file(filename);
        if (!file.is_open()) return;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            vector<string> parts;
            string token;
            for (int i = 0; i < line.size(); i++) {
                if (line[i] == ',') {
                    parts.push_back(token);
                    token = "";
                } else {
                    token += line[i];
                }
            }
            parts.push_back(token);

            if (parts.size() < 5) continue;

            string type = parts[0];
            int id = stoi(parts[1]);
            string name = parts[2];
            int qty = stoi(parts[3]);
            double price = stod(parts[4]);

            if (type == "Food" && parts.size() >= 6) {
                items.push_back(new FoodItem(id, name, qty, price, parts[5]));
            } else if (type == "General" && parts.size() >= 6) {
                items.push_back(new GeneralItem(id, name, qty, price, parts[5]));
            }

            if (id >= nextId) nextId = id + 1;
        }
        file.close();
    }
};

int main() {
    Inventory inventory("inventory_data.txt");
    int choice;

    do {
        cout << "\n============================\n";
        cout << "  Inventory Management System\n";
        cout << "============================\n";
        cout << "1. Add Item\n";
        cout << "2. Remove Item\n";
        cout << "3. Search Item\n";
        cout << "4. Display All Items\n";
        cout << "5. Save & Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: inventory.addItem(); break;
            case 2: inventory.removeItem(); break;
            case 3: inventory.searchItem(); break;
            case 4: inventory.displayAll(); break;
            case 5: inventory.saveToFile(); break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 5);

    return 0;
}
