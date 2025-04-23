#include "scheduler.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

struct AddNumber {
    float add(float a) const {
        return a + number;
    }

    float number;
};

TEST(CorrectResult, float_result) {
    float a = 1;
    float b = -2;
    float c = 0;
    AddNumber add{
        .number = 3
    };
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](float a, float c) {return -4 * a * c;}, a, c);

    auto id2 = scheduler.add([](float b, float v) {return b * b + v;}, b, scheduler.getFutureResult<float>(id1));

    auto id3 = scheduler.add([](float b, float d) {return -b + std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

    auto id4 = scheduler.add([](float b, float d) {return -b - std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

    auto id5 = scheduler.add([](float a, float v) {return v / (2 * a);}, a, scheduler.getFutureResult<float>(id3));

    auto id6 = scheduler.add([](float a, float v) {return v / (2 * a);}, a, scheduler.getFutureResult<float>(id4));

    auto id7 = scheduler.add(&AddNumber::add, add, scheduler.getFutureResult<float>(id6));

    scheduler.executeAll();

    std::vector<float> v = {scheduler.getResult<float>(id5), scheduler.getResult<float>(id6), scheduler.getResult<float>(id7)};
    ASSERT_THAT(v, ::testing::ElementsAre(2, 0, 3));
}

float result;

void func(float a, float b) {
    a += 2;
    result = a * a / b / b;
}

TEST(CorrectResult, void_result) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](float a, float b) {return a + b;}, 5, 5);
    auto id2 = scheduler.add([](float a, float b) {return a * b;}, 2, 5);
    auto id3 = scheduler.add([](float a, float b) {return a - b;}, scheduler.getFutureResult<float>(id1), scheduler.getFutureResult<float>(id2));
    auto id4 = scheduler.add([](float a, float b) {return a / b;}, scheduler.getFutureResult<float>(id1), scheduler.getFutureResult<float>(id2));
    auto id5 = scheduler.add(func, scheduler.getFutureResult<float>(id3), scheduler.getFutureResult<float>(id4));

    scheduler.getResult<void>(id5);

    ASSERT_FLOAT_EQ(result, 4);
}
