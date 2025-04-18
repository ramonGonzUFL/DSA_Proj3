//
// Created by Kaylee Zamora on 4/18/25.
//
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
}