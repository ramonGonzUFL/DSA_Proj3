#pragma once

#include <map>
#include <vector>
#include "../lib.hh"

enum Color{Red, Black};
//https://stackoverflow.com/questions/27080879/implementing-enumeration-types-in-c
struct RBTree{
    House house;

    Color color_node;
    RBTree* left;
    RBTree* right;
    RBTree* parent;
    RBTree(House house): house(house), color_node(Red), left (nullptr), right(nullptr), parent(nullptr){}
};

class RedBlackTree{
private:
    RBTree* insert_node(RBTree *root, RBTree *newnode);

    void left_rotate(RBTree*& root, RBTree* x);

    void right_rotate(RBTree*& root, RBTree* y);

    void balance(RBTree*& root, RBTree*& node);

public:
    RBTree* root = nullptr;
    void insert(House house);

    RBTree* search(RBTree* node, float price);

    std::vector<House*> price_range(float min, float max);

    void price_range_helper(RBTree* node, float min, float max, std::vector<House*>& result);

    void inorder_traversal(RBTree* node, std::map<float, Color>& result);

    ~RedBlackTree();

    void delete_tree(RBTree* node);
};
