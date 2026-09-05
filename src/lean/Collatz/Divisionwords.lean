/-
Copyright (c) 2026 Wayne Brassem. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.
Authors: Wayne Brassem
-/

import Mathlib

/-!

# Division Words for Collatz Trajectories

This module develops the formal definitions used to encode
accelerated Collatz trajectories by their 2-adic division counts.

The accelerated map is

`f(x) = (3 * x + 1) / 2^(v₂(3 * x + 1))`.
-/

/-
Establish the fact that the fully accelerated map is well-defined on the odd natural numbers. This
is done by showing that for any odd natural number x, the expression (3 * x + 1) is always divisible
by 2 raised to the power of its 2-adic valuation, which ensures that the division in the definition
of the fully accelerated map yields an odd natural number result.  And if x is odd then this in turn
ensures the the 2-adic valuation is greater tham zero.

-/

/- Create a namespace for all lemmas and definitions. -/
namespace Collatz

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

/-- The 2-adic division count for a natural number undergoing a Collatz iteration. -/
def division_count (x : ℕ) : ℕ :=
  padicValNat 2 (3 * x + 1)

/- Validity of division counts -/
#eval Collatz.division_count 1    -- should be 2
#eval Collatz.division_count 2    -- should be 0
#eval Collatz.division_count 3    -- should be 1
#eval Collatz.division_count 4    -- should be 0
#eval Collatz.division_count 5    -- should be 4

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

/- Show that the quotient associated with the decomposition is odd. -/
example {n k m : ℕ} (hn : n = 2 ^ k * m) (hm : Odd m) :
    Odd (n / 2 ^ k) := by
  rw [hn]
  norm_num
  exact hm

/-- For a natural number n = 2^k m, with m odd means 2-adic(n) = k. -/
lemma valuation_exponent_of_odd_factor
  {n k m : ℕ}
  (hn : n = 2 ^ k * m)
  (hm : Odd m)
  (hn0 : n ≠ 0) :
  padicValNat 2 n = k := by
    /- Rewrite n with 2 ^ k * m in hdiv, so 2 ^ k | 2 ^ k * m. -/
    have hdiv : 2 ^ k ∣ n := by
      rw [hn]
      exact dvd_mul_right (2 ^ k) m
    /- Since 2 ^ k divides n, padicValNat_dvd_iff_le gives k ≤ padicValNat 2 n. -/
    have h_lower : k ≤ padicValNat 2 n := by
      /- Maps the hdiv equivalence relation from 2 ^ k | n to k ≤ padicValNat 2 n. -/
      exact (padicValNat_dvd_iff_le (p := 2) (a := n) (n := k) hn0).mp hdiv
    /- Proof by contradiction tactic. -/
    have h_upper : padicValNat 2 n ≤ k := by
      by_contra h
      /- Assume the opposite of h_upper. For natural numbers, ¬(v ≤ k) implies k + 1 ≤ v. -/
      have hval : k + 1 ≤ padicValNat 2 n := by
        omega
      /- Since k + 1 ≤ padicValNat 2 n, the valuation theorem gives 2^(k + 1) ∣ n. -/
      have hpow : (2 : ℕ) ^ (k + 1) ∣ n := by
        exact (padicValNat_dvd_iff_le
          (p := 2) (a := n) (n := k + 1) hn0).mpr hval
      /- Rewrite n using hn so that the divisibility statement exposes
      the assumed factorization n = 2^k * m. -/
      have hpow' : (2 : ℕ) ^ (k + 1) ∣ 2 ^ k * m := by
        rw [hn] at hpow
        exact hpow
      /- Rewrite 2^(k + 1) as 2^k * 2 using the power recursion theorem. -/
      rw [pow_succ] at hpow'
      /- The cancellation theorem sees 2^k as its common factor.
      It therefore requires a proof that 0 < 2^k; `positivity` supplies it. -/
      have htwo : (2 : ℕ) ∣ m := by
        exact Nat.dvd_of_mul_dvd_mul_left (by positivity) hpow'
      /- Since hm : Odd m means that m = 2 * k + 1 for some k,
      unpack hm to obtain that witness and equation. -/
      have hodd : ¬ (2 : ℕ) ∣ m := by
        /- Assume 2 divides m and unpack the divisibility proof:
          m = 2 * j for some j. -/
        rintro ⟨j, hj⟩
        /- Unpack hm : Odd m:
          m = 2 * k + 1 for some k. -/
        rcases hm with ⟨k, hk⟩
        /- Now m is simultaneously 2 * j and 2 * k + 1, which is impossible. -/
        omega
      /- Contracition 2 | m and ¬2 | m -/
      exact hodd htwo
    /- If each is less than or equal to the other, then they are equal. -/
    exact Nat.le_antisymm h_upper h_lower

/- Quick exercise of the above lemma using API constraints -/
example : padicValNat 2 16 = 4 := by
  have h := Collatz.valuation_exponent_of_odd_factor
    (n := 16) (k := 4) (m := 1)
    (by norm_num)
    (by norm_num)
    (by norm_num)
  exact h

