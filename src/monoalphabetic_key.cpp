#include "../include/monoalphabetic_key.hpp"

#include <algorithm>
#include <stdexcept>


const std::array<std::vector<char>, 4>& Monoalphabetic_Key::get_forbidden_chars(int index, const std::string& cur_ngram) throw ()
{
    if(cur_ngram.size() != 4)
        throw std::runtime_error("Ngram is not a 4 gram in Monoalphabetic_Key");

    for(int i=0; i<4; i++)
    {
        if(cur_ngram[i]<'A'||cur_ngram[i]>'Z')
        {
            std::string tmp;
            tmp.push_back(cur_ngram[i]);
            throw std::runtime_error("Ngram is not caps locked : '"+tmp+"'");
        }
        std::vector<char> tmp;
        if(correspondance.count(cur_ngram[i]))
        {
            tmp.resize(25);
            int o=0;
            for(int j=0; j<26; j++)
                if('A'+j != cur_ngram[i])
                {
                    tmp[o] = cur_ngram[i];
                    o++;
                }
        }
        forbidden[i] = tmp;
    }
    return forbidden;
}

char Monoalphabetic_Key::get_correspondance(int index, char cipher) const
{
    if(correspondance.count(cipher))
        return correspondance.at(cipher);
    return '?';
}

std::string Monoalphabetic_Key::decipher(const std::string& cipher, std::vector<bool>& ngrams_set)
{
    ngrams_set.resize(cipher.size()-3);
    std::string ret = cipher;
    int cur = 0;
    for(int i=0; i<(int)cipher.size(); i++)
    {
        if(cur>=3)
            ngrams_set[i-3] = true;
        else if(i>=3)
            ngrams_set[i-3] = false;

        if(correspondance.count(cipher[i]))
        {
            ret[i] = correspondance[cipher[i]];
            cur++;
        }
        else
        {
            ret[i] = '?';
            cur = 0;
        }
    }
    return ret;
}

Monoalphabetic_Key Monoalphabetic_Key::derive_add(int index, const std::string& to_be_replaced_with, const std::string& to_use_for_replacing) throw ()
{
    if(to_be_replaced_with.size()!=4 || to_use_for_replacing.size()!=4)
        throw std::runtime_error("Ngrams are not 4grams in derivation process add");

    Monoalphabetic_Key ret = *this;

    for(int i=0; i<4; i++)
        if(correspondance.count(to_be_replaced_with[i]) &&
           correspondance[to_be_replaced_with[i]] != to_use_for_replacing[i] &&
           correspondance[to_be_replaced_with[i]] != '?')
            throw std::runtime_error("Minimum match in correspondance not reached in add");
        else
            ret.correspondance[to_be_replaced_with[i]] = to_use_for_replacing[i];

    return ret;
}

#include <iostream>
Monoalphabetic_Key Monoalphabetic_Key::derive_delete(int index, const std::string& to_be_replaced_with) throw ()
{
    if(to_be_replaced_with.size()!=4)
        throw std::runtime_error("Ngram is not 4gram in derivation process delete");

    Monoalphabetic_Key ret = *this;

    log(std::cout);
    for(int i=0; i<4; i++)
    {
        if(!correspondance.count(to_be_replaced_with[i]) ||
           correspondance[to_be_replaced_with[i]] == '?')
            throw std::runtime_error("Minimum match in correspondance not reached in delete");
        else
            ret.correspondance[to_be_replaced_with[i]] = '?';
    }

    return ret;
}

bool Monoalphabetic_Key::oracle(int index, const std::string& cur_ngram, const std::string& to_fit) throw ()
{
    if(!to_fit.size())
        return false;

    size_t cur_depth = to_fit.size()-1;
    char correspondance = get_correspondance(index+cur_depth, cur_ngram[cur_depth]);
    if(correspondance != '?' && correspondance != to_fit[cur_depth])
        return false;

    int i = (int)cur_depth;
    i--;
    while(i>=0)
    {
        if((to_fit[i]==to_fit[cur_depth] && cur_ngram[i]!=cur_ngram[cur_depth]) ||
           (cur_ngram[i]==cur_ngram[cur_depth] && to_fit[i]!=to_fit[cur_depth]))
            return false;
        i--;
    }

    return true;
}

void Monoalphabetic_Key::reset()
{
    correspondance.clear();
}

std::ostream& Monoalphabetic_Key::log(std::ostream& os)
{
    for(auto it : correspondance)
        os<<it.first<<" => "<<it.second<<std::endl;
    return os;
}
