#include "scheduler.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(TrowExeption, void_result) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](float a, float b) {return a + b;}, 5, 5);
    auto id2 = scheduler.add([](float a, float b) {return a * b;}, 2, 5);
    auto id3 = scheduler.add([](float a, float b) {return a - b;}, scheduler.getFutureResult<float>(id1), scheduler.getFutureResult<float>(id2));
    auto id4 = scheduler.add([](float a, float b) {return a / b;}, scheduler.getFutureResult<float>(id1), scheduler.getFutureResult<float>(id2));
    auto id5 = scheduler.add([](float a, float b) {return a == b;}, scheduler.getFutureResult<float>(id3), scheduler.getFutureResult<float>(id4));


    ASSERT_THROW(scheduler.getResult<float>(id5), std::bad_cast);
}