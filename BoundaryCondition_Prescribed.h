#ifndef BOUNDARYCONDITION_PRESCRIBED_H_
#define BOUNDARYCONDITION_PRESCRIBED_H_

namespace mock {

class BoundaryCondition_Prescribed {
public:
    using ScalarType = double;

    BoundaryCondition_Prescribed(ScalarType v): value(v){}

    ScalarType GetValue() const { return value; }

private:
    ScalarType value;
};
}
#endif  // BOUNDARYCONDITION_PRESCRIBED_H_
