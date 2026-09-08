/-
Copyright (c) 2026 Wayne Brassem. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.
Authors: Wayne Brassem
-/

import Mathlib
import Collatz.Divisionwords
import Collatz.Affine
import Collatz.Realizability

/-!
# Collatz Lean Development Notes

This file contains exploratory `#check` and `#eval` commands used while
developing the formalization. It is not part of the proof dependency spine.

The main formal development is contained in:

* `Collatz.Divisionwords`
* `Collatz.Affine`
* `Collatz.Realizability`
-/

/- Every language needs its equivalent of a hello world. -/
def hello := "world"

/- Open the Collatz namespace -/
namespace Collatz


/- Division Words -/

/- Show that the standard 3x+1 map is a well-defined function from Odd to Even.
   Starting with odd, 3 * odd ==> odd, so 3 * odd + 1 ==> even. -/
example (x : ℕ) (hx : Odd x) : Even (3 * x + 1) := by
  /- Three times odd remains odd -/
  have h3x : Odd (3 * x) := by
    /- Odd times odd is odd -/
    norm_num [Nat.odd_mul, hx]
  /- Odd plus one is even -/
  exact h3x.add_odd odd_one

/-
  Show that the 2-adic valuation of 16 is exactly 4.

  The argument has two parts:
  1. Since 2^4 divides 16, the Mathlib valuation theorem gives
     4 <= padicValNat 2 16.
  2. Since 2^5 does not divide 16, the valuation cannot be >= 5.
     We prove this upper bound by contradiction.

  The two inequalities then force padicValNat 2 16 = 4.
-/

#eval padicValNat 2 16   -- should be 4

/- Example of computing 2-adic valuation -/
example : padicValNat 2 16 = 4 := by
  have h4 : (2 : ℕ)^4 ∣ 16 := by
    norm_num
  have h5 : ¬(2 : ℕ)^5 ∣ 16 := by
    norm_num
  have h_lower : 4 ≤ padicValNat 2 16 := by
    exact padicValNat_dvd_iff_le (p := 2) (a := 16) (n := 4) (by norm_num) |>.mp h4
  have h_upper : padicValNat 2 16 < 5 := by
    by_contra h
    have h5' : 5 ≤ padicValNat 2 16 := by
      omega
    have : (2 : ℕ)^5 ∣ 16 := by
      exact padicValNat_dvd_iff_le (p := 2) (a := 16) (n := 5) (by norm_num) |>.mpr h5'
    exact h5 this
  omega

/- Validity of division counts -/
#eval division_count 1    -- should be 2
#eval division_count 2    -- should be 0
#eval division_count 3    -- should be 1
#eval division_count 4    -- should be 0
#eval division_count 5    -- should be 4

/- Show that the quotient associated with the decomposition is odd. -/
example {n k m : ℕ} (hn : n = 2 ^ k * m) (hm : Odd m) :
    Odd (n / 2 ^ k) := by
  rw [hn]
  norm_num
  exact hm

/- Show that the quotient associated with the decomposition is odd. -/
example {n k m : ℕ} (hn : n = 2 ^ k * m) (hm : Odd m) :
    Odd (n / 2 ^ k) := by
  rw [hn]
  norm_num
  exact hm

/- Quick exercise of the above lemma using API constraints -/
example : padicValNat 2 16 = 4 := by
  have h := Collatz.valuation_exponent_of_odd_factor
    (n := 16) (k := 4) (m := 1)
    (by norm_num)
    (by norm_num)
    (by norm_num)
  exact h

/- Validity of fully accelerated map -/
#eval fully_accelerated 1   -- should be 1
#eval fully_accelerated 2   -- should be 7
#eval fully_accelerated 3   -- should be 5
#eval fully_accelerated 4   -- should be 13
#eval fully_accelerated 5   -- should be 1

