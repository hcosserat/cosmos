#include <random>
#include "random.h"

static std::mt19937 rng(std::time(nullptr));

// Function to get random integer in range [min, max]
int random_int(const int min, const int max) {
    std::uniform_int_distribution dist(min, max);
    return dist(rng);
}

// Function to get random double in range [min, max]
double random_double(const double min, const double max) {
    std::uniform_real_distribution dist(min, max);
    return dist(rng);
}
