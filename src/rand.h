#ifndef RAND_H
#define RAND_H

#include <random>

class Rand {
    public:
        static void init();

        static int intInRange(int min, int max);
        static double doubleInRange(double min, double max);
        static float floatInRange(float min, float max);

        Rand(const Rand&) = delete;
        Rand& operator=(const Rand&) = delete;

    private:
        static std::minstd_rand *gen;

        Rand() = default;
};

#endif // RAND_H
