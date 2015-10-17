
#include <iostream>
#include <string>
#include "bimap.h"
#include <cstdlib>
#include <time.h>
#include <map>
#include <algorithm>
#include <map>
#include <assert.h>
#include <set>

using namespace std;

std::string gen_rand_str(int length)
{
    int r = rand() % 1000;
    std::string result;
    while(length > 0)
    {
        result += 'a' + r % 10;
        r /= 10;
        length--;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

std::string gen_by_number(int number)
{
    std::string result;
    number++;
    while(number > 0)
    {
        result += 'a' + number % 10;
        number /= 10;
    }
    return result;
}



int main() {
    f();
    //srand(time(nullptr));
    bool inserted;
    bool half_contain = false;
    int insert_count = 0;
    int erase_count = 0;
    my::bimap<int, int> bimap;
    map<int, int> left_to_right;
    map<int, int> right_to_left;
    for(int i = 0; i < 5000; i++)
    {
        if(!bimap.check_invariant())
        {
            return 1337;
        }
        if(!bimap.check_tree())
        {
            return 1337;
        }
        int random1 = rand() % 100;
        int random2 = rand() % 100;
        std::cout << "iteration: " << random1 << " " << random2 << std::endl;
        bool contains1;
        bool contains2;
        auto my_l = bimap.l_find(random1);
        auto my_r = bimap.r_find(random2);
        auto ltr = left_to_right.find(random1);
        auto rtl = right_to_left.find(random2);



        contains1 = bimap.l_find(random1) != bimap.l_end();
        contains2 = bimap.r_find(random2) != bimap.r_end();
        if(contains1)
        {
            std::cout << "key repeat" << random1 << std::endl;
            bimap.l_erase(random1);
            erase_count++;
            inserted = false;
            continue;
        }
        if(contains2)
        {
            if(*my_r.to_other_type() != rtl->second || *my_r != rtl->first)
            {
                return -1;
            }
            std::cout << "value repeat" << random2 << std::endl;
            bimap.r_erase(random2);
            erase_count++;
            inserted = false;
            continue;
        }
        bimap.insert(random1, random2);
        left_to_right[random1] = random2;
        right_to_left[random2] = random1;
        inserted = true;
        insert_count++;
    }

    //part2

    my::bimap<int, int> second_bimap;

    set<int> random_values;

    for(int i = 0; i < 100; i++)
    {
        random_values.insert(rand() % 30);
    }

    while(random_values.size() % 2 == 1)
    {
        random_values.insert(rand() % 30);
    }

    for(auto it = random_values.begin(); it != random_values.end(); it++)
    {
        int random1 = *it;
        it++;
        int random2 = *it;
        second_bimap.insert(random1, random2);
    }

    std::cout << "iterating left" << std::endl;
    for(auto it = second_bimap.l_begin(); it != second_bimap.l_end(); it++)
    {
        std::cout << *it << " " <<  *it.to_other_type() << std::endl;
    }
    std::cout << "iterating right" << std::endl;
    for(auto it = second_bimap.r_begin(); it != second_bimap.r_end(); it++)
    {
        std::cout << *it << " " << *it.to_other_type() << std::endl;
    }



    return 0;
}




