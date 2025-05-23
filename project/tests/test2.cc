#include "../src/lib.hh"
#include "../src/structures/redblack.hh"
#include <cassert>
#include <map>
#include <vector>
#include <iostream>

House test_value(float price) {
    House h;
    h.price = price;
    return h;
}

int main(){
    try{
        RedBlackTree redblacktree;

        for (int i = 1; i <= 5; ++i){                   //inserts values 1-5 for the price
            redblacktree.insert(test_value(i));
        }
        std::map<float, Color> actual;                  //holds what the actual tree should look like
        redblacktree.inorder_traversal(redblacktree.root, actual);

        std::map<float, Color> expected = {
                {1, Color::Black},
                {2, Color::Black},
                {3, Color::Red},
                {4, Color::Black},
                {5, Color::Red}
        };

        for (std::map<float, Color>::const_iterator it = expected.begin(); it != expected.end(); ++it){
            int price = it->first;
            Color expected_color = it->second;

            if (actual.count(price) == 0){
                std::cerr << "Price " << price << " not found in tree.\n";
                return 1;
            }
            if (actual.at(price) != expected_color){
                std::cerr << "Color does not match at price " << price
                          << ": expected " << expected_color
                          << ", got " << actual.at(price) << "\n";
                return 1;
            }
        }

        //checks if range is correct
        auto range_nodes = redblacktree.price_range(2, 4);
        std::vector<float> expectedrange = {2, 3, 4};
        if (range_nodes.size() != expectedrange.size()){
            std::cerr << "price_range returned " << range_nodes.size()
                      << " nodes; expected " << expectedrange.size() << "\n";
            return 1;
        }
        for (size_t i = 0; i < expectedrange.size(); ++i){
            float got_price = range_nodes[i]->price;
            if (got_price != expectedrange[i]){
                std::cerr << "price_range[" << i << "] = " << got_price
                          << "; expected " << expectedrange[i] << "\n";
                return 1;
            }
        }

        std::cout << "Test passed. Tree matches and range is correct." << std::endl;
        return 0;
    }
    catch(const std::exception& e){
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}