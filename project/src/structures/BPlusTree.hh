//
// Created by BoysS on 4/17/2025.
//

#ifndef BPLUSSTREE_H
#define BPLUSSTREE_H

#include <iostream>
#include <vector>
#include <memory>
#include <functional>

// use a different house structure
struct HouseInfo {
  std::string address;
  double price;
  int squareFootage;
  int bedrooms;
  double bathrooms;

  // Default constructor
  HouseInfo() : address(""), price(0.0), squareFootage(0), bedrooms(0), bathrooms(0.0) {}

  // Custom constructor
  HouseInfo(std::string addy, double pr, int sqF, int bed, double bath)
      : address(addy), price(pr), squareFootage(sqF), bedrooms(bed), bathrooms(bath) {}
};

template <typename K, typename V>
class BPlusTree {
private:
    struct Node {
      bool isLeaf;
      std::vector<K> keys;
      Node* parent;
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



#endif // BPLUSTREE_H