/-- The factor remaining after removing the full 2-adic part is odd.

    Since every nonzero natural number can be written as 2^k * m
    with m odd, valuation_exponent_of_odd_factor identifies k with
    the 2-adic valuation. Dividing by that power therefore leaves m.
-/
lemma odd_part_of_padicValNat
    {n : ℕ} (hn0 : n ≠ 0) :
    Odd (n / 2 ^ padicValNat 2 n) := by
  /- Extract k, m, Odd m, and n = 2^k * m from
     Nat.exists_eq_two_pow_mul_odd. -/
  obtain ⟨k, m, hm, hn⟩ :=
    Nat.exists_eq_two_pow_mul_odd hn0
  /- Use the previous lemma to establish that k is exactly
     the 2-adic valuation of n. -/
  have hval : padicValNat 2 n = k :=
    valuation_exponent_of_odd_factor hn hm hn0
  /- First replace the 2-adic valuation by k, then replace n
     by its decomposition 2^k * m. -/
  rw [hval, hn]
  /- Simplify (2^k * m) / 2^k to m. -/
  norm_num
  /- The resulting goal is exactly the oddness fact extracted
     above from Nat.exists_eq_two_pow_mul_odd. -/
  exact hm

/-- The fully accelerated Collatz map is defined on the natural numbers.
    It is a well-defined function which maps natural numbers to natural numbers.
    The map is defined as follows and leverages the division count definition.
-/
def fully_accelerated (x : ℕ) : ℕ :=
  (3 * x + 1) / 2 ^ (division_count x)

/- Validity of fully accelerated map -/
#eval Collatz.fully_accelerated 1   -- should be 1
#eval Collatz.fully_accelerated 2   -- should be 7
#eval Collatz.fully_accelerated 3   -- should be 5
#eval Collatz.fully_accelerated 4   -- should be 13
#eval Collatz.fully_accelerated 5   -- should be 1

/- Example mapping 5 to 1 using the concrete verification lemma. -/
example : fully_accelerated 5 = 1 := by
  rw [fully_accelerated, division_count_five]
  norm_num

/-- The fully accelerated Collatz map always produces an odd natural number. -/
lemma fully_accelerated_odd (x : ℕ) :
    Odd (fully_accelerated x) := by
  /- Expand fully_accelerated. -/
  unfold fully_accelerated
  /- Expand division_count within this. -/
  unfold division_count
  /- Infer that n := 3 * x + 1. -/
  exact odd_part_of_padicValNat (by omega)

/-- An odd natural number has a positive division count. -/
lemma division_count_pos
    (x : ℕ) (hx : Odd x) :
    1 ≤ division_count x := by
  /- Expand division_count. -/
  unfold division_count
  /- Establish 3 * x + 1 is even when x is odd. -/
  have heven : Even (3 * x + 1) := by
    /- An odd number times an odd number is odd. -/
    have h3x : Odd (3 * x) := by
      norm_num [Nat.odd_mul, hx]
    /- The sum of two odd numbers is even. -/
    exact h3x.add_odd odd_one
  /- Evenness implies divisibility by 2. -/
  have hdiv : (2 : ℕ) ∣ 3 * x + 1 := by
    exact Even.two_dvd heven
  /- Use the valuation characterization with exponent 1:
    2^1 ∣ (3 * x + 1) implies 1 ≤ padicValNat 2 (3 *x + 1). -/
  have hval : 1 ≤ padicValNat 2 (3 * x + 1) := by
      exact (padicValNat_dvd_iff_le
        (p := 2) (a := 3 * x + 1) (n := 1) (by norm_num)).mp hdiv
  /- hval is exactly the unfolded goal. -/
  exact hval

/-- A division word is a finite list of natural numbers intended to encode
    division counts along a fully accelerated Collatz trajectory. -/
abbrev division_word := List ℕ

/-- The first `m` division counts generated by `x`. -/
def division_counts (x m : ℕ) : division_word :=
  (List.iterate fully_accelerated x m).map division_count

#eval division_counts 3 2   -- should be [1, 4]
#eval [1, 2, 1, 2, 2].length  -- should be 5
#eval decide (([1, 4, 2, 2] : division_word) = division_counts 3 4)  -- Should be true

