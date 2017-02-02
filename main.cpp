#include "selection/include/quality_selection.hpp"
#include "selection/include/elit_selection.hpp"
#include "include/words_and_4grams_weight.hpp"
#include "logger/include/stdout_handler.hpp"
#include "include/monoalphabetic_key.hpp"
#include "include/polyalphabetic_key.hpp"
#include "include/crypto_object.hpp"

typedef Crypto_Object<Monoalphabetic_Key, Words_And_4grams_Weight> MyCryptobject;
//typedef Crypto_Object<Polyalphabetic_Key, Words_And_4grams_Weight> MyCryptobject;

#define TOTAL_SIZE 100
#define TO_KEEP 30


float max_evaluation = -1;

void eval(const std::array<MyCryptobject, TOTAL_SIZE>& crypto_objects, std::array<float, TOTAL_SIZE>& evaluations)
{
    float max_evaluation = 0;
    for(int i=0; i<TOTAL_SIZE; i++)
    {
        float tmp = crypto_objects[i].get_weight().to_float();
        evaluations[i] = tmp;
        if(tmp>max_evaluation)
        {
            std::cout<<"New max found : "<<tmp<<std::endl;
            std::cout<<"The weight was : ";
            crypto_objects[i].get_weight().show(std::cout);
            std::cout<<"The text obtained is : "<<std::endl;
            std::cout<<crypto_objects[i].get_clear()<<std::endl<<std::endl;
            max_evaluation = tmp;
        }
    }
}

const std::array<int, TOTAL_SIZE>& select(std::array<float, TOTAL_SIZE>& evaluations, Selection<TOTAL_SIZE,float>& selection)
{
    return selection.apply(evaluations);
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


// g++ main.cpp src/*.cpp -o main -std=c++11 -I. logger/*.cpp -g

int main()
{
    srand(time(NULL));

    String_Tree tree;
    std::vector<std::string> to_add_string;
    std::vector<float> to_add_float;
    read_strings_with_weight("data/english_quadgrams_reduced2.txt", to_add_string, to_add_float);
    tree.add_strings_with_weight(to_add_string, to_add_float);
    //tree.show(std::cout);

    Easy_Log_In_File::getInfoLog()->addHandler(std::shared_ptr<Handler>(new Stdout_Handler()));
    MyCryptobject::set_cipher("KZQLSAQXWZSHKEBAQWYPPQTZBXABEADSZKHASDZPDDZPIZPSKAWWZOHALQAJDMAJKAJDAPSAJDZQXJDPQDSZPAPOJDAQXPMBZDPQQZNAPEBAQWKPSBRZSJGZQAGPSYPJTZPKMAKWJQAJDYPJTZPKADDJSAJDYPJTZPKRZSSJMJAJDDSZPKAQKMZQXSATJQEBAQWKJBBZQMPBLPSAQDXPQWZPSSZPUTJSLJQABWZPBZJSYPJWZQXPJKAJDABAOZSDHPJDKTAWAQDHSZMZQXBAWPQABTZPKAKHJSAQDIPKYPABRABBPWJQADJZQIPKYPAPDZPSQJK");
    MyCryptobject::set_4gram_tree(tree);

    std::array<MyCryptobject, TOTAL_SIZE> crypto_objects;
    std::array<MyCryptobject, TO_KEEP> tmp;
    std::array<float, TOTAL_SIZE> evaluations;

    //crypto_objects[0].set_key(Polyalphabetic_Key(10));

    std::shared_ptr<Quality_Selection<TOTAL_SIZE, float> > quality_selection(new Quality_Selection<TOTAL_SIZE, float>());
    Elit_Selection<TOTAL_SIZE, float> chosen_selection(3, quality_selection);

    int n_iterations = 1000;
    int cur_size = 1;
    int ratio = (TOTAL_SIZE-TO_KEEP)/10;
    for(int i=0; i<n_iterations; i++)
    {
        std::cout<<"Iteration "<<i<<std::endl;
        int total_sizes = cur_size;
        int cur_ratio = ratio;
        auto crypto_objects_cpy = crypto_objects;
        while(total_sizes<TOTAL_SIZE)
            for(int j=0; j<TO_KEEP && j<cur_size; j++)
            {
                if(!cur_ratio)
                    cur_ratio = 1;
                for(int k=0; k<cur_ratio && total_sizes<TOTAL_SIZE; k++)
                {
                    if(cur_ratio>3 && k*4<cur_ratio)
                    {
                        #ifdef DEBUG
                            std::cout<<total_sizes<<" is getting a backing object of key "<<j<<std::endl;
                        #endif
                        crypto_objects_cpy[j].get_a_previous_object(crypto_objects[total_sizes]);
                    }
                    else
                    {
                        #ifdef DEBUG
                            std::cout<<total_sizes<<" is getting a following object of key "<<j<<std::endl;
                        #endif
                        crypto_objects_cpy[j].get_a_following_object(crypto_objects[total_sizes]);
                    }
                    total_sizes++;
                }
                cur_ratio /= 1.5;
            }

        eval(crypto_objects, evaluations);
        #ifdef DEBUG
            std::cout<<"Evaluation process gave "<<std::endl;
            Utils::write_vec(evaluations, std::cout);
            std::cout<<std::endl;
        #endif
        const std::array<int, TOTAL_SIZE>& selected = select(evaluations, chosen_selection);

        #ifdef DEBUG
            std::cout<<"New order is"<<std::endl;
        #endif
        for(int j=0; j<TO_KEEP; j++)
        {
            #ifdef DEBUG
                std::cout<<selected[j]<<" ("<<evaluations[selected[j]]<<")"<<std::endl;
            #endif
            tmp[j] = crypto_objects[selected[j]];
        }
        for(int j=0; j<TO_KEEP; j++)
            crypto_objects[j] = tmp[j];

        cur_size = TO_KEEP;
    }
}
