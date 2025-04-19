#include "redblack.hh"

RBTree* RedBlackTree::insert_node(RBTree *root, RBTree *newnode){
    if (root == nullptr){
        return newnode;
    }
    if (newnode->house.price < root->house.price){
        root->left = insert_node(root->left, newnode);
        root->left->parent = root;
    }
    else{
        root->right = insert_node(root->right, newnode);
        root->right->parent = root;
    }
    return root;
}

void RedBlackTree::left_rotate(RBTree*& root, RBTree* x){
    RBTree* y = x->right;
    x->right = y->left;
    if (y->left != nullptr)
        y->left->parent = x;

    y->parent = x->parent;
    if (x->parent == nullptr) {
        root = y;
    }
    else if (x == x->parent->left){
        x->parent->left = y;
    }
    else{
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void RedBlackTree::right_rotate(RBTree*& root, RBTree* x){
    RBTree* y = x->left;
    x->left = y->right;
    if (y->right != nullptr){
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr){
        root = y;
    }
    else if (x == x->parent->right){
        x->parent->right = y;
    }
    else{
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

void RedBlackTree::balance(RBTree*& root, RBTree*& node){
    while (node != root && node->parent->color_node == Red){
        RBTree* grandparent = node->parent->parent;

        if (node->parent == grandparent->left){
            RBTree* uncle = grandparent->right;

            if (uncle && uncle->color_node == Red){
                node->parent->color_node = Black;
                uncle->color_node = Black;
                grandparent->color_node = Red;
                node = grandparent;
            }
            else{
                if (node == node->parent->right){
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
            if (uncle && uncle->color_node == Red){
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

void RedBlackTree::insert(House house){         //combines functionality of insert functions above
    RBTree* newNode = new RBTree(house);
    root = insert_node(root, newNode);
    balance(root, newNode);
}

RBTree* RedBlackTree::search(RBTree* node, float price){
    if (node == nullptr || node->house.price == price){
        return node;
    }
    if (price < node->house.price){
        return search(node->left, price);
    }
    return search(node->right, price);
}

std::vector<RBTree*> RedBlackTree::price_range(int min, int max){
    std::vector<RBTree*> result;
    price_range_helper(root, min, max, result);
    return result;
}

void RedBlackTree::price_range_helper(RBTree* node, int min, int max, std::vector<RBTree*>& result){
    if (!node){
        return;
    }
    if (min < node->house.price){
        price_range_helper(node->left, min, max, result);
    }
    if (min <= node->house.price && node->house.price <= max){
        result.push_back(node);
    }
    if (max > node->house.price){
        price_range_helper(node->right, min, max, result);
    }
}

void RedBlackTree::inorder_traversal(RBTree* node, std::map<float, Color>& result){
    if (node == nullptr){
        return;
    }
    inorder_traversal(node->left, result);
    result[node->house.price] = node->color_node;
    inorder_traversal(node->right, result);
}

RedBlackTree::~RedBlackTree(){
    delete_tree(root);
}

void RedBlackTree::delete_tree(RBTree* node){
    if (!node){
        return;
    }
    delete_tree(node->left);
    delete_tree(node->right);
    delete node;
}