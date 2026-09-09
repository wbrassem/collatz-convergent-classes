/-
Copyright (c) 2026 Wayne Brassem. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.
Authors: Wayne Brassem
-/

import Collatz.Divisionwords
import Collatz.Affine
import Collatz.Realizability

/-!
# Collatz

This is the root module for the formal development of finite division-word
structure under the fully accelerated Collatz map.

The development is organized into three layers.

* `Collatz.Divisionwords` defines the fully accelerated map, division counts,
  finite division words, admissibility, realization, and the basic structural
  properties of generated words.

* `Collatz.Affine` develops the symbolic affine representation associated
  with a division word, including the affine constant, concatenation laws,
  explicit formulas, prefix structure, and dyadic growth comparisons.

* `Collatz.Realizability` connects the formal affine representation back to
  actual Collatz dynamics. It proves the exact realization congruence modulo
  `2^(D(ω)+1)`, establishes the unique canonical realization residue of an
  admissible word, and develops the resulting prefix-lifting and
  nested-or-disjoint residue structure.

Together these modules provide the formal foundation for the subsequent
development of realization classes, cylinders, and density arguments.

This root module intentionally introduces no additional mathematical
definitions or theorems; importing `Collatz` exposes the current verified
proof spine.
-/
