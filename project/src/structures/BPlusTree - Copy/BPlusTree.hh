//
// Created by BoysS on 4/17/2025.
//

#ifndef BPLUSSTREE_H
#define BPLUSSTREE_H

#include <iostream>
#include <vector>
#include <memory>
#include <functional>

using namespace std;
// use a different house structure
struct HouseInfo {
  string address;
  double price;
  int squareFootage;
  int bedrooms;
  double bathrooms;

  // Default constructor
  HouseInfo() : address(""), price(0.0), squareFootage(0), bedrooms(0), bathrooms(0.0) {}

  // Custom constructor
  HouseInfo(string addy, double pr, int sqF, int bed, double bath)
      : address(addy), price(pr), squareFootage(sqF), bedrooms(bed), bathrooms(bath) {}
};

template <typename K, typename V>
class BPlusTree {
private:
    struct Node {
      bool isLeaf;
      vector<K> keys;
      Node* parent;
      virtual ~Node() = default;
    };
    struct InternalNode : Node{
      vector<Node*> children;

      InternalNode(){
        this->isLeaf = false;
      }
    };
    struct LeafNode : Node {
      vector<V> values;
      LeafNode* next;
      LeafNode() : next(nullptr) {this->isLeaf = true;}
    };
    Node* root;
    int order;
    LeafNode* findLeaf(K key);
    void splitLeaf(LeafNode* leaf);
    void insertIntoParent(Node* olderChild, K key, Node* newChild);
    void splitInternal(InternalNode* node);



public:
  BPlusTree(int order);
  void insert(K key, V value);
  void printLeaves();
  void printTree();
  V* search(K key);

};
template <typename K, typename V>
BPlusTree<K, V>::BPlusTree(int order) : root(nullptr), order(order) {}

template <typename K, typename V>

//Insert
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

//split leaf
template <typename K, typename V>
void BPlusTree<K, V>::splitLeaf(LeafNode* leaf) {
  cout << "[splitLeaf] Splitting leaf with " << leaf->keys.size() << " keys" << endl;
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
  cout << "[splitLeaf] Promoted key: " << promotedKey << endl;

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
      cerr << "[splitLeaf] ⚠️ WARNING: leaf->parent is still null after root split!" << endl;
    }
    insertIntoParent(leaf, promotedKey, newLeaf);
  }

}
template <typename K, typename V>
void BPlusTree<K, V>::insertIntoParent(Node* olderChild, K key, Node* newChild) {
  if (olderChild->parent == nullptr) {
    cerr << "[insertIntoParent] ERROR: olderChild has no parent! Cannot insert into parent." << endl;
    return;
  }

  if (!olderChild->parent) {
    cerr << "[insertIntoParent] ❌ ERROR: olderChild has no parent!" << endl;
    return;
  }

  InternalNode* parent = dynamic_cast<InternalNode*>(olderChild->parent);
  if (!parent) {
    cerr << "[insertIntoParent] ❌ ERROR: parent is not an InternalNode (bad cast)." << endl;
    return;
  }

  cout << "[insertIntoParent] ✅ Successfully found valid parent node." << endl;


  //find the index of oldChild in parent's children
  int insertPos = -1;
  for (int i = 0; i < parent->children.size(); ++i) {
    if (parent->children[i] == olderChild) {
      insertPos = i;
      break;
    }
  }
  if (insertPos == -1) {
    cerr << "[insertIntoParent] ❌ ERROR: olderChild not found in parent->children!" << endl;
    return;
  }
  cout << "[insertIntoParent] Inserting key " << key << " at position " << insertPos << endl;
  cout << "[insertIntoParent] Parent had " << parent->children.size() << " children before inserting" << endl;

  //Insert the key and new child at the correct position
  parent->keys.insert(parent->keys.begin() + insertPos, key);
  if (insertPos + 1 > parent->children.size() + 1) {
    cerr << "[insertIntoParent] ❌ ERROR: insertPos + 1 would be out of bounds after insertion!" << endl;
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
template <typename K, typename V>
void BPlusTree<K, V>::splitInternal(InternalNode* node) {
  cout << "[splitInternal] Splitting internal node with keys: ";
  for (K k : node->keys) cout << k << " ";
  cout << endl;

  int midIndex = node->keys.size() / 2;
  K promotedKey = node->keys[midIndex];
  cout << "[splitInternal] Promoted key: " << promotedKey << endl;


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
void BPlusTree<K, V>::printLeaves() {
  //go to leftmost leaf
  Node* current = root;
  while (!current->isLeaf) {
    current = static_cast<InternalNode*>(current)->children[0];
  }
  LeafNode* leaf = static_cast<LeafNode*>(current);
  cout << "Leaf nodes: "<< endl;
  while(leaf!=nullptr){
    cout << "[";
    for(int i = 0; i < leaf->keys.size(); ++i){
      cout << leaf->keys[i]<<" ";
    }
    cout << "]" << endl;
    leaf = leaf->next;
  }
  cout << "nullptr" << endl;
}

template <typename K, typename V>
void BPlusTree<K, V>::printTree() {
  cout << "B+ Tree Structure:" << endl;
/*
 //Defines a recursive lambda function called print, taking the current node we're printing,
  the depth in the tree, and captures local variables using &
 */
  function<void(Node*, int)> print = [&](Node* node, int level) {
    for (int i = 0; i < level; ++i) cout << "  ";
    if (node->isLeaf) {
      LeafNode* leaf = static_cast<LeafNode*>(node);
      cout << "Leaf: [ ";
      for (K key : leaf->keys) cout << key << " ";
      cout << "] → ";
    } else {
      InternalNode* internal = static_cast<InternalNode*>(node);
      cout << "Internal: [ ";
      for (K key : internal->keys) cout << key << " ";
      cout << "]" << endl;
      for (Node* child : internal->children) {
        print(child, level + 1);
      }
    }
  };

  if (root) {
    print(root, 0);
  } else {
    cout << "(empty tree)" << endl;
  }
}







#endif // BPLUSTREE_H
