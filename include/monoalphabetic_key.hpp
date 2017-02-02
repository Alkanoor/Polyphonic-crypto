#ifndef MONOALPHABETIC_KEY_HPP
#define MONOALPHABETIC_KEY_HPP


#include <ostream>
#include <vector>
#include <string>
#include <array>
#include <map>


class Monoalphabetic_Key
{
    public:
        const std::array<std::vector<char>, 4>& get_forbidden_chars(int index, const std::string& cur_ngram) throw ();
        char get_correspondance(int index, char cipher) const;

        std::string decipher(const std::string& cipher, std::vector<bool>& ngrams_set);

        Monoalphabetic_Key derive_add(int index, const std::string& to_be_replaced_with, const std::string& to_use_for_replacing) throw ();
        Monoalphabetic_Key derive_delete(int index, const std::string& to_be_replaced_with) throw ();

        bool oracle(int index, const std::string& cur_ngram, const std::string& to_fit) throw ();

        void reset();

        std::ostream& log(std::ostream& os);

    private:
        std::array<std::vector<char>, 4> forbidden;
        std::map<char, char> correspondance;
};


#endif
