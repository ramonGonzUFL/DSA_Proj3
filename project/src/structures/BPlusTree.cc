#include "BPlusTree.h"

template <typename K, typename V>
void BPlusTree<K, V>::printTree() {
    std::cout << "B+ Tree Structure:" << std::endl;
    /*
     //Defines a recursive lambda function called print, taking the current node we're printing,
      the depth in the tree, and captures local variables using &
     */
    std::function<void(Node*, int)> print = [&](Node* node, int level) {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        if (node->isLeaf) {
            LeafNode* leaf = static_cast<LeafNode*>(node);
            std::cout << "Leaf: [ ";
            for (K key : leaf->keys) std::cout << key << " ";
            std::cout << "] → ";
        } else {
            InternalNode* internal = static_cast<InternalNode*>(node);
            std::cout << "Internal: [ ";
            for (K key : internal->keys) std::cout << key << " ";
            std::cout << "]" << std::endl;
            for (Node* child : internal->children) {
                print(child, level + 1);
            }
        }
    };

    if (root) {
        print(root, 0);
    } else {
        std::cout << "(empty tree)" << std::endl;
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
        for(int i = 0; i < leaf->keys.size(); ++i){
            std::cout << leaf->keys[i]<<" ";
        }
        std::cout << "]" << std::endl;
        leaf = leaf->next;
    }
    std::cout << "nullptr" << std::endl;
}

template <typename K, typename V>
V* BPlusTree<K, V>::search(K key) {
    LeafNode* leaf = findLeaf(key);
    auto it = lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
    int index = it - leaf->keys.begin();


    if (index < leaf->keys.size() && leaf->keys[index] == key) {
        return &(leaf->values[index]);
    }
    return nullptr;

}
template <typename K, typename V>
void BPlusTree<K, V>::splitInternal(InternalNode* node) {
    std::cout << "[splitInternal] Splitting internal node with keys: ";
    for (K k : node->keys) std::cout << k << " ";
    std::cout << std::endl;

    int midIndex = node->keys.size() / 2;
    K promotedKey = node->keys[midIndex];
    std::cout << "[splitInternal] Promoted key: " << promotedKey << std::endl;


    //creates a new internal node
    InternalNode* newNode = new InternalNode();

    //Move Keys AFTER the promoted key
    newNode->keys.assign(node->keys.begin() + midIndex + 1, node->keys.end());
    node->keys.resize(midIndex); // removes keys that were moved and the promoted key

    //move the corresponding children
    newNode->children.assign(node->children.begin() + midIndex + 1, node->children.end());
    node->children.resize(midIndex+1); //keep children up to and including midIndex

    //update parent pointers of newNodes children
    for (Node* child : newNode->children) {
        child->parent = newNode;  // ✅ correct
    }
    // ✅ Fix: update parent pointers for remaining children
    for (Node* child : node->children) {
        child->parent = node;
    }


    if(node->parent==nullptr){
        //create a new root
        InternalNode* newRoot = new InternalNode();
        newRoot->keys.push_back(promotedKey);
        newRoot->children.push_back(node);
        newRoot->children.push_back(newNode);
        root = newRoot;
        node->parent = newRoot;
        newNode->parent = newRoot;
    }
    else{
        insertIntoParent(node, promotedKey, newNode);
    }
}

template <typename K, typename V>
void BPlusTree<K, V>::insertIntoParent(Node* olderChild, K key, Node* newChild) {
    if (olderChild->parent == nullptr) {
        std::cerr << "[insertIntoParent] ERROR: olderChild has no parent! Cannot insert into parent." << std::endl;
        return;
    }

    if (!olderChild->parent) {
        std::cerr << "[insertIntoParent] ❌ ERROR: olderChild has no parent!" << std::endl;
        return;
    }

    InternalNode* parent = dynamic_cast<InternalNode*>(olderChild->parent);
    if (!parent) {
        std::cerr << "[insertIntoParent] ❌ ERROR: parent is not an InternalNode (bad cast)." << std::endl;
        return;
    }

    std::cout << "[insertIntoParent] ✅ Successfully found valid parent node." << std::endl;


    //find the index of oldChild in parent's children
    int insertPos = -1;
    for (int i = 0; i < parent->children.size(); ++i) {
        if (parent->children[i] == olderChild) {
            insertPos = i;
            break;
        }
    }
    if (insertPos == -1) {
        std::cerr << "[insertIntoParent] ❌ ERROR: olderChild not found in parent->children!" << std::endl;
        return;
    }
    std::cout << "[insertIntoParent] Inserting key " << key << " at position " << insertPos << std::endl;
    std::cout << "[insertIntoParent] Parent had " << parent->children.size() << " children before inserting" << std::endl;

    //Insert the key and new child at the correct position
    parent->keys.insert(parent->keys.begin() + insertPos, key);
    if (insertPos + 1 > parent->children.size() + 1) {
        std::cerr << "[insertIntoParent] ❌ ERROR: insertPos + 1 would be out of bounds after insertion!" << std::endl;
        return;
    }



    parent->children.insert(parent->children.begin() + insertPos + 1, newChild);
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
    std::cout << "[splitLeaf] Splitting leaf with " << leaf->keys.size() << " keys" << std::endl;
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
    std::cout << "[splitLeaf] Promoted key: " << promotedKey << std::endl;

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
        return; // ✅ THIS prevents crashing insertIntoParent call
    }


    else {
        if (leaf->parent == nullptr) {
            std::cerr << "[splitLeaf] ⚠️ WARNING: leaf->parent is still null after root split!" << std::endl;
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
        int i = 0;
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
    if(leaf->keys.size() >= order){
        splitLeaf(leaf);
    }
}
template <typename K, typename V>
BPlusTree<K, V>::BPlusTree(int order) : root(nullptr), order(order) {}





