#ifndef CRYPTO_OBJECT_HPP
#define CRYPTO_OBJECT_HPP


#include <vector>
#include <string>
#include <array>
#include <map>

#include "logger/include/log_in_file.hpp"
#include "string_tree.hpp"
#include "utils/util.hpp"


#define DEBUG


template <typename Key, typename Weight, size_t N = 2000, size_t M = 5000>
class Crypto_Object
{
    public:
        Crypto_Object();
        Crypto_Object(const Key& k);

        void get_a_following_object(Crypto_Object& res) throw ();
        void get_a_previous_object(Crypto_Object& res) throw ();

        const Weight& get_weight() const;
        const std::string& get_clear() const;

        void set_key(const Key& k);

        static void set_cipher(const std::string& txt);
        static void set_4gram_tree(const String_Tree& s);
        static void set_logger(const std::shared_ptr<Info_Warning_Error_Logger>& logger = Easy_Log_In_File::getInfoLog());

    private:
        std::vector<bool> ngrams_set;
        std::string cur_deciphered;
        static std::string cipher;

        Weight cur_weight;
        Key key;

        static String_Tree Ngram_tree;
        static std::shared_ptr<Info_Warning_Error_Logger> logger;

        void get_4grams_counter(std::map<std::string, std::pair<int, int> >& Ngrams_counter, bool in_cur_key = false);
};


template <typename Key, typename Weight, size_t N, size_t M>
String_Tree Crypto_Object<Key, Weight, N, M>::Ngram_tree;

template <typename Key, typename Weight, size_t N, size_t M>
std::string Crypto_Object<Key, Weight, N, M>::cipher;

template <typename Key, typename Weight, size_t N, size_t M>
std::shared_ptr<Info_Warning_Error_Logger> Crypto_Object<Key, Weight, N, M>::logger;


template <typename Key, typename Weight, size_t N, size_t M>
Crypto_Object<Key, Weight, N, M>::Crypto_Object()
{
    if(!logger)
        logger = Easy_Log_In_File::getInfoLog();
}

template <typename Key, typename Weight, size_t N, size_t M>
Crypto_Object<Key, Weight, N, M>::Crypto_Object(const Key& k) :
    key(k)
{
    cur_deciphered = key.decipher(cipher, ngrams_set);
    cur_weight.weight(cipher, cur_deciphered);
    if(!logger)
        logger = Easy_Log_In_File::getInfoLog();
}

