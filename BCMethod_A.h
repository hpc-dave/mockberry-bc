#ifndef BC_METHOD_A_H_
#define BC_METHOD_A_H_

#include <iostream>
#include <memory>
#include <unordered_map>
#include "MatrixBlock.h"
#include "BCObjectManager.h"
#include "BoundaryLocator.h"
#include "BoundaryCondition_ABD.h"

namespace mock {

// some kind of boundary condition method
// only contains methods and data relevant for
// applying the matrix manipulation, nothing else!
class BCMETHOD_A {
public:
    using ObjectIndex = BCObjectManager::ObjectIndex;
    using BLObserverType = BoundaryLocator::ObserverType;
    using OMObserverType = BCObjectManager::ObserverType;
    using BCType = BoundaryCondition_ABD;
    using BCContainer = std::unordered_map<ObjectIndex, BCType>;

    BCMETHOD_A(BCObjectManager* om, BoundaryLocator* bl) {
        om_observer = std::make_unique<OMObserverType>(
            [this](const BCObjectManager& bc_om, BCObjectManager::StateChange prop) {
                std::cout << "BCMETHOD_A observer was triggered by the object manager" << std::endl;
            });
        om->Attach(om_observer.get());
        bl_observer = std::make_unique<BLObserverType>(
            [this](const BoundaryLocator& bc_bl, BoundaryLocator::StateChange prop) {
                std::cout << "BCMETHOD_A observer was triggered by the boundary locator" << std::endl;
            });
        bl->Attach(bl_observer.get());
    }

    void apply(MatrixBlock* mb) {
        // Here we could access the relevant object depending on the ID given in the boundary locator
        std::cout << "BCMethodA -> applying boundary conditions" << std::endl;
    }

    bool AddBC(BCType bc, ObjectIndex n) {
        return bcs.insert({n, bc}).second;
    }

    const BCType& GetBC(ObjectIndex n) const {
        auto it = bcs.find(n);
        if(it == bcs.end()){
            std::cout << "Oo, missing bc" << std::endl;
        }
        return it->second;
    }

    BCType& GetBC(ObjectIndex n) {
        auto it = bcs.find(n);
        if (it == bcs.end()) {
            std::cout << "Oo, missing bc" << std::endl;
        }
        return it->second;
    }

    // commented out here, because not required
    // void Update() {
    //     // recompute internal variables in the case
    //     // objects were added or removed
    // }

    template <typename T>
    void ExtrapolateToGhostCells(T* field) {
        std::cout << "Extrapolating to ghost cells" << std::endl;
    };

private:
    std::unique_ptr<OMObserverType> om_observer;
    std::unique_ptr<BLObserverType> bl_observer;
    BCContainer bcs;
};
}

#endif  // BC_METHOD_A_H_