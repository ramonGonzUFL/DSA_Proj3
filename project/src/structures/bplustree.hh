#pragma once
#include <cstddef>
#include <iostream>
#include <limits>
#include <vector>
#include <memory>
#include <functional>
#include <queue>
#include <algorithm>
#include "lib.hh"

template <typename K, typename V>
class BPlusTree {
private:
    struct Node {
        bool isLeaf{};
        std::vector<K> keys;
        Node* parent = nullptr;
        virtual ~Node() = default;
    };
    struct InternalNode : Node{
        std::vector<Node*> children;

        InternalNode(){
            this->isLeaf = false;
        }
    };
    struct LeafNode : Node {
        std::vector<V> values;
        LeafNode* next;
        LeafNode() : next(nullptr) {this->isLeaf = true;}
    };
    Node* root;
    size_t order;
    LeafNode* findLeaf(K key);
    void splitLeaf(LeafNode* leaf);
    void insertIntoParent(Node* olderChild, K key, Node* newChild);
    void splitInternal(InternalNode* node);



public:
    explicit BPlusTree(int order);
    void insert(K key, V value);
    void printLeaves();
    void printTree();
    V* search(K key);
    void printRange(const K& low, const K& high);



};
template <typename K, typename V>
void BPlusTree<K,V>::printRange(const K& low, const K& high) {
    if (!root) {
        std::cout << "Empty Tree" << std::endl;
        return;
    }
    //Find the first leaf that could be the low value
    LeafNode* leaf = findLeaf(low);

    //walk to the first key >= low in that leaf
    auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(),low);
    size_t idx = it - leaf->keys.begin();
    std::cout << "[" << low << "-" << high << "]:\n" ;

    bool first = true;
    //scan leaf pages until > high
    while (leaf) {
        for (size_t i = idx; i<leaf->keys.size(); ++i) {
            K key = leaf->keys[i];
            if (key > high) {
                std::cout<<"\n";
                return;
            }
            if (!first) {
                std::cout<<",";
            }
            first = false;
            std::cout<<key;
        }
        //next leaf
        leaf = leaf->next;
        idx=0;
    }
    std::cout<<"\n";
}
template<typename K, typename V>
void BPlusTree<K, V>::printTree() {
    if (!root) {
        std::cout << "(empty)\n";
        return;
    }

    std::queue<Node*> q;
    q.push(root);

    while (!q.empty()) {
        int levelSize = q.size();
        // print all nodes on this level
        for (int i = 0; i < levelSize; ++i) {
            Node* cur = q.front();
            q.pop();

            // print this node’s keys
            std::cout << "[";
            for (size_t j = 0; j < cur->keys.size(); ++j) {
                std::cout << cur->keys[j]
                          << (j + 1 < cur->keys.size() ? "," : "");
            }
            std::cout << "]";

            // space between siblings
            if (i + 1 < levelSize)
                std::cout << "   ";

            // enqueue children
            if (!cur->isLeaf) {
                auto in = static_cast<InternalNode*>(cur);
                for (Node* child : in->children)
                    q.push(child);
            }
        }

        // end of this level
        std::cout << "\n";
    }
}

template <typename K, typename V>
void BPlusTree<K, V>::printLeaves() {
    //go to leftmost leaf
    Node* current = root;
    while (!current->isLeaf) {
        current = static_cast<InternalNode*>(current)->children[0];
    }
    LeafNode* leaf = static_cast<LeafNode*>(current);
    std::cout << "Leaf nodes: "<< std::endl;
    while(leaf!=nullptr){
        std::cout << "[";
        for(size_t i = 0; i < leaf->keys.size(); ++i){
            std::cout << leaf->keys[i]<<" ";
        }
        std::cout << "]" << std::endl;
        leaf = leaf->next;
    }
}

template <typename K, typename V>
V* BPlusTree<K, V>::search(K key) {
    // 1) Find the first leaf that *could* contain the key
    LeafNode* leaf = findLeaf(key);

    // 2) Walk forward through the linked list of leaves
    while (leaf) {
        // 3) Scan this leaf’s keys linearly
        for (size_t i = 0; i < leaf->keys.size(); ++i) {
            if (leaf->keys[i] == key) {
                // found it!
                return &leaf->values[i];
            }
        }

        // 4) If this leaf’s largest key is already > our search key, we can stop
        if (!leaf->keys.empty() && leaf->keys.back() > key) {
            break;
        }

        // 5) Otherwise advance to the next leaf in the linked list
        leaf = leaf->next;
    }

    // not found
    return nullptr;
}
template<typename K, typename V>
void BPlusTree<K, V>::splitInternal(InternalNode* node) {
    // pick the median
    int mid = node->keys.size()/2;
    K promote = node->keys[mid];

    // build the new right‐hand node
    InternalNode* right = new InternalNode();
    // move keys *after* the promoted one
    right->keys.assign(
      node->keys.begin() + mid + 1,
      node->keys.end()
    );
    // move children *after* that position
    right->children.assign(
      node->children.begin() + mid + 1,
      node->children.end()
    );

    // shrink the original (left) node
    node->keys.resize(mid);
    node->children.resize(mid + 1);

    // fix parent pointers on the right node’s children
    for (auto c : right->children)
        c->parent = right;

    // push the promote key & the right node up
    if (!node->parent) {
        // brand‑new root
        InternalNode* R = new InternalNode();
        R->keys.push_back(promote);
        R->children = { node, right };
        node->parent = right->parent = R;
        root = R;
    } else {
        insertIntoParent(node, promote, right);
    }
}

