#ifndef MATRIXBLOCK_H_
#define MATRIXBLOCK_H_

#include <vector>

namespace mock {

// a very simplistic version of the matrix block, no fancy organization or so
class MatrixBlock {
public:
    using ScalarType = double;
    using CellIndexType = int;

    MatrixBlock(CellIndexType ind) : cell_index(ind) {}

    const std::vector<ScalarType>& Get() const {
        return params;
    }

    std::vector<ScalarType>& Get() {
        return params;
    }

    CellIndexType GetCellIndex()const{
        return cell_index;
    }

private:
    std::vector<ScalarType> params;
    CellIndexType cell_index;
};
}

#endif  // MATRIXBLOCK_H_