/- Establish the division count for x = 5 is 4 in a concrete verification lemma. -/
lemma division_count_five : division_count 5 = 4 := by
  unfold division_count
  have h4 : (2 : ℕ)^4 ∣ 16 := by
    norm_num
  have h5 : ¬(2 : ℕ)^5 ∣ 16 := by
    norm_num
  have h_lower : 4 ≤ padicValNat 2 16 := by
    exact padicValNat_dvd_iff_le
      (p := 2) (a := 16) (n := 4) (by norm_num) |>.mp h4
  have h_upper : padicValNat 2 16 < 5 := by
    by_contra h
    have h5' : 5 ≤ padicValNat 2 16 := by
      omega
    have : (2 : ℕ)^5 ∣ 16 := by
      exact padicValNat_dvd_iff_le
        (p := 2) (a := 16) (n := 5) (by norm_num) |>.mpr h5'
    exact h5 this
  have h_upper' : padicValNat 2 16 ≤ 4 := by
    omega
  exact Nat.le_antisymm h_upper' h_lower

/- Example mapping 5 to 1 using the concrete verification lemma. -/
example : fully_accelerated 5 = 1 := by
  rw [fully_accelerated, division_count_five]
  norm_num

#eval division_counts 3 2     -- should be [1, 4]
#eval [1, 2, 1, 2, 2].length  -- should be 5
#eval decide (([1, 4, 2, 2] : division_word) = division_counts 3 4)  -- Should be true

#eval List.iterate fully_accelerated 3 2 -- should be [3, 5]
#eval List.iterate fully_accelerated 3 3 -- should be [3, 5, 1]
#eval List.map division_count (List.iterate fully_accelerated 3 2) -- Should be [1, 4]

#eval total_division_count [1, 2, 3]    -- should be 6


/- Affine Representation -/

/- #eval affine_constant [1, 2] should be
    3^1 + 2^1 * (3^0 + 2^2 * 0) =
    3 + 2 * (1 + 4 * 0) =
    3 + 2 * 1 = 3 + 2 = 5
-/
#eval affine_constant [1, 2] -- should be 5

/-!
## Formal integrality versus exact realization

The word `[1, 1, 1, 4]` has total division count `7` and affine constant
`65`, so its formal affine map is

    F(x) = (81*x + 65) / 128.

The starting value `15` satisfies the formal integrality condition modulo
`128`, but does not realize the word.  The starting value `143` satisfies
the exact congruence modulo `256` and does realize the word.
-/

/- #eval affine_constant [1, 1, 1, 4] should be
    3^3 + 2^1 * (3^2 + 2^1 * (3^1 + 2^1 * (3^0 + 2^4 * 0))) =
    27 + 2 * (9 + 2 * (3 + 2 * (1 + 16 * 0))) =
    27 + 2 * (9 + 2 * 5) = 27 + 2 * (9 + 10) =
    27 + 2 * 19 = 27 + 38 = 65
-/
#eval affine_constant [1, 1, 1, 4] -- should be 65

example :
    total_division_count [1, 1, 1, 4] = 7 := by
  norm_num [total_division_count]

example :
    affine_constant [1, 1, 1, 4] = 65 := by
  norm_num [affine_constant]

#eval prefix_division_count [1, 2, 3] 0 -- should be 0
#eval prefix_division_count [1, 2, 3] 3 -- should be 6

#eval explicit_affine_constant [1, 1, 1, 4] -- should be 65

example :
    affine_constant [1, 1, 1, 4] =
      explicit_affine_constant [1, 1, 1, 4] := by
  exact affine_constant_eq_explicit [1, 1, 1, 4]

#eval division_counts 15 4    -- should be [1, 1, 1, 5]

#eval division_counts 143 4   -- should be [1, 1, 1, 4]

/- `x = 15` satisfies the formal affine integrality condition modulo `2^D`. -/
example :
    Nat.ModEq
      (2 ^ total_division_count [1, 1, 1, 4])
      (3 ^ [1, 1, 1, 4].length * 15 +
        affine_constant [1, 1, 1, 4])
      0 := by
  norm_num [
    total_division_count,
    affine_constant,
    Nat.ModEq
  ]


