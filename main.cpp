#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <string.h>
#include <vector>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap{
private:
    size_t tableSize = 0;
    Hash hasher;

    struct Node{
        std::pair<const KeyType, ValueType> value;
        Node* next = nullptr;
        int index = -1;

        Node() {}

        Node(std::pair<const KeyType, ValueType> value): value(value) {}
    };

    std::vector<int> indexes;
    std::vector<Node*> hashTable{std::vector<Node*>(1000228, nullptr)};

    void clear(Node* node) {
        if (node == nullptr)
            return;
        clear(node->next);
        delete node;
    }

    Node* last(Node* node) {
        if (node == nullptr)
            return nullptr;
        if (node->next == nullptr)
            return node;
        return last(node->next);
    }

    void resize() {
        Node* firstNode = nullptr;
        Node* lastNode = nullptr;
        for (int index : indexes) {
            if (hashTable[index] == nullptr)
                continue;
            if (firstNode == nullptr) {
                firstNode = hashTable[index];
                lastNode = last(firstNode);
            }
            else {
                lastNode->next = hashTable[index];
                lastNode = last(hashTable[index]);
            }
            hashTable[index] = nullptr;
        }
        clear();
        hashTable.resize(6 * hashTable.size() + 13, nullptr);
        while (firstNode != lastNode) {
            insert(firstNode->value);
            Node* tmp = firstNode;
            firstNode = firstNode->next;
            delete tmp;
        }
        insert(firstNode->value);
        delete firstNode;
    }

