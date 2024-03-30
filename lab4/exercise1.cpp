#include "string"
#include "vector"
#include "iostream"

class Item {
protected:
    std::string name;
    std::string type;
public:
    explicit Item(const std::string &name, const std::string &type) {
        this->name = name;
        this->type = type;
    }

    virtual std::string getName() const = 0;

    virtual ~Item() = default;

    virtual const std::string &getType() const = 0;
};

class SymLink: public Item {
    Item* address;
    std::string itemType;
public:
    SymLink(Item* address, const std::string& name, const std::string& type):
        Item(name, "symlink") {
        this->address = address;
        this->itemType = type;
    }

    Item *getAddress() const {
        return address;
    }

    const std::string &getItemType() const {
        return itemType;
    }

    std::string getName() const override {
        return this->name;
    }

    const std::string &getType() const override {
        return this->type;
    }
};

class File: public Item {
    int size;
    std::string extension;
public:
    File(const std::string &name, const std::string& extension, int size = 0):
        Item(name, "file") {
            this->size = size;
            this->extension = extension;
    }

    int getSize() const {
        return size;
    }

    std::string getName() const override {
        return this->name;
    }

    const std::string &getType() const override {
        return this->type;
    }

    const std::string &getExtension() const {
        return extension;
    }
};

class Directory: public Item {
    std::vector<Item*> items;
public:
    explicit Directory(const std::string &name): Item(name, "directory"){};

    void addItem(Directory* directory) {
        items.push_back(directory);
    }

    std::string getName() const override {
        return this->name;
    }

    const std::string& getType() const override {
        return this->type;
    }

    void addItem(File *file) {
        items.push_back(file);
    }

    void addDirectory(const std::string &name) {
        // auto* newDir = new Directory(name);
        this->addItem(new Directory(name));
    }

    Directory* navigateTo(const std::string &name) {
        for (Item* item : items) {
            if (item->getName() == name and item->getType() == "directory") {
                return dynamic_cast<Directory *>(item);
            }
        }
        return nullptr;
    }

    int getSize() const {
        int totalSize = 0;
        for (Item* item : items) {
            if (item->getType() == "file") {
                const auto* currFile = dynamic_cast<const File*>(item);
                totalSize += currFile->getSize();
            } else {
                const auto* currDir = dynamic_cast<const Directory*>(item);
                totalSize += currDir->getSize();
            }
        }
        return totalSize;
    }

    int getNumberOfItems() {
        return (int) this->items.size();
    }

    void showHierarchy(std::ostream& out, int level, const Directory &directory) const {
        std::string prefix, ans, start = level == 0 ? "|-" : "|_";
        for (int i = 0; i < level; ++i) {
            prefix += "  ";
        }
        for (Item* item : directory.items) {
            if (item->getType() == "file") {
                const File* currFile = dynamic_cast<const File*>(item);
                out << prefix << start << currFile->getName();
                out << "." << currFile->getExtension();
                out << " " << currFile->getSize() << "\n";
            } else if (item->getType() == "directory") {
                auto* currDir = dynamic_cast<Directory*>(item);
                out << prefix << start << currDir->getName() << '\n';
                currDir->showHierarchy(out, level + 1, *currDir);
            }
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const Directory &directory) {
        out << directory.getName() << '\n';
        directory.showHierarchy(out, 0, directory);
        return out;
    }
};


int main() {
    // Item i; can't make a object of class i

    Directory root("/");
    root.addItem(new Directory("bin"));
    root.addItem(new Directory("home"));
    Directory *home = root.navigateTo("home");
    home->addDirectory("tim");
    Directory *tim = home->navigateTo("tim");
    tim->addItem(new File("info", "txt", 100));
    tim->addItem(new File("data", "in"));
    tim->addItem(new Directory("data"));

    std::cout << root.getSize() << std::endl;
    std::cout << root.getNumberOfItems() << std::endl;
    std::cout << root;
    // va fi vizibila o ierarhie de itemi
    /*
        /
        |-bin
        |-home
          |_tim
            |_info.txt 100
            |_data.in 0
            |_data
    */

    // SymLinks functionalities illustration:
    SymLink testLink(tim, tim->getName(), "directory");
    auto timDirCopy = dynamic_cast<const Directory*>(testLink.getAddress());
    std::cout << timDirCopy->getSize() << '\n';
}