#ifndef RAND_H
#define RAND_H

#include <random>

class Rand {
    public:
        static void init();

        static int intInRange(int min, int max);
        static double doubleInRange(double min, double max);

    private:
        static std::minstd_rand *gen; //mt19937_64

        Rand() = default;
		Rand& operator=(const Rand& rand);
};

#endif // RAND_H
