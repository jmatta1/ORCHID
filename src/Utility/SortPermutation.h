/***************************************************************************//**
********************************************************************************
**
** @file SortPermutation.h
** @author James Till Matta
** @date 09 May, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details File that holds the simple title string to display
**
********************************************************************************
*******************************************************************************/
// includes for C system headers
// includes for C++ system headers
#include<vector>
#include<algorithm>
// includes from other libraries
// includes from ORCHID
namespace Utility
{
//calculates the permutation required to order the key vector
template<typename T, typename Compare>
std::vector<std::size_t> oneKeySortPermutation(const std::vector<T>& key,
                                               Compare& compare)
{
    std::vector<std::size_t> permutation(key.size());
    std::iota(permutation.begin(), permutation.end(), 0);
    std::sort(permutation.begin(), permutation.end(),
        [&](std::size_t i, std::size_t j){ return compare(key[i], key[j]); });
    return permutation;
}

//calculates the permutation required to order the secondary and primary key vectors
template<typename T, typename Compare>
std::vector<std::size_t> twoKeySortPermutation(const std::vector<T>& primaryKey,
                                               const std::vector<T>& secondaryKey,
                                               Compare& compare)
{
    std::vector<std::size_t> permutation(primaryKey.size());
    std::iota(permutation.begin(), permutation.end(), 0);
    std::sort(permutation.begin(), permutation.end(),
        [&](std::size_t i, std::size_t j){ return compare(primaryKey[i],
                                                          primaryKey[j],
                                                          secondaryKey[i],
                                                          secondaryKey[j]); });
    return permutation;
}

//takes the permutation vector and applies that permutation to a given vector
template<typename T>
std::vector<T> applyPermutation(std::vector<T>& input,
                                const std::vector<std::size_t>& permutation)
{
    std::vector<T> sorted(permutation.size());
    std::transform(permutation.begin(), permutation.end(), sorted.begin(),
        [&](std::size_t i){return input[i]; });
    return sorted;
}

struct OneKeyIntCompare
{
    bool operator()(int key1, int key2) { return key1<key2;}
};

struct TwoKeyIntCompare
{
    bool operator()(int primaryKey1, int primaryKey2, int secondaryKey1, int secondaryKey2)
        { return ( (primaryKey1 != primaryKey2) ? (primaryKey1 < primaryKey2) : (secondaryKey1 < secondaryKey2)); }
};

}
