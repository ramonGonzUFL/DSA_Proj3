#include<iostream>
#include <algorithm>

enum Color{Red, Black};
//https://stackoverflow.com/questions/27080879/implementing-enumeration-types-in-c
struct RBTree{
    std::string address;
    int price;
    Color color_node;
    RBTree *left;
    RBTree *right;
    RBTree *parent;
    RBTree(int x, std::string ad = ""): price(x), address(ad), color_node(Red), left (nullptr), right(nullptr), parent(nullptr){}
};

class RedBlackTree{
private:
    RBTree* insert_node(RBTree *root, RBTree *newnode){
        if (root == nullptr){
            return newnode;
        }
        if (newnode->price < root->price){
            root->left = insert_node(root->left, newnode);
            root->left->parent = root;
        }
        else{
            root->right = insert_node(root->right, newnode);
            root->right->parent = root;
        }
    }

    void left_rotate(RBTree*& root, RBTree* x){
        RBTree* y = x->right;
        x->right = y->left;
        if (y->left != nullptr)
            y->left->parent = x;

        y->parent = x->parent;

        if (x->parent == nullptr)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;

        y->left = x;
        x->parent = y;
    }

    void right_rotate(RBTree*& root, RBTree* y){
        RBTree* x = y->left;
        y->left = x->right;
        if (x->right != nullptr)
            x->right->parent = y;
        x->parent = y->parent;

        if (y->parent == nullptr)
            root = x;
        else if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;

        x->right = y;
        y->parent = x;
    }

    void balance(RBTree*& root, RBTree*& node){
        while (node != root && node->parent->color_node == Red) {
            RBTree* grandparent = node->parent->parent;

            if (node->parent == grandparent->left) {
                RBTree* uncle = grandparent->right;

                if (uncle && uncle->color_node == Red) {
                    node->parent->color_node = Black;
                    uncle->color_node = Black;
                    grandparent->color_node = Red;
                    node = grandparent;
                } else {
                    if (node == node->parent->right) {
                        node = node->parent;
                        left_rotate(root, node);
                    }
                    node->parent->color_node = Black;
                    grandparent->color_node = Red;
                    right_rotate(root, grandparent);
                }
            }
            else{
                RBTree* uncle = grandparent->left;
                if (uncle && uncle->color_node == Red) {
                    node->parent->color_node = Black;
                    uncle->color_node = Black;
                    grandparent->color_node = Red;
                    node = grandparent;
                }
                else{
                    if (node == node->parent->left){
                        node = node->parent;
                        right_rotate(root, node);
                    }
                    node->parent->color_node = Black;
                    grandparent->color_node = Red;
                    left_rotate(root, grandparent);
                }
            }
        }
        root->color_node = Black;
    }

public:
    RBTree* root = nullptr;
    void insert(int price, const std::string& address){         //combines functionality of insert functions above
        RBTree* newNode = new RBTree(price, address);
        root = insert_node(root, newNode);
        balance(root, newNode);
    }

    RBTree* search(RBTree* node, int price) {
        if (node == nullptr || node->price == price) {
            return node;
        }
        if (price < node->price) {
            return search(node->left, price);
        }
        return search(node->right, price);
    }

    std::vector<RBTree*> price_range(RBTree* node, int min, int max){
        std::vector<RBTree*> range;
        if (!node){
            return range;
        }
        if (min < node->price){
            std::vector<RBTree*> left = price_range(node->left, min, max);
            range.insert(range.end(), left.begin(), left.end());
        }
        if (min <= node->price && node->price <= max){
            range.push_back(node);
        }
        if (max > node->price){
            std::vector<RBTree*> right = price_range(node->right, min, max);
            range.insert(range.end(), right.begin(), right.end());
        }
        return range;
    }
};