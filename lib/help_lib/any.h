#pragma once
#include <typeinfo>
#include "move.h"

class Any {
private:
    struct BaseHolder {
        virtual ~BaseHolder() = default;
        virtual const std::type_info& type() const = 0;
        virtual BaseHolder* clone() const = 0;
    };

    template<typename T>
    struct Holder : BaseHolder {
        T value;

        Holder(const T& val) : value(val) {}
        Holder(T&& val) : value(move(val)) {}

        const std::type_info& type() const override {
            return typeid(T);
        }

        BaseHolder* clone() const override {
            return new Holder<T>(value);
        }
    };

    BaseHolder* content = nullptr;

public:
    Any() = default;

    template<typename T>
    Any(const T& value) : content(new Holder<T>(value)) {}

    template<typename T>
    Any(T&& value) : content(new Holder<typename remove_const<typename remove_reference<T>::type>::type>(forward<T>(value))) {}

    Any(const Any& other) : content(other.content ? other.content->clone() : nullptr) {}

    Any(Any&& other) noexcept : content(other.content) {
        other.content = nullptr;
    }

    ~Any() {
        delete content;
    }

    Any& operator=(const Any& other) {
        if (this != &other) {
            delete content;
            content = other.content ? other.content->clone() : nullptr;
        }
        return *this;
    }

    Any& operator=(Any&& other) noexcept {
        if (this != &other) {
            delete content;
            content = other.content;
            other.content = nullptr;
        }
        return *this;
    }

    bool has_value() const noexcept {
        return content != nullptr;
    }

    const std::type_info& type() const {
        if (!content) throw "bad_cast";
        return content->type();
    }

    template<typename T>
    friend T any_cast(const Any& any);

    void reset() noexcept {
        delete content;
        content = nullptr;
    }

    void swap(Any& other) noexcept {
        BaseHolder* save = content;
        content = other.content;
        other.content = save;
    }
};

template<typename T>
T any_cast(const Any& any) {
    if (any.type() != typeid(T)) {
        throw "bad_cast";
    }
    return static_cast<Any::Holder<T>*>(any.content)->value;
}