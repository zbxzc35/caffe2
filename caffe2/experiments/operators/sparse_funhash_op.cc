#include "caffe2/experiments/operators/sparse_funhash_op.h"

namespace caffe2 {
namespace {

REGISTER_CPU_OPERATOR(SparseFunHash, SparseFunHashOp<float, CPUContext>);
REGISTER_CPU_OPERATOR(
    SparseFunHashGradient,
    SparseFunHashGradientOp<float, CPUContext>);

OPERATOR_SCHEMA(SparseFunHash)
    .NumInputs(4, 5)
    .NumOutputs(1)
    .SetDoc(R"DOC(
This layer compresses a fully-connected layer for sparse inputs
via hashing.
It takes four required inputs and an option fifth input.
The first three inputs `scalars`, `indices`, and `segment_ids` are
the sparse segmented representation of sparse data, which are the
same as the last three inputs of the `SparseSortedSegmentWeightedSum`
operator. If the argument `num_segments` is specified, it would be used
as the first dimension for the output; otherwise it would be derived
from the maximum segment ID.

The fourth input is a 1D weight vector. Each entry of the fully-connected
layer would be randomly mapped from one of the entries in this vector.

When the optional fifth input vector is present, each weight of the
fully-connected layer would be the linear combination of K entries
randomly mapped from the weight vector, provided the input
(length-K vector) serves as the coefficients.
)DOC")
    .Input(0, "scalars", "Values of the non-zero entries of the sparse data.")
    .Input(1, "indices", "Indices to the non-zero valued features.")
    .Input(
        2,
        "segment_ids",
        "Segment IDs corresponding to the non-zero entries.")
    .Input(3, "weight", "Weight vector")
    .Input(
        4,
        "alpha",
        "Optional coefficients for linear combination of hashed weights.")
    .Output(
        0,
        "output",
        "Output tensor with the first dimension equal to the number "
        "of segments.")
    .Arg("num_outputs", "Number of outputs")
    .Arg("num_segments", "Number of segments");

OPERATOR_SCHEMA(SparseFunHashGradient).NumInputs(5, 6).NumOutputs(2, 3);

class GetSparseFunHashGradient : public GradientMakerBase {
  using GradientMakerBase::GradientMakerBase;
  vector<OperatorDef> GetGradientDefs() override {
    if (def_.input_size() == 4) {
      return SingleGradientDef(
          "SparseFunHashGradient",
          "",
          vector<string>{GO(0), I(0), I(1), I(2), I(3)},
          vector<string>{GI_V(3), GI_I(3)});
    }
    // def_.input_size() == 5
    return SingleGradientDef(
        "SparseFunHashGradient",
        "",
        vector<string>{GO(0), I(0), I(1), I(2), I(3), I(4)},
        vector<string>{GI_V(3), GI_I(3), GI(4)});
  }
};

REGISTER_GRADIENT(SparseFunHash, GetSparseFunHashGradient);

} // namespace
} // namespace caffe2