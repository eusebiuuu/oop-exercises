#include "string"
#include "vector"
#include "iostream"

class Item {
protected:
    std::string name;
public:
    explicit Item(const std::string &name) {
        this->name = name;
    }

    virtual std::string getName() const = 0;

    virtual ~Item() = default;
};

class File: public Item {
    int size;
    std::string extension;
public:
    File(const std::string &name, const std::string& extension, int size = 0):
        Item(name) {
            this->size = size;
            this->extension = extension;
    }

    File(const File* file): Item(file->getName()) {
        this->size = file->size;
        this->extension = file->extension;
    }

    int getSize() const {
        return size;
    }

    std::string getName() const override {
        return this->name;
    }

    const std::string &getExtension() const {
        return extension;
    }
};

class Directory: public Item {
    std::vector<Item*> items;
public:
    explicit Directory(const std::string &name): Item(name){};

    Directory(const Directory* dir): Item(dir->getName()) {
        this->items = dir->items;
    }

    void addItem(Directory* directory) {
        items.push_back(directory);
    }

    std::string getName() const override {
        return this->name;
    }

    File* addItem(File *file) {
        items.push_back(file);
        return file;
    }

    void addDirectory(const std::string &name) {
        // auto* newDir = new Directory(name);
        this->addItem(new Directory(name));
    }

    Directory* navigateTo(const std::string &name) {
        for (Item* item : items) {
            if (auto* currDir = dynamic_cast<Directory *>(item)) {
                if (currDir->getName() == name) {
                    return currDir;
                }
            }
        }
        return nullptr;
    }

    int getSize() const {
        int totalSize = 0;
        for (Item* item : items) {
            if (auto* currFile = dynamic_cast<File*>(item)) {
                totalSize += currFile->getSize();
            } else if (auto* currDir = dynamic_cast<Directory *>(item)) {
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
            if (auto* currFile = dynamic_cast<File*>(item)) {
                out << prefix << start << currFile->getName();
                out << "." << currFile->getExtension();
                out << " " << currFile->getSize() << "\n";
            } else if (auto* currDir = dynamic_cast<Directory *>(item)) {
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

class SymLink: public Item {
    Item* pointedItem;
public:
    SymLink(Item* item): Item(item->getName()) {
        this->pointedItem = item;
    }

    std::string getName() const override {
        return this->name;
    }

    Item* getTarget() {
        return pointedItem;
    }
};

int main() {
    // Item i; can't make an object of class i

    Directory root("/");
    root.addItem(new Directory("bin"));
    root.addItem(new Directory("home"));
    Directory *home = root.navigateTo("home");
    home->addDirectory("tim");
    Directory *tim = home->navigateTo("tim");
    auto* infoFile = tim->addItem(new File("info", "txt", 100));
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
    auto* timDirCopy = new SymLink(tim);
    auto* infoFileCopy = new SymLink(infoFile);
    auto currDir = dynamic_cast<Directory*>(timDirCopy->getTarget());
    auto currFile = dynamic_cast<File*>(infoFileCopy->getTarget());
    std::cout << currDir->getNumberOfItems() << '\n' << currFile->getSize() << '\n';
}