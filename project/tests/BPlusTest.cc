#include "BPlusTree.h"
#include <iostream>

int main() {
    BPlusTree<int, HouseInfo> tree(3);  // B+ Tree with order 3

    tree.insert(5, HouseInfo("123 Main St", 250000.0, 1400, 3, 2.5));
    cout << "Inserted key: 5" << endl;

    tree.insert(3, HouseInfo("456 Oak Ave", 180000.0, 1100, 2, 1.5));
    cout << "Inserted key: 3" << endl;

    tree.insert(10, HouseInfo("789 Pine Rd", 320000.0, 1600, 4, 3.0));
    cout << "Inserted key: 10" << endl;

    tree.insert(8, HouseInfo("101 Maple Ln", 270000.0, 1450, 3, 2.0));
    cout << "Inserted key: 8" << endl;

    tree.insert(12, HouseInfo("202 Elm Dr", 300000.0, 1500, 3, 2.5));
    cout << "Inserted key: 12" << endl;

    tree.insert(15, HouseInfo("303 Birch Blvd", 350000.0, 1750, 4, 3.5));
    cout << "Inserted key: 15" << endl;

    std::cout << "\nPrinting Tree Structure:\n";
    tree.printTree();

    std::cout << "\nPrinting Leaf Nodes:\n";
    tree.printLeaves();

    return 0;
}
