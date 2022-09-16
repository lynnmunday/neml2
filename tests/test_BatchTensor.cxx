#include <catch2/catch.hpp>

#include "BatchTensor.h"

TEST_CASE("BatchTensors have the correct shapes", "[BatchTensors]") {
  
  // Should have 2 batch dimensions and 3 base dimensions
  BatchTensor<2> A(torch::zeros({10,5,4,11,2}));

  SECTION(" batch sizes are correct" ) {
    REQUIRE(A.nbatch() == 2);
    REQUIRE(A.batch_sizes() == TorchShape({10,5}));
  }

  SECTION(" base sizes are correct" ) {
    REQUIRE(A.nbase() == 3);
    REQUIRE(A.base_sizes() == TorchShape({4,11,2}));
  }
}

TEST_CASE("BatchTensors can't be created with few than the number of "
          "batch dimensions") {

  // Can't make this guy, as it won't have enough dimensions for the batch
  REQUIRE_THROWS(BatchTensor<2>(torch::zeros({10})));
}
