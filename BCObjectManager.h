#ifndef BCOBJECTMANAGER_H_
#define BCOBJECTMANAGER_H_
#include <vector>
#include <unordered_map>
#include <set>
#include "Observer.h"
#include "BCObject.h"


namespace mock {

// manages objects, abstracts them and provides them on request
class BCObjectManager {
public:
    enum StateChange {
        AddObject,
        RemoveObject,
        FinishedUpdating
    };

    using ObjectIndex = std::size_t;
    using ContainerType = std::unordered_map<ObjectIndex, BCObject>;
    using iterator = typename ContainerType::iterator;
    using const_iterator = typename ContainerType::const_iterator;
    using difference_type = typename ContainerType::difference_type;

    using ObserverType = Observer<BCObjectManager, StateChange>;
    using PriorityType = typename BCObject::PriorityType;

    BCObjectManager() : max_object_id(0), is_updating(true) {}
    ~BCObjectManager(){}

    BCObjectManager(const BCObjectManager&) = delete;
    BCObjectManager& operator=(const BCObjectManager&) = delete;
    BCObjectManager(BCObjectManager&&) = delete;

    iterator begin() { return objects.begin(); }
    iterator end() { return objects.end(); }
    const_iterator cbegin() const { return objects.cbegin(); }
    const_iterator cend() const { return objects.cend(); }

    std::size_t size() const {
        return objects.size();
    }

    BCObject& Get(ObjectIndex n) {
        if(!IsOpenForUpdate()){
            std::cerr << "You won't get access to an object if the manager is not updating!" << std::endl;
        }
        // do some more safety management here
        return objects.find(n)->second;
    }

    const BCObject& Get(ObjectIndex n) const {
        return objects.find(n)->second;
    }

    template <typename ObjectT>
    ObjectIndex Add(ObjectT obj, PriorityType prio = 1) {
        if (!IsOpenForUpdate()) {
            std::cerr << "You cannot add an object if the manager is not updating!" << std::endl;
        }
        // not thread safe!
        ObjectIndex obj_id = max_object_id++;
        auto [it, success] = objects.insert({obj_id, BCObject(std::move(obj), prio)});
        if(!success) {
            std::cout << "Object insertion failed!" << std::endl;
        }
        added_objects.push_back(obj_id);
        Notify(AddObject);
        return obj_id;
    }

    template <typename ObjectT>
    ObjectIndex Add(const ObjectT* obj, PriorityType prio = 1) {
        if (!IsOpenForUpdate()) {
            std::cerr << "You cannot add an object if the manager is not updating!" << std::endl;
        }
        // not thread safe!
        ObjectIndex obj_id = max_object_id++;
        auto [it, success] = objects.insert({obj_id, BCObject(obj, prio)});
        if (!success) {
            std::cout << "Object insertion failed!" << std::endl;
        }
        added_objects.push_back(obj_id);
        Notify(AddObject);
        return obj_id;
    }

    auto Remove(ObjectIndex n) {
        if (!IsOpenForUpdate()) {
            std::cerr << "You cannot remove an object if the manager is not updating!" << std::endl;
        }
        auto it = objects.find(n);
        // some safety would be nice here
        auto r = objects.erase(it);
        removed_objects.push_back(n);
        Notify(RemoveObject);
        return r;
    }

    bool Attach(ObserverType* observer) {
        auto [pos, success] = observers.insert(observer);
        return success;
    }

    bool Detach(ObserverType* observer) {
        return (observers.erase(observer) > 0U);
    }

    void Notify(StateChange property) {
        // This formulation makes sure detach() operations
        // can be detected during the iteration
        for (auto iter = observers.begin(); iter != observers.end();) {
            auto const pos = iter++;
            (*pos)->update(*this, property);
        }
    }

    void StartUpdating(){
        added_objects.clear();
        removed_objects.clear();
        is_updating = true;
    }

    void FinishUpdating() {
        is_updating = false;
        Notify(StateChange::FinishedUpdating);
    }

    bool IsOpenForUpdate() const {
        return is_updating;
    }

    const std::vector<ObjectIndex>& GetAddedObjects() const{
        return added_objects;
    }

    const std::vector<ObjectIndex>& GetRemovedObjects() const {
        return removed_objects;
    }

private:
    ContainerType objects;
    std::vector<ObjectIndex> added_objects;
    std::vector<ObjectIndex> removed_objects;
    std::set<ObserverType*> observers;
    ObjectIndex max_object_id;
    bool is_updating;
};
}

#endif  // BCOBJECTMANAGER_H_
