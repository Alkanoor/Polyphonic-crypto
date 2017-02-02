#ifndef WORDS_AND_4GRAMS_WEIGHT_HPP
#define WORDS_AND_4GRAMS_WEIGHT_HPP


#include <ostream>
#include <string>
#include <map>

#include "string_tree.hpp"


class Words_And_4grams_Weight
{
    public:
        float to_float() const;
        std::ostream& show(std::ostream& os) const;

        void weight(const std::string& cipher, const std::string& plain);

        static void set_4gram_tree(const String_Tree& s);

    private:
        float current_value;
        std::string cur_plain;
        std::map<char, int> current_letters_counter;
        std::map<std::string, int> current_bigrams_counter;
        std::map<char, float> current_letters_frequencies;
        std::map<std::string, float> current_bigrams_frequencies;

        static String_Tree Ngram_tree;
};


#endif
