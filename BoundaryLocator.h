#ifndef BOUNDARY_LOCATOR_H_
#define BOUNDARY_LOCATOR_H_

#include <iostream>
#include <vector>
#include <set>
#include <memory>

#include "Observer.h"
#include "BCObjectManager.h"

namespace mock {

// something similar to the Flag fields
class BoundaryLocator {
public:
    enum class StateChange {
        UpdatedFlagField
    };
    using FlagType = int;
    using IndexType = int;
    using ObjectIndex = BCObjectManager::ObjectIndex;
    using FieldType = std::vector<FlagType>;
    using ObserverType = Observer<BoundaryLocator, StateChange>;
    using OMObserverType = mock::BCObjectManager::ObserverType;

    explicit BoundaryLocator(BCObjectManager* obj_man) {
        om_observer = std::make_unique<OMObserverType>(
            [this](const mock::BCObjectManager& objman, mock::BCObjectManager::StateChange prop) {
                switch(prop) {
                    case BCObjectManager::StateChange::FinishedUpdating:
                        std::cout << "Boundary Locator here! I report that the object manager has finished updating!" << std::endl;
                        if ((objman.GetAddedObjects().size() != 0)  || (objman.GetRemovedObjects().size() != 0))
                            UpdateFlagField(objman);
                        break;
                    default:
                        {} // do nothing
                }
            });
        obj_man->Attach(om_observer.get());
    }

    bool HasObject(IndexType ind) {
        bool has_obj = ind % 2; // something dummy
        std::cout << "there is " << (has_obj ? "an" : "no") << "object in cell " << ind << std::endl;
        return has_obj;
    }

    ObjectIndex GetObjectIndex(int cell) const {
        // should do some query for objects
        return cell;
    }

    void UpdateFlagField(const mock::BCObjectManager& obj_man) {
        // Here we loop through all objects
        std::cout << "Updating Flag field" << std::endl;
        for (std::size_t n{0}; n < obj_man.size(); n++) {
            std::cout << "Object " << n << " has priority " << std::to_string(obj_man.Get(n).GetPriority()) << " and has following message:\n";
            obj_man.Get(n).Print();
        }
        Notify(StateChange::UpdatedFlagField);
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

private:
    FieldType flagfield;
    std::set<ObserverType*> observers;  // observers which are attached to this object

    std::unique_ptr<OMObserverType> om_observer;  // observer for the object manager
};
}

#endif  // BOUNDARY_LOCATOR_H_