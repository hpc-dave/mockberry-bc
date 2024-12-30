#ifndef BOUNDARYCONDITION_ABD_H_
#define BOUNDARYCONDITION_ABD_H_

#include <array>

namespace mock {
class BoundaryCondition_ABD {
public:
    enum {
        a,
        b,
        d,
        count
    };
    using ScalarType = double;
    using StorageType = std::array<ScalarType, count>;
    using SC = ScalarType;

    BoundaryCondition_ABD(SC a, SC b, SC d) : params({a, b, d}) {
    }

    SC GetA() const { return params[a]; }
    SC GetB() const { return params[b]; }
    SC GetD() const { return params[d]; }
    void SetA(SC v) { params[a] = v; }
    void SetB(SC v) { params[b] = v; }
    void SetD(SC v) { params[d] = v; }
    const StorageType& GetParams() { return params; }

private:
    StorageType params;
};
}

#endif  // BOUNDARYCONDITION_ABD_H_
