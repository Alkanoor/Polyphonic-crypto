#ifndef STRING_TREE_HPP
#define STRING_TREE_HPP


#include <functional>
#include <algorithm>
#include <ostream>
#include <vector>
#include <string>
#include <map>


class String_Tree
{
    public:
        String_Tree();

        void add_string(const std::string& s);
        void add_strings(const std::vector<std::string>& v);
        void add_string_with_weight(const std::string& s, float weight);
        void add_strings_with_weight(const std::vector<std::string>& v, std::vector<float>& weights);

        bool is_completely_in_tree(const std::string& s) const;
        bool is_partially_in_tree(const std::string& s) const;
        bool is_completely_in_tree_or_partially_from_size(const std::string& s, int limit_size = 5, int cur_size = 0) const;
        float get_weight(const std::string& s) const;

        bool get_all_children_from_size(const std::string& cur, std::vector<std::string>& children_result, int size = 5, const std::string& model=std::string()) const;
        bool select_all_from_pattern(std::vector<std::string>& result, const std::string& pattern="????", const std::string& cur="") const;

        std::ostream& show(std::ostream& os, const std::string& cur=std::string());

        template <size_t N>
        void select(const std::array<std::vector<char>, N>& forbidden, std::vector<std::string>& granted_Ngrams, std::vector<float>& associated_values) const;
        void select_with_oracle(const std::function<bool(const std::string&)>& oracle, std::vector<std::string>& granted_Ngrams, std::vector<float>& associated_values) const;

    private:
        float weight;
        bool terminal;
        std::map<char, String_Tree> children;

        template <size_t N>
        void select_recursive(const std::array<std::vector<char>, N>& forbidden, std::vector<std::string>& granted_Ngrams, std::vector<float>& associated_values, int& cur_index, size_t bloc_size, int cur_depth, const std::string& prefix) const;
        void select_recursive_with_oracle(const std::function<bool(const std::string&)>& oracle, std::vector<std::string>& granted_Ngrams, std::vector<float>& associated_values, int& cur_index, size_t bloc_size, int cur_depth, const std::string& prefix) const;
};


template <size_t N>
void String_Tree::select(const std::array<std::vector<char>, N>& forbidden, std::vector<std::string>& granted_Ngrams, std::vector<float>& associated_values) const
{
    size_t bloc_size = granted_Ngrams.size();
    if(!bloc_size)
    {
        bloc_size = 100;
        granted_Ngrams.resize(bloc_size);
    }
    associated_values.resize(bloc_size);
    int cur_index = 0;
    select_recursive<N>(forbidden, granted_Ngrams, associated_values, cur_index, bloc_size, 0, "");
}

template <size_t N>
void String_Tree::select_recursive(const std::array<std::vector<char>, N>& forbidden, std::vector<std::string>& granted_Ngrams, std::vector<float>& associated_values, int& cur_index, size_t bloc_size, int cur_depth, const std::string& prefix) const
{
    if(cur_depth>=4)
    {
        granted_Ngrams[cur_index] = prefix;
        associated_values[cur_index] = weight;
        cur_index++;
        if(cur_index%bloc_size==0)
        {
            granted_Ngrams.resize(granted_Ngrams.size()+bloc_size);
            associated_values.resize(associated_values.size()+bloc_size);
        }
        return;
    }

    for(auto it : children)
        if(std::find(forbidden[cur_depth].begin(), forbidden[cur_depth].end(), it.first) == forbidden[cur_depth].end())
            it.second.select_recursive<N>(forbidden, granted_Ngrams, associated_values, cur_index, bloc_size, cur_depth+1, prefix+it.first);
}


#endif
