// Copyright 2019, Intel Corporation

#pragma once

#include <algorithm>
#include <map>

#include "tile/plaid_ir/mlir.h"

namespace vertexai {
namespace tile {
namespace plaid_ir {

// A macro to add operator overloads for a class
#define AFFINE_OP_OVERLOADS(X)                 \
  inline X operator*(const X& in, int64_t x) { \
    X r = in;                                  \
    r *= x;                                    \
    return r;                                  \
  }                                            \
  inline X operator*(int64_t x, const X& in) { \
    X r = in;                                  \
    r *= x;                                    \
    return r;                                  \
  }                                            \
  inline X operator+(const X& a, const X& b) { \
    X r = a;                                   \
    r += b;                                    \
    return r;                                  \
  }                                            \
  inline X operator-(const X& a, const X& b) { \
    X r = a;                                   \
    r += (-1 * b);                             \
    return r;                                  \
  }

// An affine 'polynomial', basically a series of terms, each consisting of a
// multiplier and an index (a Affine that is also a BlockArgument), as well as a
// constant offset.  We can 'flatten' any affine expression into such a
// polynomial.
struct AffinePolynomial {
  std::map<mlir::BlockArgument*, int64_t> terms;
  int64_t constant;
  // Make an empty polynomial
  AffinePolynomial();
  // Make a constant polynomial
  explicit AffinePolynomial(int64_t x);
  // Make a polynomial from an affine expression
  explicit AffinePolynomial(Value* x);
  // Perform operations on a polynomial
  AffinePolynomial& operator*=(int64_t x);
  AffinePolynomial& operator+=(const AffinePolynomial& x);
};

AFFINE_OP_OVERLOADS(AffinePolynomial)

// An affine range, which is a is basically the 'extent' of an affine
// polynomial, and also supports the operations of 'unioning'
struct AffineRange {
  int64_t min;      // Inclusive minimum value
  int64_t max;      // Inclusive maximum value
  uint64_t stride;  // Step size of the affine

  AffineRange() : min(0), max(0), stride(0) {}
  AffineRange(int64_t _min, int64_t _max, uint64_t _stride = 1);
  AffineRange& operator*=(int64_t x);
  AffineRange& operator+=(const AffineRange& x);
  AffineRange& operator|=(const AffineRange& x);
  explicit AffineRange(const AffinePolynomial& x);
  explicit AffineRange(Value* x) : AffineRange(AffinePolynomial(x)) {}
};

// Operator overload for affine ops as well as union
AFFINE_OP_OVERLOADS(AffineRange)
inline AffineRange operator|(const AffineRange& a, const AffineRange& b) {
  AffineRange r = a;
  r |= b;
  return r;
}

/*

// Compute the range as per above per dimension given a refinement and optional base
std::vector<AffineRange> ComputeTensorRanges(Value* tensor, Value* base = nullptr);

// Find all uses of tensor and compute the union of all accesses with the the
// passed tensor as the base of the access.
std::vector<AffineRange> ComputeInteriorShape(Value* tensor);

*/

}  // namespace plaid_ir
}  // namespace tile
}  // namespace vertexai
