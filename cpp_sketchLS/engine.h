#ifndef GUARD_ENGINE_H
#define GUARD_ENGINE_H

#include <random>

// 乱数の準備
std::random_device rd;
std::mt19937 engine(rd());

#endif // GUARD_ENGINE_H