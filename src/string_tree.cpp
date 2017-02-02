#include "../include/string_tree.hpp"


String_Tree::String_Tree() :
    weight(0),
    terminal(false)
{}

void String_Tree::add_string(const std::string& s)
{
    if(!s.size())
    {
        terminal = true;
        return;
    }
    children[s[0]].add_string(s.substr(1));
}

void String_Tree::add_strings(const std::vector<std::string>& v)
{
    for(auto s : v)
        add_string(s);
}

void String_Tree::add_string_with_weight(const std::string& s, float weight)
{
    if(!s.size())
    {
        terminal = true;
        this->weight = weight;
        return;
    }
    children[s[0]].add_string_with_weight(s.substr(1), weight);
}

void String_Tree::add_strings_with_weight(const std::vector<std::string>& v, std::vector<float>& weights)
{
    for(size_t i=0; i<v.size(); i++)
        add_string_with_weight(v[i], weights[i]);
}


bool String_Tree::is_completely_in_tree(const std::string& s) const
{
    if(!s.size()&&terminal)
        return true;
    else if(!s.size())
        return false;

    if(!children.count(s[0]))
        return false;
    else
        return children.at(s[0]).is_completely_in_tree(s.substr(1));
}

bool String_Tree::is_partially_in_tree(const std::string& s) const
{
    if(!s.size())
        return true;

    if(!children.count(s[0]))
        return false;
    else
        return children.at(s[0]).is_partially_in_tree(s.substr(1));
}

bool String_Tree::is_completely_in_tree_or_partially_from_size(const std::string& s, int limit_size, int cur_size) const
{
    if(!s.size()&&terminal)
        return true;
    else if(!s.size()&&cur_size>=limit_size)
        return true;
    else if(!s.size())
        return false;

    if(!children.count(s[0]))
        return false;
    else
        return children.at(s[0]).is_completely_in_tree_or_partially_from_size(s.substr(1), limit_size, cur_size+1);
}

float String_Tree::get_weight(const std::string& s) const
{
    if(!s.size())
        return weight;

    if(!children.count(s[0]))
        return 0;
    else
        return children.at(s[0]).get_weight(s.substr(1));
}

bool String_Tree::get_all_children_from_size(const std::string& cur, std::vector<std::string>& children_result, int size, const std::string& model) const
{
    if(!cur.size())
    {
        if(terminal)
            children_result.push_back(model);
        if((int)model.size()>=size)
            for(auto i : children)
            {
                std::string model2 = model+i.first;
                i.second.get_all_children_from_size(cur, children_result, size, model2);
            }

        return true;
    }

    if(!children.count(cur[0]))
        return false;

    if(!model.size())
        return children.at(cur[0]).get_all_children_from_size(cur.substr(1), children_result, size, cur);
    else
        return children.at(cur[0]).get_all_children_from_size(cur.substr(1), children_result, size, model);
}

bool String_Tree::select_all_from_pattern(std::vector<std::string>& result, const std::string& pattern, const std::string& cur) const
{
    if(!pattern.size())
    {
        result.push_back(cur);
        return true;
    }

    bool to_return = false;
    if(pattern[0]>='A' && pattern[0]<='Z')
        if(!children.count(pattern[0]))
            return false;
        else
            to_return = children.at(pattern[0]).select_all_from_pattern(result, pattern.substr(1), cur+pattern[0]);
    else
        for(auto i : children)
            to_return = i.second.select_all_from_pattern(result, pattern.substr(1), cur+i.first) || to_return;
    return to_return;
}

std::ostream& String_Tree::show(std::ostream& os, const std::string& cur)
{
    if(terminal)
        os<<cur<<" => "<<weight<<std::endl;
    for(auto i : children)
    {
        std::string cur2 = cur+i.first;
        i.second.show(os, cur2);
    }

    return os;
}

void String_Tree::select_with_oracle(const std::function<bool(const std::string&)>& oracle, std::vector<std::string>& granted_Ngrams, std::vector<float>& associated_values) const
{
    size_t bloc_size = granted_Ngrams.size();
    if(!bloc_size)
    {
        bloc_size = 100;
        granted_Ngrams.resize(bloc_size);
    }
    associated_values.resize(bloc_size);
    int cur_index = 0;
    select_recursive_with_oracle(oracle, granted_Ngrams, associated_values, cur_index, bloc_size, 0, "");
}

void String_Tree::select_recursive_with_oracle(const std::function<bool(const std::string&)>& oracle, std::vector<std::string>& granted_Ngrams, std::vector<float>& associated_values, int& cur_index, size_t bloc_size, int cur_depth, const std::string& prefix) const
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
        if(oracle(prefix+it.first))
            it.second.select_recursive_with_oracle(oracle, granted_Ngrams, associated_values, cur_index, bloc_size, cur_depth+1, prefix+it.first);
}
