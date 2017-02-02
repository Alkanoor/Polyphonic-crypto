#include "../include/polyalphabetic_key.hpp"

#include <algorithm>
#include <stdexcept>


Polyalphabetic_Key::Polyalphabetic_Key(int key_length) :
    key_length_(key_length)
{
    correspondance.resize(key_length);
}

const std::array<std::vector<char>, 4>& Polyalphabetic_Key::get_forbidden_chars(int index, const std::string& cur_ngram) throw ()
{
    if(cur_ngram.size() != 4)
        throw std::runtime_error("Ngram is not a 4 gram in Polyalphabetic_Key");

    for(int i=0; i<4; i++)
    {
        if(cur_ngram[i]<'A'||cur_ngram[i]>'Z')
        {
            std::string tmp;
            tmp.push_back(cur_ngram[i]);
            throw std::runtime_error("Ngram is not caps locked : '"+tmp+"'");
        }
        std::vector<char> tmp;
        if(correspondance[index%key_length_].count(cur_ngram[i]))
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

char Polyalphabetic_Key::get_correspondance(int index, char cipher) const
{
    if(correspondance.at(index%key_length_).count(cipher))
        return correspondance.at(index%key_length_).at(cipher);
    return '?';
}

#include <iostream>
std::string Polyalphabetic_Key::decipher(const std::string& cipher, std::vector<bool>& ngrams_set)
{
    std::cout<<key_length_<<std::endl;
    ngrams_set.resize(cipher.size()-3);
    std::string ret = cipher;
    int cur = 0;
    for(int i=0; i<(int)cipher.size(); i++)
    {
        std::cout<<"cur, i : "<<cur<<" "<<i<<std::endl;
        if(cur>=3)
            ngrams_set[i-3] = true;
        else if(i>=3)
            ngrams_set[i-3] = false;
        std::cout<<i<<" "<<cipher[i]<<std::endl;

        if(correspondance[i%key_length_].count(cipher[i]))
        {
            ret[i] = correspondance[i%key_length_][cipher[i]];
            cur++;
        }
        else
        {
            ret[i] = '?';
            cur = 0;
        }
    }
    std::cout<<ret<<std::endl;
    return ret;
}

Polyalphabetic_Key Polyalphabetic_Key::derive_add(int index, const std::string& Ngram_ciphered, const std::string& Ngram_plain) throw ()
{
    if(Ngram_ciphered.size()!=4 || Ngram_plain.size()!=4)
        throw std::runtime_error("Ngrams are not 4grams in derivation process add");

    Polyalphabetic_Key ret = *this;

    std::cout<<this<<" "<<correspondance.size()<<" "<<key_length_<<std::endl;
    for(auto i : ret.correspondance)
        for(auto j : i)
            std::cout<<j.first<<" "<<j.second<<" in cor"<<std::endl;

    for(int i=0; i<4; i++)
        if(correspondance[(i+index)%key_length_].count(Ngram_ciphered[i]) &&
           correspondance[(i+index)%key_length_][Ngram_ciphered[i]] != Ngram_plain[i] &&
           correspondance[(i+index)%key_length_][Ngram_ciphered[i]] != '?')
        {
            std::cout<<Ngram_ciphered<<" with index "<<i<<" "<<Ngram_plain<<std::endl;
            throw std::runtime_error("Minimum match in correspondance not reached in add");
        }
        else
            ret.correspondance[(i+index)%key_length_][Ngram_ciphered[i]] = Ngram_plain[i];

    std::cout<<this<<" "<<correspondance.size()<<" "<<key_length_<<" at index "<<index<<std::endl;
    for(int i=0; i<ret.correspondance.size(); i++)
        for(auto j : ret.correspondance[i])
            std::cout<<j.first<<" "<<j.second<<" in cor at index "<<i<<std::endl;

    return ret;
}

Polyalphabetic_Key Polyalphabetic_Key::derive_delete(int index, const std::string& Ngram_ciphered) throw ()
{
    if(Ngram_ciphered.size()!=4)
        throw std::runtime_error("Ngram is not 4gram in derivation process delete");

    Polyalphabetic_Key ret = *this;

    for(int i=0; i<4; i++)
        if(!correspondance[(i+index)%key_length_].count(Ngram_ciphered[i]) ||
           correspondance[(i+index)%key_length_][Ngram_ciphered[i]] == '?')
            throw std::runtime_error("Minimum match in correspondance not reached in delete");
        else
            ret.correspondance[(i+index)%key_length_][Ngram_ciphered[i]] = '?';

    return ret;
}

bool Polyalphabetic_Key::oracle(int index, const std::string& cur_ngram, const std::string& to_fit) throw ()
{
    //please be insured that get_forbidden_chars has been previously called

    size_t cur_depth = to_fit.size()-1;
    int i = (int)cur_depth;
    i -= key_length_;
    while(i>=0)
    {
        if(to_fit[i]==to_fit[cur_depth] && cur_ngram[i]!=cur_ngram[cur_depth])
            return false;
        i -= key_length_;
    }
    return std::find(forbidden[cur_depth].begin(), forbidden[cur_depth].end(), to_fit[cur_depth]) == forbidden[cur_depth].end();
}

void Polyalphabetic_Key::reset()
{
    for(auto i : correspondance)
        i.clear();
}
