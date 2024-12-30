#ifndef BCOBJECT_H_
#define BCOBJECT_H_

#include <memory>
#include <type_traits>

namespace mock {

// a tagging class
class BCObjectTag {
};

// with c++20 concepts that would be easier
template<typename T, typename Enable = void>
struct is_bc_object : std::false_type {
};

template <typename T>
struct is_bc_object<T, std::enable_if_t<std::is_base_of_v<BCObjectTag, T>>> : std::true_type {
};

template <typename T>
constexpr bool is_bc_object_v = is_bc_object<T>::value;

namespace detail {

// external polymorphism, virtual base class
class BCObjectConcept {
public:
    virtual bool IsInside(double x, double y) const = 0;
    virtual void Print() const = 0;
};

template<typename ObjectT>
class OwningBCObjectConcept: public BCObjectConcept {
public:
    explicit OwningBCObjectConcept(ObjectT obj) : object(std::move(obj)) {
        static_assert(is_bc_object_v<ObjectT>, "the provided object is not a BCObject type");
    }

    bool IsInside(double x, double y) const override {
        return object.IsInside(x, y);
    }

    void Print() const override {
        object.Print();
    }

private:
    ObjectT object;
};

template <typename ObjectT>
class NonOwningBCObjectConcept : public BCObjectConcept {
public:
    explicit NonOwningBCObjectConcept(const ObjectT* obj) : object(obj) {
        static_assert(is_bc_object_v<ObjectT>, "the provided object is not a BCObject type");
    }

    bool IsInside(double x, double y) const override {
        return object->IsInside(x, y);
    }

    void Print() const override {
        object->Print();
    }

private:
    const ObjectT* object;
};

}

class BCObjectTypeErasure {
public:
    using PriorityType = uint8_t;   // a priority identifier 0->lowest
    template <typename Object>
    BCObjectTypeErasure(Object obj, PriorityType prio) : priority(prio) {
        using Model = detail::OwningBCObjectConcept<Object>;
        pimpl = std::make_unique<Model>(std::move(obj));
    }

    template <typename Object>
    BCObjectTypeErasure(Object* obj, PriorityType prio) : priority(prio) {
        using Model = detail::NonOwningBCObjectConcept<Object>;
        pimpl = std::make_unique<Model>(obj);
    }

    bool IsInside(double x, double y) const {
        return pimpl->IsInside(x, y);
    }

    void Print() const {
        pimpl->Print();
    }

    PriorityType GetPriority() const {
        return priority;
    }

private:
    PriorityType priority;
    std::unique_ptr<detail::BCObjectConcept> pimpl;
};

using BCObject = BCObjectTypeErasure;
}

#endif  // BCOBJECT_H_