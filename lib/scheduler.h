#pragma once
#include <memory>
#include <unordered_map>
#include "lib/help_lib/invoke.h"
#include "lib/help_lib/any.h"

class TTaskScheduler;


template<typename T>
class FutureResult {
private:
    int parent_id_;
public:
    using ResultType = T;

    FutureResult(int parent_id) : parent_id_(parent_id) {}

    int getID() const {
        return parent_id_;
    }
};

struct WTask {
    virtual ~WTask() {}
    virtual Any& execute(TTaskScheduler* scheduler) = 0;
};

template<typename Func, typename... Args>
struct Task;

template<typename Func>
struct Task<Func> : public WTask {
    Func func;
    Any result;
    bool executed = false;

    Task(Func&& f) : func(forward<Func>(f)) {}

    Any& execute(TTaskScheduler* scheduler) override {
        if constexpr (is_same_v<Any, void>) {
            invoke(func);
        } else {
            result = invoke(func);
        }
        return result;
    }
};

template<typename Func, typename Arg1>
struct Task<Func, Arg1> : public WTask {
    Func func;
    Arg1 arg1;
    Any result;
    bool executed = false;

    Task(Func&& f, Arg1&& a1)
        : func(forward<Func>(f)), arg1(forward<Arg1>(a1)) {}

    Any& execute(TTaskScheduler* scheduler) override {
        if (executed) {
            return result;
        }
        auto resolved_arg1 = resolve_arg(arg1, scheduler);
        if constexpr (is_same_v<Any, void>) {
            invoke(func, resolved_arg1);
        } else {
            result = invoke(func, resolved_arg1);
        }
        executed = true;
        return result;
    }
};

template<typename Func, typename Arg1, typename Arg2>
struct Task<Func, Arg1, Arg2> : public WTask {
    Func func;
    Arg1 arg1;
    Arg2 arg2;
    Any result;
    bool executed = false;

    Task(Func&& f, Arg1&& a1, Arg2&& a2)
        : func(forward<Func>(f)), arg1(forward<Arg1>(a1)), arg2(forward<Arg2>(a2)) {}

    Any& execute(TTaskScheduler* scheduler) override {
        if (executed) {
            return result;
        }
        auto resolved_arg1 = resolve_arg(arg1, scheduler);
        auto resolved_arg2 = resolve_arg(arg2, scheduler);
        if constexpr (is_same_v<Any, void>) {
            invoke(func, resolved_arg1, resolved_arg2);
        } else {
            result = invoke(func, resolved_arg1, resolved_arg2);
        }
        executed = true;
        return result;
    }
};

class TTaskScheduler {
    public:
        template<typename Func>
        int add(Func&& f) {
            tasks[curId] = std::make_unique<Task<Func>>(
                forward<Func>(f)
            );
            return curId++;
        }
        
        template<typename Func, typename Arg1>
        int add(Func&& f, Arg1&& arg1) {
            tasks[curId] = std::make_unique<Task<Func, Arg1>>(
                forward<Func>(f),
                forward<Arg1>(arg1)
            );
            return curId++;
        }
        
        template<typename Func, typename Arg1, typename Arg2>
        int add(Func&& f, Arg1&& arg1, Arg2&& arg2) {
            tasks[curId] = std::make_unique<Task<Func, Arg1, Arg2>>(
                forward<Func>(f),
                forward<Arg1>(arg1),
                forward<Arg2>(arg2)
            );
            return curId++;
        }
    
        template<typename T>
        FutureResult<T> getFutureResult(int id) {
            return FutureResult<T>(id);
        }
    
        template<typename T>
        T getResult(int id) {
            auto it = tasks.find(id);
            if (it == tasks.end()) throw "Invalid task ID";
            return any_cast<T>(it->second->execute(this));
        }
    
        void executeAll() {
            for (auto& [id, task] : tasks) {
                task->execute(this);
            }
        }
    
    private:
        int curId = 0;
        std::unordered_map<int, std::unique_ptr<WTask>> tasks;
};


template<typename T>
constexpr bool is_future_result = false;

template<typename T>
constexpr bool is_future_result<FutureResult<T>> = true;

template<typename T>
auto resolve_arg(T&& arg, TTaskScheduler* scheduler) {
    if constexpr (is_future_result<std::decay_t<T>>) {
        using ResultType = typename std::decay_t<T>::ResultType;
        return scheduler->getResult<ResultType>(arg.getID());
    } else {
        return forward<T>(arg);
    }
}
    