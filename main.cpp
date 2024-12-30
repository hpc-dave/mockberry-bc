#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <random>
#include <chrono>

#include "BCObject.h"
#include "BCObjectManager.h"
// #include "BoundaryLocator.h"
#include "Equation.h"
#include "BCMethod_A.h"
#include "BCMethod_B.h"


// create two different objects for showing off the polymorphic nature
class Foo : mock::BCObjectTag {
public:
    bool IsInside(double x, double y) const {
        return false;
    }
    void Print() const {
        std::cout << "I am Foo" << std::endl;
    }
};

class Goo : mock::BCObjectTag {
public:
    bool IsInside(double x, double y) const {
        return true;
    }
    void Print() const {
        std::cout << "I am Goo" << std::endl;
    }
};

int main() {
    using OI = mock::BCObjectManager::ObjectIndex;
    // define Objects with object manager
    mock::BCObjectManager objects;

    // externally managed object
    Foo foo;
    OI obj1 = objects.Add(&foo);
    // internally managed object
    OI obj2 = objects.Add(Goo{});

    // define the Equation
    mock::Equation<mock::BCMETHOD_A> equation_A;
    mock::Equation<mock::BCMETHOD_B> equation_B;

    // The boundary condition method requires a boundary locator (bascically the old flag field)
    mock::BoundaryLocator bc_locator(&objects);

    equation_A.InitializeBCMethod(&objects, &bc_locator);
    equation_B.InitializeBCMethod(&objects, &bc_locator);

    // add an observer for showing off that feature and notifyin us whenever an object is added or removed
    mock::BCObjectManager::ObserverType observer([](const mock::BCObjectManager& objman, mock::BCObjectManager::StateChange prop) {
        if (prop == mock::BCObjectManager::StateChange::AddObject) {
            std::cout << "An object was added!" << std::endl;
        } else if (prop == mock::BCObjectManager::StateChange::RemoveObject) {
            std::cout << "An object was removed!" << std::endl;
        }
    });
    objects.Attach(&observer);

    // adding another object afterwards
    Foo foo2;
    OI obj3 = objects.Add(&foo2);  // Here we trigger the observer

    objects.FinishUpdating();  // now we cannot add any other objects
    // This also triggers the updating of the BoundaryLocator

    // add copy of foo2
    objects.Add(foo2);  // will throw an error, because updating is finished
    objects.StartUpdating(); // now we can add objects again!
    objects.Add(foo2);  // Adds another object
    objects.FinishUpdating();

    // Now lets add some boundary conditions
    // BCMethod_A only has the ABD type boundary conditions
    mock::BoundaryCondition_ABD bc_a(0., 1., 0.);
    equation_A.GetBCMethod().AddBC(bc_a, obj1);
    equation_A.GetBCMethod().AddBC(bc_a, obj2);
    equation_A.GetBCMethod().AddBC(bc_a, obj3);

    mock::BoundaryCondition_Prescribed bc_b_1(1.);
    mock::BoundaryCondition_ABD bc_b_2(1., 0., 1.);
    mock::BoundaryCondition_NoApply bc_b_3;
    equation_B.GetBCMethod().AddBC(bc_b_1, obj1);
    equation_B.GetBCMethod().AddBC(bc_b_2, obj2);
    equation_B.GetBCMethod().AddBC(bc_b_3, obj3);

    equation_A.PreStep();
    equation_A.Build();
    equation_A.Solve();
    equation_A.UpdateBoundaryCells();
    equation_A.PostStep();

    equation_B.PreStep();
    equation_B.Build();
    equation_B.Solve();
    equation_B.UpdateBoundaryCells();
    equation_B.PostStep();

    return 0;
}