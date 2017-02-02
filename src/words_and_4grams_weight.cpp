#include "../include/words_and_4grams_weight.hpp"


String_Tree Words_And_4grams_Weight::Ngram_tree;

float Words_And_4grams_Weight::to_float() const
{return current_value;}

std::ostream& Words_And_4grams_Weight::show(std::ostream& os) const
{
    os<<"Basic counter :"<<std::endl;
    auto it2 = current_letters_frequencies.begin();
    for(auto it = current_letters_counter.begin();
             it != current_letters_counter.end() && it2 != current_letters_frequencies.end();
             it++, it2++)
        if(it->second>1)
            os<<it->first<<" => "<<it->second<<" || "<<it2->second<<std::endl;
    os<<"Bigrams counter :"<<std::endl;
    auto it3 = current_bigrams_frequencies.begin();
    for(auto it = current_bigrams_counter.begin();
             it != current_bigrams_counter.end() && it3 != current_bigrams_frequencies.end();
             it++, it3++)
        if(it->second>2)
            os<<it->first<<" => "<<it->second<<" || "<<it3->second<<std::endl;
    os<<"Deciphered to "<<cur_plain<<std::endl;
    os<<"With score "<<current_value<<std::endl;
    return os;
}

void Words_And_4grams_Weight::weight(const std::string& cipher, const std::string& plain)
{
    current_letters_counter.clear();
    current_bigrams_counter.clear();
    current_letters_frequencies.clear();
    current_bigrams_frequencies.clear();

    int l1 = 0, l2 = 0;
    float n_4grams = 1;
    current_value = 0;
    cur_plain = plain;
    std::string cur_bigram = "aa";
    bool bad_bigram_after, bad_bigram_now;

    for(int i=0; i<plain.size(); i++)
    {
        if(plain[i]<'A' || plain[i]>'Z')
        {
            bad_bigram_after = true;
            bad_bigram_now = true;
        }
        else
        {
            l1++;
            if(current_letters_counter.count(plain[i]))
                current_letters_counter[plain[i]] ++;
            else
                current_letters_counter[plain[i]] = 1;

            if(bad_bigram_after)
            {
                bad_bigram_after = false;
                bad_bigram_now = true;
            }
            else
            {
                if(bad_bigram_now)
                    bad_bigram_now = false;
                else
                {
                    l2++;
                    cur_bigram[0] = cur_bigram[1];
                    cur_bigram[1] = plain[i];
                    if(i>0 && current_bigrams_counter.count(cur_bigram))
                        current_bigrams_counter[cur_bigram] ++;
                    else if(i>0)
                        current_bigrams_counter[cur_bigram] = 1;
                }
            }
        }

        if(i<cipher.size()-3)
        {
            int bad = 0;
            std::string cur = "????";
            for(int j=0; j<4; j++)
            {
                cur[j] = plain[i+j];
                if(cur[j] < 'A' || cur[j] > 'Z')
                    bad++;
            }
            if(!bad)
            {
                n_4grams++;
                current_value += Ngram_tree.get_weight(cur);
            }
            else if(bad == 1)
            {
                std::vector<std::string> matching_pattern;
                if(Ngram_tree.select_all_from_pattern(matching_pattern, cur))
                {
                    float max = 0, s;
                    for(auto i : matching_pattern)
                    {
                        s = Ngram_tree.get_weight(i);
                        if(s>max)
                            max = s;
                    }
                    current_value += s;
                    n_4grams++;
                }
            }
        }
    }

    if(n_4grams>0)
        current_value /= n_4grams;

    for(auto it : current_letters_counter)
        current_letters_frequencies[it.first] = it.second/(float)l1;
    for(auto it : current_bigrams_counter)
        current_bigrams_frequencies[it.first] = it.second/(float)l2;
}

void Words_And_4grams_Weight::set_4gram_tree(const String_Tree& s)
{Ngram_tree = s;}