/-- Every division count generated from an odd starting value is positive. -/
lemma division_counts_pos
    (x m : ℕ)
    (hx : Odd x) :
    ∀ d ∈ division_counts x m, 1 ≤ d := by
  /- Induct on the length `m` of the generated division-count list.
    The goal is to show every entry `d` in that list satisfies `1 ≤ d`. -/
  induction m generalizing x with
  /- The m=i=0 case represents the empty division word. -/
  | zero =>
     /- Simplify to nothing since there is nothing to iterate over. -/
      simp [division_counts]
  /- Assume the result for length `m` and prove it for length `m + 1`.
     The induction hypothesis `ih` is generalized over the starting value `x`. -/
  | succ m ih =>
    /- Introduce an arbitrary division count `d` and the hypothesis
      that `d` occurs in the length-(m+1) generated word. -/
    intro d hd
    /- Unfold the successor trace enough to expose membership as either
       membership in the head or in the remaining tail. -/
    simp only [division_counts, List.iterate] at hd
    /- Membership in the successor trace has two cases:
      `d` is the head division count, or `d` occurs in the tail. -/
    rcases hd with hhead | htail
    /- Head case: `d` is the current division count, which is positive
      because the current state `x` is odd. -/
    · exact division_count_pos x hx
    /- For (m + 1) rename the internal at iterator
        variable to htail for later use. -/
    · rename_i htail
      /- Apply the induction hypothesis to the tail trajectory, whose
         new starting value is `fully_accelerated x`.  Its oddness is
         supplied by `fully_accelerated_odd x`. -/
      apply ih
          (fully_accelerated x)
          (fully_accelerated_odd x)
          d
      /- The induction hypothesis leaves the goal
         d ∈ division_counts (fully_accelerated x) m.
         Rewrite that goal into the unfolded form in which `htail`
        is already stated. -/
      change d ∈ List.map division_count
        (List.iterate fully_accelerated (fully_accelerated x) m)
      /- `htail` is now the same membership proposition as the goal,
         written using `List.Mem` rather than the `∈` notation. -/
      exact htail

/-- A valid division word is a non-empty division word. -/
def valid_division_word (ω : division_word) : Prop :=
  0 < ω.length

-- #eval decide valid_division_word [2, 2]

#eval List.iterate fully_accelerated 3 2
#eval List.map division_count (List.iterate fully_accelerated 3 2)

/-- An admissible division word is one where each 2-adic factor is ≥ 1. -/
def admissible_division_word (ω : division_word) : Prop :=
  valid_division_word ω ∧ ∀ d ∈ ω, 1 ≤ d

/-- Every nonempty finite division-count trace generated from an odd
    starting value is an admissible division word. -/
lemma division_counts_admissible
    (x m : ℕ)
    (hx : Odd x)
    (hm : 0 < m) :
    admissible_division_word (division_counts x m) := by
  /- Expand admissibility into its two requirements:
     nonempty length and positivity of every entry. -/
  unfold admissible_division_word
  /- Prove the two conjuncts separately. -/
  constructor
  /- The generated list has length `m`, so `hm : 0 < m`
     proves that it is nonempty. -/
  · simpa [valid_division_word, division_counts] using hm
  /- Every generated division count is positive because the
     starting value is odd. -/
  · exact division_counts_pos x m hx

/-- A natural number `x` realizes the division word `ω` when `ω` is exactly
    the finite sequence of division counts generated by the first `ω.length`
    iterates starting from `x`. -/
def realizes (x : ℕ) (ω : division_word) : Prop :=
  ω = division_counts x ω.length

/-- The total division count of a division word is the sum of its entries. -/
def total_division_count (ω : division_word) : ℕ :=
  ω.sum

#eval total_division_count [1, 2, 3]    -- should be 6

/-- The generated division-count word has the requested length. -/
lemma division_counts_length (x m : ℕ) :
    (division_counts x m).length = m := by
  simp [division_counts]

/-- If every entry of a division word is positive, then its length
    is at most its total sum. -/
lemma division_word_length_le_sum_of_pos
    (ω : division_word)
    (hpos : ∀ d ∈ ω, 1 ≤ d) :
    ω.length ≤ ω.sum := by
  /- Move `hpos` back into the goal so that the induction
     hypothesis may assume positivity for the tail word. -/
  revert hpos
  /- Induct directly on the list representing the division word. -/
  induction ω with
  /- Empty word: both its length and its sum are zero. -/
  | nil =>
      intro hpos
      simp
  /- Assume the result for the tail `ω` and prove it for
     the word formed by prepending `d`. -/
  | cons d ω ih =>
      intro hpos
      /- The head entry is positive because it belongs to `d :: ω`. -/
      have hd : 1 ≤ d := by
        exact hpos d (by simp)
      /- Every entry of the tail is also an entry of `d :: ω`,
         so the positivity hypothesis restricts to the tail. -/
      have htail : ∀ e ∈ ω, 1 ≤ e := by
        intro e he
        exact hpos e (by simp [he])
      /- Apply the induction hypothesis to the tail. -/
      have ih' : ω.length ≤ ω.sum := by
        exact ih htail
      /- Adding the positive head preserves the inequality. -/
      calc
        (d :: ω).length = ω.length + 1 := by simp
        _ ≤ ω.sum + d := Nat.add_le_add ih' hd
        _ = d + ω.sum := by
          exact Nat.add_comm _ _
        _ = (d :: ω).sum := by simp

/-- The length of an admissible division word is at most
    its total division count. -/
lemma division_word_length_le_total
    {ω : division_word}
    (hω : admissible_division_word ω) :
    ω.length ≤ total_division_count ω := by
  /- Admissibility supplies nonemptiness and positivity.
     Only positivity is needed for this inequality. -/
  rcases hω with ⟨_, hpos⟩
  unfold total_division_count
  exact division_word_length_le_sum_of_pos ω hpos

/- End of the Collatz namespace. -/
end Collatz
