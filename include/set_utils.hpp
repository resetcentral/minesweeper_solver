#include <set>
#include <algorithm>

namespace set_utils {
    template <typename T>
    std::set<T> set_intersection(std::set<T> set1, std::set<T> set2) {
        std::set<T> result;
        std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(result, result.begin()));

        return result;
    }

    template <typename T>
    std::set<T> set_union(std::set<T> set1, std::set<T> set2) {
        std::set<T> result;
        std::set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(result, result.begin()));

        return result;
    }

    template <typename T>
    std::set<T> set_difference(std::set<T> set1, std::set<T> set2) {
        std::set<T> result;
        std::set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(result, result.begin()));

        return result;
    }
}