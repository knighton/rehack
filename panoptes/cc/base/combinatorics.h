#ifndef CC_BASE_COMBINATORICS_H_
#define CC_BASE_COMBINATORICS_H_

#include <cstdint>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace Combinatorics {

struct InclusiveSpan {
    size_t a;
    size_t z_incl;
};

template <typename T>
void InclusiveSpansOfSame(const vector<T>& v, vector<InclusiveSpan>* spans);

template <typename T>
bool NextChooseOneFromEach(
    const vector<T>& num_options_per_field, vector<T>* values);

template <typename T>
void CollapseToWildcards(
    const vector<T>& num_options_per_field, vector<vector<T> >* tuples);

}  // namespace Combinatorics

#include "combinatorics_impl.h"

#endif  // CC_BASE_COMBINATORICS
