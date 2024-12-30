#ifndef EQUATION_H_
#define EQUATION_H_

#include <memory>
#include <type_traits>
#include "BCMethod_A.h"
#include "MatrixBlock.h"

namespace mock {

namespace detail {
template <typename T, typename = void>
struct has_Update_function : std::false_type {};

template <typename T>
using test_for_Update_function = decltype(std::declval<T>().Update());

template <typename T>
struct has_Update_function<T, std::void_t<test_for_Update_function<T>> > : std::true_type {};

template<typename T>
constexpr bool has_Update_function_v = has_Update_function<T>::value;
}

// this class assumes a matrix block type of assembly structure and
// therefore also may contain a boundary applicator
template<typename BCMETHOD = BCMETHOD_A>
class Equation {
public:
    using ScalarType = double;
    using FieldType = std::vector<ScalarType>;
    using BoundaryConditionType = typename BCMETHOD::BCType;

    template<typename... Args>
    void InitializeBCMethod(Args&&... args){
        bc_method = std::make_unique<BCMETHOD>(args...);
    }

    BCMETHOD& GetBCMethod() { return *bc_method; }
    const BCMETHOD& GetBCMethod() const { return *bc_method; }

    void PreStep(){
        if constexpr(detail::has_Update_function_v<BCMETHOD>)
            bc_method->Update();
    }

    void Build(){
        // for showing, the n should be cells, but it also is the object right now
        for (int n = 0; n < 3; n++) {
            MatrixBlock mb(n);
            bc_method->apply(&mb);
        }
    }

    void Solve(){

    }

    void UpdateBoundaryCells() {
        double dummy{0.};
        bc_method->ExtrapolateToGhostCells(&dummy);
    }

    void PostStep(){

    }


private:
    std::unique_ptr<BCMETHOD> bc_method;  // making it dynamic also means less overhead when loading the class
};
}

#endif  // EQUATION_H_