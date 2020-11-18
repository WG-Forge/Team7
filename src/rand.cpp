#include "rand.h"
#include <chrono>

std::minstd_rand* Rand::gen = nullptr;

void Rand::init() {
	if (!gen) {
        uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::seed_seq ss {uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32)};
        gen = new std::minstd_rand();
		gen->seed(ss);
	}
}

int Rand::intInRange(int min, int max) {
    std::uniform_int_distribution<int> uid(min, max);
	return uid(*gen);
}

double Rand::doubleInRange(double min, double max) {
    std::uniform_real_distribution<double> urd(min, max);
	return urd(*gen);
}

float Rand::floatInRange(float min, float max) {
    std::uniform_real_distribution<float> urd(min, max);
    return urd(*gen);
}