template <typename Key, typename Weight, size_t N, size_t M>
void Crypto_Object<Key, Weight, N, M>::get_a_following_object(Crypto_Object& res) throw ()
{
    if(cipher.size()<4)
        throw std::runtime_error("Error cipher too small in following object");

    if(ngrams_set.size()+3 != cipher.size())
    {
        ngrams_set.resize(cipher.size()-3);
        for(size_t i=0; i<ngrams_set.size(); i++)
            ngrams_set[i] = false;
        key.reset();
    }

    std::map<std::string, std::pair<int, int> > Ngrams_counter;
    get_4grams_counter(Ngrams_counter);

    if(!Ngrams_counter.size())
    {
        get_a_previous_object(res);
        return;
    }

    #ifdef DEBUG
        (*logger)<<"=============================";
        logger->endLine();
        (*logger)<<"New forwarding key generation";
        logger->endLine();
        (*logger)<<"=============================";
        logger->endLine();
    #endif

    if(N<Ngrams_counter.size())
    {
        (*logger)<<"[-] Ouch not enough forseen range for array : "<<N<<" instead of "<<Ngrams_counter.size();
        logger->endLine();
        return;
    }

    std::array<float, N> qualities;
    std::array<std::string, N> Ngrams;

    auto it = Ngrams_counter.begin();
    for(int i=0; i<N; i++)
    {
        if(it != Ngrams_counter.end())
        {
            qualities[i] = it->second.first*it->second.first*it->second.first;
            Ngrams[i] = it->first;
            #ifdef DEBUG
                (*logger)<<"Add a 4gram "<<Ngrams[i]<<" with weight "<<qualities[i];
                logger->endLine();
            #endif
            it++;
        }
        else
            qualities[i] = 0;
    }

    Quality_Selection<N, float> q;
    int chosen = q.apply(qualities)[0]; //chosen is index of the cipher quadrigram which will be replaced

    #ifdef DEBUG
        (*logger)<<chosen<<" (that leads to "<<Ngrams_counter[Ngrams[chosen]].second<<")"<<" has been chosen to be replaced, it matches 4gram "<<Ngrams[chosen]<<" with weight "<<qualities[chosen]<<" ("<<cur_deciphered<<")";
        logger->endLine();
    #endif

    // std::array<std::vector<char>, 4> forbidden = key.get_forbidden_chars(chosen, Ngrams[chosen]);
    //key.get_forbidden_chars(chosen, Ngrams[chosen]);
    std::function<bool(const std::string&)> key_oracle = std::bind(&Key::oracle, key, chosen, Ngrams[chosen], std::placeholders::_1);
    std::vector<std::string> granted_4grams(10000);
    std::vector<float> associated_values(10000);
    Ngram_tree.select_with_oracle(key_oracle, granted_4grams, associated_values);

    std::vector<int> sorted;

    Utils::index_after_sorting<float>(associated_values, sorted);
    #ifdef DEBUG
        (*logger)<<"After sort we get "<<associated_values[sorted[0]]<<" "<<associated_values[sorted[1]]<<" "<<associated_values[sorted[2]]<<" ";
        logger->endLine();
    #endif

    std::array<std::string, M> to_keep_strings;
    std::array<float, M> to_keep_associated;
    for(int i=0; i<M && i<(int)sorted.size(); i++)
    {
        to_keep_associated[i] = associated_values[sorted[i]];
        to_keep_strings[i] = granted_4grams[sorted[i]];
    }
    for(int i=(int)sorted.size(); i<M; i++)
        to_keep_associated[i] = 0;

    Quality_Selection<M, float> q2;
    int chosen2 = q2.apply(to_keep_associated)[0]; //chosen2 is index of the plain quadrigram which will replace ciphered chosen one
    #ifdef DEBUG
        (*logger)<<chosen2<<" has been chosen to replace, it matches 4gram "<<to_keep_strings[chosen2]<<" with weight "<<to_keep_associated[chosen2];
        logger->endLine();
    #endif
    Key key2 = key.derive_add(Ngrams_counter[Ngrams[chosen]].second, Ngrams[chosen], to_keep_strings[chosen2]);
    res = Crypto_Object(key2);
}
#include <iostream>
template <typename Key, typename Weight, size_t N, size_t M>
void Crypto_Object<Key, Weight, N, M>::get_a_previous_object(Crypto_Object& res) throw ()
{
    if(cipher.size()<4)
        throw std::runtime_error("Error cipher too small in following object");

    if(ngrams_set.size()+3 != cipher.size())
    {
        ngrams_set.resize(cipher.size()-3);
        for(size_t i=0; i<ngrams_set.size(); i++)
            ngrams_set[i] = false;
        key.reset();
    }

    std::map<std::string, std::pair<int, int> > Ngrams_counter;
    get_4grams_counter(Ngrams_counter, true);

    if(!Ngrams_counter.size())
    {
        get_a_previous_object(res);
        return;
    }

    #ifdef DEBUG
        (*logger)<<"==========================";
        logger->endLine();
        (*logger)<<"New backing key generation";
        logger->endLine();
        (*logger)<<"==========================";
        logger->endLine();
    #endif

    int cur = 0;
    std::array<float, N> qualities;
    std::map<int, int> correspondances;
    for(int i=0; i<(int)cipher.size()-3; i++)
        if(ngrams_set[i])
        {
            std::string cur_4gram_plain = "aaaa";
            std::string cur_4gram_ciphered = "aaaa";
            for(int j=0; j<4; j++)
            {
                cur_4gram_ciphered[j] = cipher[i+j];
                cur_4gram_plain[j] = key.get_correspondance(i+j, cipher[i+j]);
            }

            qualities[cur] = log((float)Ngrams_counter[cur_4gram_ciphered].first*Ngram_tree.get_weight(cur_4gram_plain));
            #ifdef DEBUG
                (*logger)<<"For 4gram "<<cur_4gram_ciphered<<" at index "<<i<<" with corresponding deciphering "<<cur_4gram_plain<<" has weight before inversion "<<qualities[i];
                logger->endLine();
            #endif
            qualities[cur] = 1.0/qualities[cur];
            #ifdef DEBUG
                (*logger)<<" and after inversion : "<<qualities[cur];
                logger->endLine();
            #endif
            correspondances[cur] = i;
            cur++;
        }

    for(int i=cur; i<N; i++)
        qualities[i] = 0;

    if(!correspondances.size())
    {
        (*logger)<<"[-] Ouch something went wrong : correspondance table is empty in previous crypto object";
        logger->endLine();
        return;
    }

    Quality_Selection<N, float> q;
    int chosen = q.apply(qualities)[0]; //chosen is index of cipher quadrigram which will be replaced
    #ifdef DEBUG
        (*logger)<<chosen<<" has been chosen to be deleted, it matches 4gram "<<correspondances[chosen]<<" with weight "<<qualities[chosen]<<" at index "<<correspondances[chosen];
        logger->endLine();
    #endif

    std::string cipher_4gram = "aaaa";
    for(int j=0; j<4; j++)
        cipher_4gram[j] = cipher[correspondances[chosen]+j];
     #ifdef DEBUG
        (*logger)<<" this is cipher 4gram "<<cipher_4gram;
        logger->endLine();
    #endif
    Key key2 = key.derive_delete(correspondances[chosen], cipher_4gram);
    res = Crypto_Object(key2);
    std::cout<<"GAVE IT !! "<<res.get_clear()<<std::endl;
}


