#ifndef BC_METHOD_B_H_
#define BC_METHOD_B_H_

#include <iostream>
#include <memory>
#include <unordered_map>
#include <variant>

#include "MatrixBlock.h"
#include "BCObjectManager.h"
#include "BoundaryLocator.h"
#include "BoundaryCondition_ABD.h"
#include "BoundaryCondition_Prescribed.h"
#include "BoundaryCondition_NoApply.h"

namespace mock {

// some kind of boundary condition method
// only contains methods and data relevant for
// applying the matrix manipulation, nothing else!
class BCMETHOD_B {
public:
    using ObjectIndex = BCObjectManager::ObjectIndex;
    using BLObserverType = BoundaryLocator::ObserverType;
    using OMObserverType = BCObjectManager::ObserverType;
    using BCType = std::variant<BoundaryCondition_ABD, BoundaryCondition_Prescribed, BoundaryCondition_NoApply>;
    using BCContainer = std::unordered_map<ObjectIndex, BCType>;

    BCMETHOD_B(BCObjectManager* om, BoundaryLocator* bl) : boundary_loc(bl) {
        om_observer = std::make_unique<OMObserverType>(
            [this](const BCObjectManager& bc_om, BCObjectManager::StateChange prop) {
                std::cout << "BCMETHOD_B observer was triggered by object manager" << std::endl;
            });
        om->Attach(om_observer.get());
        bl_observer = std::make_unique<BLObserverType>(
            [this](const BoundaryLocator& bc_bl, BoundaryLocator::StateChange prop) {
                std::cout << "BCMETHOD_B observer was triggered by boundary locator" << std::endl;
            });
        bl->Attach(bl_observer.get());
    }

    void apply(MatrixBlock* mb) {
        std::cout << "applying boundary conditions" << std::endl;
        for (int n = 0; n < 3; n++){
            ObjectIndex obj_ind = boundary_loc->GetObjectIndex(n);
            auto bc = bcs.find(obj_ind);
            std::visit([obj_ind](auto&& arg) {
                            using T = std::decay_t<decltype(arg)>;
                            if constexpr (std::is_same_v<T, BoundaryCondition_ABD>) {
                                std::cout << "Applying ABD Boundary condition for object" << obj_ind << std::endl;
                            } else if constexpr (std::is_same_v<T, BoundaryCondition_Prescribed>) {
                                std::cout << "Applying Prescribed Boundary condition for object " << obj_ind << std::endl;
                            } else if constexpr (std::is_same_v<T, BoundaryCondition_NoApply>) {
                                std::cout << "Applying no boundary condition for object " << obj_ind << std::endl;
                            }
                        },
                        bc->second);
        }
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

    void Update() {
        // recompute internal variables in the case
        // objects were added or removed
    }

    template <typename T>
    void ExtrapolateToGhostCells(T* field){
        std::cout << "Extrapolating to ghost cells" << std::endl;
    };

private:
    std::unique_ptr<OMObserverType> om_observer;
    std::unique_ptr<BLObserverType> bl_observer;
    BCContainer bcs;
    const BoundaryLocator* boundary_loc;
};
}

#endif  // BC_METHOD_B_H_