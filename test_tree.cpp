#include "include/string_tree.hpp"
#include "include/util.hpp"

#include <iostream>
#include <cstdlib>
#include <fstream>


//g++ test_tree.cpp src/string_tree.cpp -o test -std=c++11

void read_strings(const std::string& path, std::vector<std::string>& res)
{
    std::ifstream ifs(path.c_str(), std::ios::in);
    if(!ifs)
    {
        std::cerr<<"Error during file "<<path<<" opening, exiting"<<std::endl;
        exit(1);
    }
    std::string tmp;
    while(ifs>>tmp)
        res.push_back(tmp);
}

void read_strings_with_weight(const std::string& path, std::vector<std::string>& res, std::vector<float>& associated_values)
{
    std::ifstream ifs(path.c_str(), std::ios::in);
    if(!ifs)
    {
        std::cerr<<"Error during file "<<path<<" opening, exiting"<<std::endl;
        exit(1);
    }
    std::string tmp1;
    float tmp2;
    while(ifs>>tmp1>>tmp2)
    {
        res.push_back(tmp1);
        associated_values.push_back(tmp2);
    }
}

int main()
{
    std::vector<std::string> for_tree_double;
    read_strings("data/english_words_reduced.txt", for_tree_double);
    std::vector<std::string> for_tree(for_tree_double.size()/2);
    for(int i=0; i<(int)for_tree.size(); i++)
        for_tree[i] = for_tree_double[i*2];

    Utils::write_vec(for_tree, std::cout, ' ', true);

    String_Tree tree;
    tree.add_strings(for_tree);
    tree.show(std::cout);

    std::cout<<tree.is_completely_in_tree("GOVERNMENT")<<std::endl;
    std::cout<<tree.is_completely_in_tree("GOVERN")<<std::endl;
    std::cout<<tree.is_partially_in_tree("GOVERN")<<std::endl;
    std::cout<<tree.is_completely_in_tree_or_partially_from_size("GOVERN")<<std::endl;
    std::cout<<tree.is_completely_in_tree_or_partially_from_size("GOVER")<<std::endl;
    std::cout<<tree.is_completely_in_tree_or_partially_from_size("GOVE")<<std::endl;
    std::cout<<tree.is_completely_in_tree_or_partially_from_size("GOV")<<std::endl;

    std::vector<std::string> children;
    std::cout<<tree.get_all_children_from_size("GO", children, 2)<<std::endl;
    Utils::write_vec(children, std::cout, ' ', true);

    children.clear();
    std::cout<<tree.get_all_children_from_size("GO", children, 3)<<std::endl;
    Utils::write_vec(children, std::cout, ' ', true);

    children.clear();
    std::cout<<tree.get_all_children_from_size("GC", children, 2)<<std::endl;
    Utils::write_vec(children, std::cout, ' ', true);

    std::array<std::vector<char>, 4> forbidden;
    std::vector<std::string> granted;
    std::vector<float> associated_values;
    forbidden[0].push_back('A');
    forbidden[0].push_back('T');
    forbidden[0].push_back('a');
    forbidden[0].push_back('d');
    forbidden[0].push_back('f');
    forbidden[0].push_back('r');
    forbidden[1].push_back('o');
    forbidden[1].push_back('s');
    forbidden[2].push_back('E');
    forbidden[2].push_back('h');
    forbidden[2].push_back('k');
    forbidden[2].push_back('l');
    forbidden[2].push_back('m');
    forbidden[2].push_back('q');
    forbidden[3].push_back('z');
    tree.add_string_with_weight("abcd", 0.456);
    tree.add_string_with_weight("bbcd", 0.457);
    tree.add_string_with_weight("bocd", 0.458);
    tree.add_string_with_weight("dbcd", 0.459);
    tree.add_string_with_weight("zddh", 0.46);
    tree.add_string_with_weight("zdmh", 0.461);
    tree.add_string_with_weight("zdsh", 0.462);
    tree.select<4>(forbidden, granted, associated_values);

    Utils::write_vec(granted,std::cout)<<std::endl;
    Utils::write_vec(associated_values,std::cout)<<std::endl;

    std::vector<std::string> to_add_string;
    std::vector<float> to_add_float;
    granted.resize(10000);
    read_strings_with_weight("data/english_quadgrams_reduced.txt", to_add_string, to_add_float);
    tree.add_strings_with_weight(to_add_string, to_add_float);
    tree.select<4>(forbidden, granted, associated_values);

    Utils::write_vec(granted,std::cout)<<std::endl;
    Utils::write_vec(associated_values,std::cout)<<std::endl;

    children.clear();
    std::cout<<tree.select_all_from_pattern(children, "?AB?")<<std::endl;
    Utils::write_vec(children, std::cout, ' ', true);

    children.clear();
    std::cout<<tree.select_all_from_pattern(children, "TH?N")<<std::endl;
    Utils::write_vec(children, std::cout, ' ', true);

    return 0;
}