/- Its formal affine endpoint is the integer `10`. -/
example :
    affine_map [1, 1, 1, 4] (15 : ℚ) = 10 := by
  norm_num [
    affine_map,
    total_division_count,
    affine_constant
  ]

/- Nevertheless, `15` does not realize `[1,1,1,4]`.

   If it did, the exact-realization theorem would force the stronger
   congruence modulo `2^(D+1) = 256`, which `15` does not satisfy. -/
example :
    ¬ realizes 15 [1, 1, 1, 4] := by
  intro hrealizes
  have hω :
      admissible_division_word [1, 1, 1, 4] := by
    norm_num [
      admissible_division_word,
      valid_division_word
    ]
  have hexact :=
    (realizes_iff_exact_congruence hω).mp hrealizes
  norm_num [
    total_division_count,
    affine_constant,
    Nat.ModEq
  ] at hexact


/- `x = 143` satisfies the exact congruence modulo `2^(D+1) = 256`. -/
example :
    Nat.ModEq
      (2 ^ (total_division_count [1, 1, 1, 4] + 1))
      (3 ^ [1, 1, 1, 4].length * 143 +
        affine_constant [1, 1, 1, 4])
      (2 ^ total_division_count [1, 1, 1, 4]) := by
  norm_num [
    total_division_count,
    affine_constant,
    Nat.ModEq
  ]


/- Therefore `143` genuinely realizes the word. -/
example :
    realizes 143 [1, 1, 1, 4] := by
  have hω :
      admissible_division_word [1, 1, 1, 4] := by
    norm_num [
      admissible_division_word,
      valid_division_word
    ]
  apply (realizes_iff_exact_congruence hω).mpr
  norm_num [
    total_division_count,
    affine_constant,
    Nat.ModEq
  ]


/- Its genuine accelerated endpoint is `91`. -/
example :
    affine_map [1, 1, 1, 4] (143 : ℚ) = 91 := by
  norm_num [
    affine_map,
    total_division_count,
    affine_constant
  ]


/- Realizability -/

example :
    Nat.ModEq
      (2 ^ total_division_count [1,1,1,4])
      (3 ^ 4 * 15 + affine_constant [1,1,1,4])
      0 := by
  norm_num [total_division_count, affine_constant]

example :
    Nat.ModEq
      (2 ^ (total_division_count [1,1,1,4] + 1))
      (3 ^ 4 * 143 + affine_constant [1,1,1,4])
      (2 ^ total_division_count [1,1,1,4]) := by
  norm_num [total_division_count, affine_constant]


/- End of the Collatz namespace. -/
end Collatz

/- Check definitions and theorems related to division words -/
#check Collatz.division_count
#check Collatz.valuation_exponent_of_odd_factor
#check Collatz.odd_part_of_padicValNat
#check Collatz.fully_accelerated
#check Collatz.division_count_pos

/- Natural numbers -/
#check Nat.odd_mul
#check Nat.dvd_of_mul_dvd_mul_left
#check Nat.dvd_of_mul_dvd_mul_right
#check Nat.pow_padicValNat_mul_divMaxPow
#check Nat.pow_dvd_iff_le_padicValNat
#check Nat.le_antisymm
#check Nat.exists_eq_two_pow_mul_odd
#check Nat.exists_eq_succ_of_ne_zero

/- Number parity and algebra -/
#check Odd.add_odd
#check Even.two_dvd
#check even_iff_two_dvd
#check dvd_mul_right

/- p-adic valuations -/
#check padicValNat
#check padicValNat_dvd_iff_le
#check pow_padicValNat_dvd
#check pow_succ
#check pow_succ_padicValNat_not_dvd

/- Finite sets -/
#check Finset.sum_range_succ
#check Finset.sum_range_succ_comm
