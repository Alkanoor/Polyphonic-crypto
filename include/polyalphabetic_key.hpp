#ifndef POLYALPHABETIC_KEY_HPP
#define POLYALPHABETIC_KEY_HPP


#include <vector>
#include <string>
#include <array>
#include <map>


class Polyalphabetic_Key
{
    public:
        Polyalphabetic_Key(int key_length=1);

        const std::array<std::vector<char>, 4>& get_forbidden_chars(int index, const std::string& cur_ngram) throw ();
        char get_correspondance(int index, char cipher) const;

        std::string decipher(const std::string& cipher, std::vector<bool>& ngrams_set);

        Polyalphabetic_Key derive_add(int index, const std::string& Ngram_ciphered, const std::string& Ngram_plain) throw ();
        Polyalphabetic_Key derive_delete(int index, const std::string& Ngram_ciphered) throw ();

        bool oracle(int index, const std::string& cur_ngram, const std::string& to_fit) throw ();

        void reset();

    private:
        int key_length_;
        std::vector<std::map<char, char> > correspondance;
        std::array<std::vector<char>, 4> forbidden;
};


#endif