template <typename K, typename V>
void BPlusTree<K, V>::insertIntoParent(Node* olderChild, K key, Node* newChild) {

    InternalNode* parent = dynamic_cast<InternalNode*>(olderChild->parent);
    if (olderChild->parent == nullptr) {
        std::cerr << "[insertIntoParent] ERROR: olderChild has no parent! Cannot insert into parent." << std::endl;
        return;
    }

    if (!olderChild->parent) {
        std::cerr << "[insertIntoParent]  ERROR: olderChild has no parent!" << std::endl;
        return;
    }


    if (!parent) {
        std::cerr << "[insertIntoParent]  ERROR: parent is not an InternalNode (bad cast)." << std::endl;
        return;
    }


    //find the index of oldChild in parent's children
    size_t insertPos = std::numeric_limits<size_t>::max();
    for (size_t i = 0; i < parent->children.size(); ++i) {
        if (parent->children[i] == olderChild) {
            insertPos = i;
            break;
        }
    }

    if (insertPos == std::numeric_limits<size_t>::max()) {
        std::cerr << "[insertIntoParent]  ERROR: olderChild not found in parent->children!" << std::endl;
        return;
    }
    //Insert the key and new child at the correct position
    parent->keys.insert(parent->keys.begin() + insertPos, key);
    if (insertPos + 1 > parent->children.size() + 1) {
        std::cerr << "[insertIntoParent]  ERROR: insertPos + 1 would be out of bounds after insertion!" << std::endl;
        return;
    }



    parent->children.insert(parent->children.begin() + insertPos+1, newChild);
    if (newChild != nullptr){
        newChild->parent = parent;
    }

    //split parent if over capacity
    if (parent->keys.size() >= order) {
        splitInternal(parent);

    }
}
//split leaf
template <typename K, typename V>
void BPlusTree<K, V>::splitLeaf(LeafNode* leaf) {
    //std::cout << "[splitLeaf] Splitting leaf with " << leaf->keys.size() << " keys" << std::endl;
    int mid = leaf->keys.size() / 2;

    LeafNode* newLeaf = new LeafNode();

    //moves second half of keys to the second half
    newLeaf->keys.assign(leaf->keys.begin() + mid, leaf->keys.end());
    newLeaf->values.assign(leaf->values.begin() + mid, leaf->values.end());

    //shrink original leaf
    leaf->keys.resize(mid);
    leaf->values.resize(mid);

    //link new leaves
    newLeaf->next = leaf->next;
    leaf->next = newLeaf;

    //promote first key of the new leaf
    K promotedKey = newLeaf->keys[0];
   // std::cout << "[splitLeaf] Promoted key: " << promotedKey << std::endl;

    //Handle the parent node
    if (leaf->parent == nullptr) {
        //create new root
        InternalNode* newRoot = new InternalNode();
        newRoot->keys.push_back(promotedKey);
        newRoot->children.push_back(leaf);
        newRoot->children.push_back(newLeaf);
        root = newRoot;
        leaf->parent = newRoot;
        newLeaf->parent = newRoot;

    }else {
        if (leaf->parent == nullptr) {
            std::cerr << "[splitLeaf]  WARNING: leaf->parent is still null after root split!" << std::endl;
        }
        insertIntoParent(leaf, promotedKey, newLeaf);
    }

}
//Find Leaf
template <typename K, typename V>
typename BPlusTree<K, V>::LeafNode* BPlusTree<K, V>::findLeaf(K key) {
    Node* current = root;
    /*Navigates the tree by comparing the key to the keys in
      InternalNode's each node tells you go this way until you hit a leaf
      once it reaches the leaf node it returns that leaf.
    */
    while (!current->isLeaf) {
        InternalNode* internal = static_cast<InternalNode*>(current);
        size_t i = 0;
        while (i < internal->keys.size() && key >= internal->keys[i]) {
            ++i;
        }
        current = internal->children[i];
    }

    return static_cast<LeafNode*>(current);
}
//Insert
template <typename K, typename V>
void BPlusTree<K, V>::insert(K key, V value) {
    /*If the tree is empty, create a root leaf node by instantiating
      leaf, then pushing the key onto leaf's keys, then same with values,
      then set the root = leaf
     */
    if (root == nullptr) {
        LeafNode* leaf = new LeafNode();
        leaf->keys.push_back(key);
        leaf->values.push_back(value);
        root = leaf;
        return;
    }
    //casts the leaf node to a leaf node
    LeafNode* leaf = findLeaf(key);

    //finds where to insert the key
    auto it = lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
    int index = it - leaf->keys.begin();

    leaf->keys.insert(leaf->keys.begin() + index, key);
    leaf->values.insert(leaf->values.begin() + index, value);
    if(leaf->keys.size() > order){
        splitLeaf(leaf);
    }
}
template <typename K, typename V>
BPlusTree<K, V>::BPlusTree(int order) : root(nullptr), order(order) {}


extern template class BPlusTree<int, std::string>;
extern template class BPlusTree<std::string, House>;