public:
    HashMap(Hash hasher = Hash()): hasher(hasher) {}

    template<typename Iterator>
    HashMap(Iterator begin, Iterator end, Hash hasher = Hash()): hasher(hasher) {
        while (begin != end) {
            insert(*begin);
            ++begin;
        }
    }

    HashMap(const std::initializer_list<std::pair<KeyType, ValueType>> input, Hash hasher = Hash()): hasher(hasher) {
        for (std::pair<KeyType, ValueType> newElement : input)
            insert(newElement);
    }

    HashMap(const HashMap<KeyType, ValueType, Hash>& other): hasher(other.hash_function()) {
        for (const std::pair<const KeyType, ValueType> newElement : other)
            insert(newElement);
    }

    HashMap operator=(const HashMap<KeyType, ValueType, Hash>& other) {
        clear();
        hasher = other.hash_function();
        for (const std::pair<const KeyType, ValueType> newElement : other)
            insert(newElement);
        return *this;
    }

    size_t size() const {
        return tableSize;
    }

    bool empty() const {
        return (tableSize == 0);
    }

    Hash hash_function() const {
        return hasher;
    }

    void insert(std::pair<KeyType, ValueType> newElement) {
        if (find(newElement.first) != end())
            return;
        int hash = hasher(newElement.first) % hashTable.size();
        Node* newNode = new Node(newElement);
        newNode->next = hashTable[hash];
        if (hashTable[hash] == nullptr) {
            newNode->index = indexes.size();
            indexes.push_back(hash);
        }
        else
            newNode->index = hashTable[hash]->index;
        hashTable[hash] = newNode;
        ++tableSize;
        if (tableSize * 3 > hashTable.size())
            resize();
    }

    void erase(KeyType key) {
        if (find(key) == end())
            return;
        int hash = hasher(key) % hashTable.size();
        Node* nodeToErase = hashTable[hash];
        if ((nodeToErase->value).first == key) {
            hashTable[hash] = nodeToErase->next;
            delete nodeToErase;
        }
        else while (true) {
            if ((nodeToErase->next->value).first == key) {
                Node* tmp = nodeToErase->next;
                nodeToErase->next = nodeToErase->next->next;
                delete tmp;
                break;
            }
            nodeToErase = nodeToErase->next;
        }
        --tableSize;
    }

    class iterator{
    public:
        Node* value = nullptr;
        size_t index = -1;
        const std::vector<Node*>* hashTable;
        const std::vector<int>* indexes;

        iterator() {}

        iterator(Node* value, int index, const std::vector<Node*>* hashTable, const std::vector<int>* indexes): value(value), index(index), hashTable(hashTable), indexes(indexes) {}

        iterator& operator++() {
            value = value->next;
            while (index != indexes->size() && value == nullptr) {
                ++index;
                if (index == indexes->size())
                    value = nullptr;
                else
                    value = (*hashTable)[(*indexes)[index]];
            }
            return *this;
        }

        iterator operator++(int) {
            auto tmp = *this;
            value = value->next;
            while (index != indexes->size() && value == nullptr) {
                ++index;
                if (index == indexes->size())
                    value = nullptr;
                else
                    value = (*hashTable)[(*indexes)[index]];
            }
            return tmp;
        }

        std::pair<const KeyType, ValueType> operator*() {
            return value->value;
        }

        std::pair<const KeyType, ValueType>* operator->() {
            return &(value->value);
        }

        bool operator==(iterator other) {
            return value == other.value;
        }

        bool operator!=(iterator other) {
            return value != other.value;
        }
    };

    class const_iterator{
    public:
        Node* value = nullptr;
        size_t index = -1;
        const std::vector<Node*>* hashTable;
        const std::vector<int>* indexes;

        const_iterator() {}

        const_iterator(Node* value, int index, const std::vector<Node*>* hashTable, const std::vector<int>* indexes): value(value), index(index), hashTable(hashTable), indexes(indexes) {}

        const_iterator& operator++() {
            value = value->next;
            while (index != indexes->size() && value == nullptr) {
                ++index;
                if (index == indexes->size())
                    value = nullptr;
                else
                    value = (*hashTable)[(*indexes)[index]];
            }
            return *this;
        }

        const_iterator operator++(int) {
            auto tmp = *this;
            value = value->next;
            while (index != indexes->size() && value == nullptr) {
                ++index;
                if (index == indexes->size())
                    value = nullptr;
                else
                    value = (*hashTable)[(*indexes)[index]];
            }
            return tmp;
        }

        const std::pair<const KeyType, ValueType> operator*() {
            return value->value;
        }

        const std::pair<const KeyType, ValueType>* operator->() {
            return &(value->value);
        }

        bool operator==(const_iterator other) {
            return value == other.value;
        }

        bool operator!=(const_iterator other) {
            return value != other.value;
        }
    };

    iterator begin() {
        return iterator((indexes.empty() ? nullptr : hashTable[indexes.front()]), 0, &hashTable, &indexes);
    }

    iterator end() {
        return iterator(nullptr, indexes.size(), &hashTable, &indexes);
    }

    const_iterator begin() const {
        return const_iterator((indexes.empty() ? nullptr : hashTable[indexes.front()]), 0, &hashTable, &indexes);
    }

    const_iterator end() const {
        return const_iterator(nullptr, indexes.size(), &hashTable, &indexes);
    }

    iterator find(KeyType key) {
        int hash = hasher(key) % hashTable.size();
        Node* currentNode = hashTable[hash];
        while (currentNode != nullptr) {
            if ((currentNode->value).first == key)
                return iterator(currentNode, currentNode->index, &hashTable, &indexes);
            currentNode = currentNode->next;
        }
        return end();
    }

    const_iterator find(KeyType key) const {
        int hash = hasher(key) % hashTable.size();
        Node* currentNode = hashTable[hash];
        while (currentNode != nullptr) {
            if ((currentNode->value).first == key)
                return const_iterator(currentNode, currentNode->index, &hashTable, &indexes);
            currentNode = currentNode->next;
        }
        return end();
    }

    ValueType& operator[](KeyType key) {
        iterator iterator = find(key);
        if (iterator == end()) {
            insert(std::make_pair(key, ValueType()));
            return find(key)->second;
        }
        return iterator->second;
    }

    const ValueType& at(KeyType key) const {
        const_iterator iterator = find(key);
        if (iterator == end())
            throw std::out_of_range("invalid key");
        return iterator->second;
    }

    void clear() {
        for (int index : indexes) {
            clear(hashTable[index]);
            hashTable[index] = nullptr;
        }
        indexes.clear();
        tableSize = 0;
    }

    ~HashMap() {
        clear();
    }
};
