#include <memory>
#include <type_traits>

namespace mock {

namespace detail {

// external polymorphism, virtual base class
class ObjectConcept {
public:
    virtual bool IsInside(double x, double y) const = 0;
    virtual void Print() const = 0;
};

template<typename ObjectT>
class OwningObjectConcept: public ObjectConcept {
public:
    explicit OwningObjectConcept(ObjectT obj) : object(std::move(obj)){
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
class NonOwningObjectConcept : public ObjectConcept {
public:
    explicit NonOwningObjectConcept(const ObjectT* obj) : object(obj) {
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

class ObjectTypeErasure {
public:
    template <typename Object>
    ObjectTypeErasure(Object obj) {
        using Model = detail::OwningObjectConcept<Object>;
        pimpl = std::make_unique<Model>(std::move(obj));
    }

    template<typename Object>
    ObjectTypeErasure(Object* obj) {
        using Model = detail::NonOwningObjectConcept<Object>;
        pimpl = std::make_unique<Model>(obj);
    }

    bool IsInside(double x, double y) const {
        return pimpl->IsInside(x, y);
    }

    void Print() const {
        pimpl->Print();
    }

private:
    std::unique_ptr<detail::ObjectConcept> pimpl;
};

using Object = ObjectTypeErasure;
}