template <typename Key, typename Weight, size_t N, size_t M>
const Weight& Crypto_Object<Key, Weight, N, M>::get_weight() const
{return cur_weight;}

template <typename Key, typename Weight, size_t N, size_t M>
const std::string& Crypto_Object<Key, Weight, N, M>::get_clear() const
{return cur_deciphered;}

template <typename Key, typename Weight, size_t N, size_t M>
void Crypto_Object<Key, Weight, N, M>::set_key(const Key& k)
{
    key = k;
    cur_deciphered = key.decipher(cipher, ngrams_set);
    cur_weight.weight(cipher, cur_deciphered);
}


template <typename Key, typename Weight, size_t N, size_t M>
void Crypto_Object<Key, Weight, N, M>::set_cipher(const std::string& txt)
{cipher = txt;}

template <typename Key, typename Weight, size_t N, size_t M>
void Crypto_Object<Key, Weight, N, M>::set_4gram_tree(const String_Tree& s)
{
    Ngram_tree = s;
    Weight::set_4gram_tree(s);
}

template <typename Key, typename Weight, size_t N, size_t M>
void Crypto_Object<Key, Weight, N, M>::set_logger(const std::shared_ptr<Info_Warning_Error_Logger>& logger)
{
    Crypto_Object<Key, Weight, N, M>::logger = logger;
}


template <typename Key, typename Weight, size_t N, size_t M>
void Crypto_Object<Key, Weight, N, M>::get_4grams_counter(std::map<std::string, std::pair<int, int> >& Ngrams_counter, bool in_cur_key)
{
    for(int i=0; i<cipher.size()-3; i++)
        if(!ngrams_set[i]&&!in_cur_key)
            if(Ngrams_counter.count(cipher.substr(i,4)))
            {
                Ngrams_counter[cipher.substr(i,4)].first += 1;
                if(rand()%2)
                    Ngrams_counter[cipher.substr(i,4)].second = i;
            }
            else
                Ngrams_counter[cipher.substr(i,4)] = std::pair<int,int>(1,i);
        else if(ngrams_set[i]&&in_cur_key)
            if(Ngrams_counter.count(cipher.substr(i,4)))
            {
                Ngrams_counter[cipher.substr(i,4)].first += 1;
                if(rand()%2)
                    Ngrams_counter[cipher.substr(i,4)].second = i;
            }
            else
                Ngrams_counter[cipher.substr(i,4)] = std::pair<int,int>(1,i);
}


#endif
