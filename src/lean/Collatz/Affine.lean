/-
Copyright (c) 2026 Wayne Brassem. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.
Authors: Wayne Brassem
-/

import Collatz.Divisionwords

/-!

# Affine Representation of Division Words

This module develops the affine representation associated with finite
division words for the fully accelerated Collatz map.

The development has three main layers:

formal symbolic steps
        │
        ▼
   `word_action`
        │
        ▼
    `affine_map`
        │
        ├──────────────► explicit affine constant
        │                   │
        │                   ▼
        │          `affine_constant_eq_explicit`
        │
        ▼
realized trajectories
        │
        ▼
`word_action_realized`
        │
        ▼
 `affine_map_realized`

For a division word `ω`, symbolic composition of the accelerated Collatz
steps produces an affine expression of the form

`F_ω(x) = (3^|ω| * x + c(ω)) / 2^(D(ω))`

where `D(ω)` is the total division count and `c(ω)` is the natural-number
affine constant determined by the entries of `ω`. The formal affine map
itself is defined over `ℚ`, since arbitrary symbolic division words need
not prescribe exact natural-number divisions.

The affine representation is defined for every formal division word; the
word need not be realized by an actual Collatz trajectory. Realizability is
therefore a separate dynamical condition imposed on the symbolic affine
data. When a natural number realizes `ω`, the formal affine map agrees with
the corresponding finite segment of the fully accelerated Collatz
trajectory.

This module develops the structural properties of the affine
representation, including:

* the behavior of affine data under concatenation and passage to prefixes;
* the recursive and explicit forms of the affine constant `c(ω)`;
* the equivalence of those two affine-constant representations;
* the dyadic-ternary threshold governing whether the affine slope is below
  or above `1`;
* the bridge from realized symbolic words to actual accelerated Collatz
  iterates.

The affine representation provides the algebraic foundation for the
integrality, exact-realization, and congruence structure developed in
subsequent modules.

-/

/- BigOperators required for summation in the explicit formula for the affine constant -/
open scoped BigOperators

/- Open the Collatz namespace. -/
namespace Collatz

/-- One formal Collatz step using the prescribed division count `d`. -/
def symbolic_step (d : ℕ) (x : ℚ) : ℚ :=
  (3 * x + 1) / 2 ^ d

/-- Apply the formal Collatz steps encoded by a division word. -/
def word_action : division_word → ℚ → ℚ
  | [], x => x
  /- Recursively call the function on the rest of the word -/
  | d :: ω, x => word_action ω (symbolic_step d x)

/-- The affine constant associated with a division word. -/
def affine_constant : division_word → ℕ
  | [] => 0
  /- Recursively compute the affine constant. -/
  | d :: ω =>
      3 ^ ω.length + 2 ^ d * affine_constant ω

/-- The formal affine map associated with a division word. -/
def affine_map (ω : division_word) (x : ℚ) : ℚ :=
  ((3 : ℚ) ^ ω.length * x + (affine_constant ω : ℚ)) /
    (2 : ℚ) ^ total_division_count ω

/--
The recursively composed symbolic action of a division word agrees
with its closed-form affine representation.

This theorem is purely algebraic: it holds for every division word,
whether or not the word is realizable by an actual Collatz trajectory.
The proof proceeds by induction on the word. In the inductive step,
the head symbolic step is composed with the affine representation of
the tail, and the resulting rational expression is normalized to the
affine form for the complete word.
-/
theorem word_action_eq_affine_map
    (ω : division_word) (x : ℚ) :
    word_action ω x = affine_map ω x := by
  /- Induct on the division word. Generalizing x is essential because
  the recursive action on the tail begins at symbolic_step d x,
  rather than at the original value x. -/
  induction ω generalizing x with
  | nil =>
      /- For the empty word, the symbolic action is the identity map.
         The affine formula reduces to (3^0 * x + 0) / 2^0 = x. -/
      simp [word_action, affine_map, affine_constant, total_division_count]
  | cons d ω ih =>
      /- Unfold one symbolic step:
         word_action (d :: ω) x becomes
         word_action ω (symbolic_step d x). -/
      rw [word_action]
      /- Apply the induction hypothesis to the tail word, evaluated at
         the result of the head symbolic step. -/
      rw [ih]
      /- Expose the affine formula for the tail and the explicit formula
         for the prescribed head step. -/
      unfold affine_map
      unfold symbolic_step
      /- Simplify the structural invariants of the word `d :: ω`:
          d :: ω| = |ω| + 1,
          D(d :: ω) = d + D(ω),
          c(d :: ω) = 3^|ω| + 2^d c(ω). -/
      simp only [affine_constant, total_division_count,
        List.length_cons, List.sum_cons]
      /- Move coercions from ℕ into the surrounding ℚ arithmetic so that
         the remaining goal is a rational algebraic identity. -/
      push_cast
      /- Rewrite the powers occurring in the full-word affine expression:
          3^(|ω| + 1) = 3^|ω| * 3,
          2^(d + D(ω)) = 2^d * 2^D(ω). -/
      rw [pow_succ, pow_add]
      /- Clear the rational denominators. Powers of 2 are nonzero in ℚ. -/
      field_simp
      /- The remaining equality is a polynomial identity. -/
      ring

/--
Acting by a concatenated division word is the same as first acting by
the prefix word and then by the appended word.
-/
lemma word_action_append
    (ω η : division_word) (x : ℚ) :
    /- The catenation of ω and η is the same is taking
       the action of ω first, then the action of η. -/
    word_action (ω ++ η) x =
      word_action η (word_action ω x) := by
  induction ω generalizing x with
  | nil =>
      simp [word_action]
  | cons d ω ih =>
      -- simp [word_action, ih]
      simp [word_action]
      simp [ih]

/-- Affine maps compose according to concatenation of division words. -/
theorem affine_map_append
    (ω η : division_word) (x : ℚ) :
    affine_map (ω ++ η) x =
      affine_map η (affine_map ω x) := by
  /- affine_map (ω ++ η) = word_action (ω ++ η) -/
  rw [← word_action_eq_affine_map]
  /- word_action (ω ++ η) x =  word_action η (word_action ω x) -/
  rw [word_action_append]
  /-  word_action η (word_action ω x) = affine_map η (word_action ω x) -/
  rw [word_action_eq_affine_map]
  /- word_action ω x = affine_map ω x -/
  rw [word_action_eq_affine_map]

/--
The affine constant of a concatenated division word satisfies the
affine composition law.
-/
lemma affine_constant_append
    (ω η : division_word) :
    affine_constant (ω ++ η) =
      3 ^ η.length * affine_constant ω +
      2 ^ total_division_count ω * affine_constant η := by
  /- Induct on the first word. -/
  induction ω with
  | nil =>
      simp [affine_constant, total_division_count]
  | cons d ω ih =>
    /- Rewrite concatenation through the head of the first word. -/
    rw [List.cons_append]
    /- Unfold the recursive affine constant on the concatenated word. -/
    rw [affine_constant]
    /- Expose the total division count as the sum of the word entries. -/
    unfold total_division_count
    /- Replace the affine constant of the concatenated tail using the
       induction hypothesis. -/
    rw [ih]
    /- The induction hypothesis reintroduces `total_division_count`. -/
    unfold total_division_count
    /- Rewrite the structural identities
         |ω ++ η| = |ω| + |η|,
         (d :: ω).sum = d + ω.sum,
       unfold the remaining recursive affine constant, and distribute
       powers over additive exponents. -/
    simp only [List.length_append, List.sum_cons, affine_constant, pow_add]
    /- The remaining equality is a commutative semiring identity. -/
    ring

/--
The first `m` generated division counts are the first `m` entries
of any longer generated division-count word.
-/
lemma division_counts_take
    (x m n : ℕ) :
    (division_counts x (m + n)).take m =
      division_counts x m := by
  induction m generalizing x with
  | zero =>
      simp [division_counts]
  | succ m ih =>
      /- Rewrite the requested length into successor form so that
        `List.iterate` exposes its first trajectory state. -/
      have hlen : m + 1 + n = (m + n) + 1 := by
        omega
      rw [hlen]
      /- Expose the first generated division count on both sides and
        cancel the common head, reducing the goal to equality of tails. -/
      simp only [division_counts, List.iterate,
        List.map_cons, List.take_succ_cons, List.cons.injEq, true_and]
      /- The remaining tail statement is exactly the induction hypothesis,
        now starting from the next accelerated Collatz state. -/
      change
        List.take m
            (division_counts (fully_accelerated x) (m + n)) =
          division_counts (fully_accelerated x) m
      exact ih (fully_accelerated x)

/--
Any realization of a concatenated division word also realizes
its left-hand prefix.
-/
theorem realizes_append_left
    {x : ℕ} {ω η : division_word}
    (h : realizes x (ω ++ η)) :
    realizes x ω := by
  unfold realizes at h ⊢
  /- Apply `List.take ω.length` to both sides of the realization
     equality for the concatenated word. -/
  have ht := congrArg (List.take ω.length) h
  /- Taking `ω.length` entries from `ω ++ η` recovers `ω`, while
     `division_counts_take` says that taking the same prefix from the
     generated word recovers the first `ω.length` division counts. -/
  simpa [List.length_append, division_counts_take] using ht

/--
A symbolic step using the actual division count of `x` agrees with
one fully accelerated Collatz step after coercion to `ℚ`.

The symbolic step is defined using division in `ℚ`, whereas
`fully_accelerated` uses natural-number division. The definition of
`division_count` guarantees that the required power of two divides
`3*x + 1` exactly, so these two forms of division agree after coercion.
-/
lemma symbolic_step_division_count
    (x : ℕ) :
    symbolic_step (division_count x) (x : ℚ) =
      (fully_accelerated x : ℚ) := by
  /- Expose the rational symbolic step and the natural-number accelerated
     Collatz step. The remaining difference is the type in which division
     is performed. -/
  unfold symbolic_step
  unfold fully_accelerated
  /- By definition of the 2-adic valuation, the corresponding power of
     two divides `3*x + 1` exactly. The inequality required by
     `padicValNat_dvd_iff_le` is reflexive because the exponent is the
     valuation itself. -/
  have hdiv :
      2 ^ division_count x ∣ 3 * x + 1 := by
    unfold division_count
    exact (padicValNat_dvd_iff_le
      (p := 2)
      (a := 3 * x + 1)
      (n := padicValNat 2 (3 * x + 1))
      (by omega)).mpr le_rfl
  /- Since the natural-number division is exact, `Nat.cast_div` rewrites
     the coerced natural quotient as division in `ℚ`. This generates a
     side condition asserting that the coerced denominator is nonzero. -/
  rw [Nat.cast_div hdiv]
    /- Normalize the casts of the numerator and denominator. This identifies
      the resulting rational quotient with the symbolic-step expression. -/
  · norm_num
    /- The denominator is a power of 2, and is therefore nonzero. -/
  · positivity

/--
If a natural number `x` realizes the division word `ω`, then the
symbolic action encoded by `ω` agrees, after coercion to `ℚ`, with
`ω.length` actual iterations of the fully accelerated Collatz map.

This is the dynamical bridge between the formal symbolic action and
the actual Collatz trajectory. The proof proceeds by induction on the
realized word. In the inductive step, realization of `d :: ω` splits
into the statement that `d` is the actual division count at `x` and
the statement that the tail `ω` is realized starting from
`fully_accelerated x`.
-/
theorem word_action_realized
    {x : ℕ} {ω : division_word}
    (h : realizes x ω) :
    word_action ω (x : ℚ) =
      (((fully_accelerated^[ω.length]) x : ℕ) : ℚ) := by
  /- Induct on the realized division word. Generalizing `x` is essential:
     after consuming the head of the word, the tail is realized from the
     new starting value `fully_accelerated x`, not from the original `x`. -/
  induction ω generalizing x with
  | nil =>
      /- The empty word performs no symbolic steps, and zero iterations of
      `fully_accelerated` also leave `x` unchanged. -/
      simp [word_action]
  | cons d ω ih =>
      /- Expand realization of `d :: ω`. The generated division-count list
         begins with `division_count x`, followed by the division counts
         generated from `fully_accelerated x`. Thus realization decomposes
         into a head equality and a realization statement for the tail. -/
      unfold realizes at h
      simp only [division_counts, List.length_cons, List.iterate,
        List.map_cons, List.cons.injEq] at h
      /- `hd` identifies the prescribed head count with the actual division
         count at `x`. `htail` describes the remaining generated counts
         starting from the next Collatz state. -/
      rcases h with ⟨hd, htail⟩
      /- Repackage the low-level equality `htail` as the higher-level fact
         that the tail word `ω` is realized by `fully_accelerated x`. -/
      have hrealizes_tail :
          realizes (fully_accelerated x) ω := by
        unfold realizes
        unfold division_counts
        exact htail
      /- Expose the first symbolic step of the word action. -/
      rw [word_action]
      /- Replace the prescribed head exponent `d` by the actual division
         count at `x`, using the realization information. -/
      rw [hd]
      /- The symbolic step using the actual division count agrees with one
         genuine fully accelerated Collatz step after coercion to `ℚ`. -/
      rw [symbolic_step_division_count]
      /- Apply the induction hypothesis to the realized tail, now starting
         from `fully_accelerated x`. -/
      rw [ih hrealizes_tail]
      /- Finally, `|d :: ω| = |ω| + 1`, and one additional iteration of
         `fully_accelerated` is exactly the head step followed by the
         `ω.length` iterations represented by the tail. -/
      simp only [List.length_cons, Function.iterate_succ_apply]


/--
If `x` realizes the division word `ω`, then the formal affine map
associated with `ω` agrees, after coercion to `ℚ`, with the actual
`ω.length`-step fully accelerated Collatz trajectory starting at `x`.

This follows immediately by combining the algebraic identity
`word_action_eq_affine_map` with the dynamical bridge
`word_action_realized`.
-/
theorem affine_map_realized
    {x : ℕ} {ω : division_word}
    (h : realizes x ω) :
    affine_map ω (x : ℚ) =
      ((fully_accelerated^[ω.length]) x : ℚ) := by
  /- Replace the affine map by the recursively composed symbolic action. -/
  rw [← word_action_eq_affine_map]
  /- For a realized word, that symbolic action is exactly the corresponding
  finite segment of the actual fully accelerated Collatz trajectory. -/
  exact word_action_realized h

/-- The total division count contributed by the first `j` entries of `ω`. -/
def prefix_division_count
    (ω : division_word) (j : ℕ) : ℕ :=
  total_division_count (ω.take j)

/-- The explicit summation formula for the affine constant of `ω`. -/
def explicit_affine_constant
    (ω : division_word) : ℕ :=
  ∑ j ∈ Finset.range ω.length,
    3 ^ (ω.length - 1 - j) *
      2 ^ prefix_division_count ω j

/--
Prepending a division count `d` shifts every positive prefix count by one.

The first `j + 1` entries of `d :: ω` consist of the head `d`
followed by the first `j` entries of `ω`. Therefore their total
division count is `d` plus the corresponding prefix division count
of the tail.
-/
lemma prefix_division_count_cons_succ
    (d : ℕ) (ω : division_word) (j : ℕ) :
    prefix_division_count (d :: ω) (j + 1) =
      d + prefix_division_count ω j := by
  /- Reduce the statement to sums of list prefixes. -/
  unfold prefix_division_count
  unfold total_division_count
  /- `take (j + 1) (d :: ω)` is `d :: take j ω`, so the sum of the
     longer prefix is the head `d` plus the sum of the tail prefix. -/
  simp

/--
Split a finite sum over `0, ..., n` into its initial term and a
reindexed sum over the remaining terms.

This is the "peel off the first term" counterpart to
`Finset.sum_range_succ`, which naturally peels off the last term:
∑ j=0 to n f(j) = f(0) + ∑ j=0 to n-1 f(j+1)

The shifted form is useful when a recursive construction separates
the contribution at index zero from the contributions associated
with the tail of a sequence.
-/
lemma sum_range_succ_shift
    {M : Type*} [AddCommMonoid M]
    (f : ℕ → M) (n : ℕ) :
    (∑ j ∈ Finset.range (n + 1), f j) =
      f 0 + ∑ j ∈ Finset.range n, f (j + 1) := by
  /- Induct on the upper range parameter. The base case contains only
     the term `f 0`. -/
  induction n with
  | zero =>
      simp
  | succ n ih =>
      /- Peel the final term from the sum on the left. -/
      rw [Finset.sum_range_succ]
      /- Rewrite the preceding range sum using the shifted form supplied
         by the induction hypothesis. -/
      rw [ih]
      /- Peel the final term from the shifted range sum on the right.
         Its new final term is `f (n + 1)`, matching the term peeled
         from the original sum. -/
      rw [Finset.sum_range_succ]
      /- The two sides now differ only by reassociation of addition. -/
      simp [add_assoc]

/--
The explicit affine constant satisfies the same head-tail recurrence as
the recursively defined affine constant.

For a word `d :: ω`, the index-zero term of the explicit sum contributes
exactly `3 ^ ω.length`. Every remaining term corresponds to an index of
the tail `ω`. By `prefix_division_count_cons_succ`, its prefix division
count acquires an additional initial contribution `d`, so its dyadic factor
contains a common factor `2 ^ d`. Thus cₑₓₚ(d :: ω) = 3^|ω| + 2^d cₑₓₚ(ω).

This recurrence is the key step used later to prove that the recursive
and explicit definitions of the affine constant are identical.
-/
lemma explicit_affine_constant_cons
    (d : ℕ) (ω : division_word) :
    explicit_affine_constant (d :: ω) =
      3 ^ ω.length +
        2 ^ d * explicit_affine_constant ω := by
  /- Expand the explicit finite-sum definition. The length of `d :: ω`
     becomes `ω.length + 1`. -/
  unfold explicit_affine_constant
  simp only [List.length_cons]
  /- Split the sum into the index-zero contribution and the reindexed
     tail sum. This is exactly the form needed to separate the head `d`
     from the remainder of the division word. -/
  rw [sum_range_succ_shift]
  /- The prefix preceding the zeroth entry is empty, so its total
     division count is zero. -/
  have hp0 :
      prefix_division_count (d :: ω) 0 = 0 := by
    simp [prefix_division_count, total_division_count]
  /- Simplify the index-zero term. It contributes
     3 ^ ω.length * 2 ^ 0 = 3 ^ ω.length. -/
  rw [hp0]
  simp only [
    Nat.add_sub_cancel,
    Nat.sub_zero,
    pow_zero,
    mul_one
  ]
  /- The leading terms now agree. It remains to show that the shifted
     tail sum is `2 ^ d` times the explicit affine constant of `ω`. -/
  congr 1
  /- Distribute the common factor `2 ^ d` across the tail sum so that
     corresponding summands can be compared individually. -/
  rw [Finset.mul_sum]
  apply Finset.sum_congr rfl
  intro j _
  /- Reindex the ternary exponent. Natural-number subtraction is truncated,
     and this identity holds uniformly for every `j`. -/
  have hexp :
    ω.length - (j + 1) =
      ω.length - 1 - j := by
    omega
  /- The first `j + 1` entries of `d :: ω` consist of `d` followed by
     the first `j` entries of `ω`. Hence their cumulative division count
     is `d + prefix_division_count ω j`. -/
  rw [prefix_division_count_cons_succ]
  /- Align the ternary powers of the shifted summand with those in the
  explicit affine constant of the tail. -/
  rw [hexp]
  /- Split the dyadic factor
     2 ^ (d + D_j) = 2 ^ d * 2 ^ D_j. -/
  rw [pow_add]
  /- The two summands are now identical up to rearrangement of
     multiplication. -/
  ring

/--
The recursively defined affine constant agrees with its explicit
prefix-sum formula.

The proof uses the fact that both constructions satisfy the same
head-tail recurrence. For the empty word both constants are zero.
For a nonempty word `d :: ω`, the recursive definition gives
c(d :: ω) = 3^|ω| + 2^d c(ω), while `explicit_affine_constant_cons`
establishes the identical recurrence for the explicit prefix-sum
formula. The induction hypothesis then identifies the constants
associated with the tail.

Consequently, the recursive definition used to construct the affine
map and the explicit summation formula are two representations of the
same affine constant.
-/
theorem affine_constant_eq_explicit
    (ω : division_word) :
    affine_constant ω = explicit_affine_constant ω := by
  /- Induct on the structure of the division word. -/
  induction ω with
  | nil =>
      /- For the empty word, both definitions evaluate to zero. -/
      simp [affine_constant, explicit_affine_constant]
  | cons d ω ih =>
      /- Expand the recursive affine constant at the head of the word. -/
      rw [affine_constant]
      /- Rewrite the explicit constant using the matching head-tail
         recurrence proved above. -/
      rw [explicit_affine_constant_cons]
      /- The remaining equality is exactly the induction hypothesis for
         the tail `ω`. -/
      rw [ih]

/-- The least dyadic exponent whose power of two exceeds `3^m`. -/
def dyadic_threshold (m : ℕ) : ℕ :=
  (3 ^ m).log2 + 1

/--
The dyadic threshold `dyadic_threshold m` characterizes exactly those
exponents `k` for which the dyadic factor `2 ^ k` exceeds the ternary
factor `3 ^ m`.

Equivalently, B(m) ≤ K ↔ 3^m < 2^k.

Thus `dyadic_threshold m` is not merely a boundary value: every
exponent at or above the threshold gives strict dyadic dominance,
and every exponent below it fails to do so.
-/
theorem dyadic_threshold_le_iff
    (m k : ℕ) :
    dyadic_threshold m ≤ k ↔
      3 ^ m < 2 ^ k := by
  /- By definition, the threshold is one greater than the base-two
     logarithm of `3 ^ m`. -/
  unfold dyadic_threshold
  /- The power `3 ^ m` is always nonzero. This is required by
     `Nat.log2_lt`, which relates the natural logarithm to powers of two. -/
  have hnonzero : 3 ^ m ≠ 0 := by
    positivity
  constructor
  · intro h
    /- From
        (3 ^ m).log2 + 1 ≤ k
       obtain the equivalent strict inequality
        (3 ^ m).log2 < k. -/
    have hlog : (3 ^ m).log2 < k := by
      omega
    /- `Nat.log2_lt` converts this logarithmic inequality exactly into
       the corresponding power-of-two inequality. -/
    exact (Nat.log2_lt hnonzero).mp hlog
  · intro h
    /- Conversely, strict dyadic dominance gives
        (3 ^ m).log2 < k
       by the reverse direction of `Nat.log2_lt`. -/
    have hlog : (3 ^ m).log2 < k := by
      exact (Nat.log2_lt hnonzero).mpr h
    /- Re-express the strict logarithmic inequality as
        (3 ^ m).log2 + 1 ≤ k,
       which is precisely the threshold inequality. -/
    omega

/--
For a division word `ω`, its total division count reaches the dyadic
threshold exactly when its cumulative dyadic factor exceeds its
cumulative ternary factor.

Specializing `dyadic_threshold_le_iff` to `m = ω.length` and
`k = total_division_count ω` gives B(|ω|) ≤ D(ω) ↔ 3^|ω| < 2^(D(ω)).

Thus the threshold condition can be expressed either combinatorially,
as a lower bound on the total division count, or multiplicatively,
as strict dyadic dominance over the ternary factor.

Equivalently, this is precisely the condition under which the linear
coefficient 3^|ω| / 2^(D(ω)) of the affine map is strictly less than one.
This statement concerns the linear coefficient only; it does not by itself
assert that the affine map is contractive on its realizations.
-/
theorem dyadic_threshold_le_total_iff
    (ω : division_word) :
    dyadic_threshold ω.length ≤ total_division_count ω ↔
      3 ^ ω.length <
        2 ^ total_division_count ω := by
  /- Apply the general dyadic-threshold characterization with the word
     length as the ternary exponent and the total division count as the
     dyadic exponent. -/
  exact dyadic_threshold_le_iff
    ω.length
    (total_division_count ω)

/--
The affine constant of every nonempty division word is strictly positive.

The empty word is the unique degenerate case, with `affine_constant [] = 0`.
For a nonempty word `d :: ω`, the recursive definition gives
  c(d :: ω) = 3^|ω| + 2^d c(ω),
whose first term is already strictly positive and whose second term is
nonnegative. Hence the entire affine constant is positive.
-/
lemma affine_constant_pos
    {ω : division_word}
    (hω : 0 < ω.length) :
    0 < affine_constant ω := by
  /- Split on whether the word is empty or has a head. The hypothesis
     rules out the empty case immediately. -/
  cases ω with
  | nil =>
      simp at hω
  | cons d ω =>
      /- For a nonempty word, the recursive definition contains the
      positive term `3 ^ ω.length`; simplification is enough to show
      that the full sum is therefore strictly positive. -/
      simp [affine_constant]

/--
The dyadic threshold is the least natural exponent whose power of two
strictly exceeds `3 ^ m`.

In set-theoretic form, `dyadic_threshold m` is the least element of
k ∈ ℕ : 3^m < 2^k.

This packages the minimality statement implicit in
`dyadic_threshold_le_iff` into the exact order-theoretic form used by
the paper: `B(m)` is the smallest exponent at which strict dyadic
dominance begins.
-/
theorem dyadic_threshold_isLeast
      (m : ℕ) :
      IsLeast
        {k : ℕ | 3 ^ m < 2 ^ k}
        (dyadic_threshold m) := by
  /- To prove `IsLeast`, establish first that the threshold itself belongs
     to the set, and then that it is less than or equal to every other
     member of the set. -/
  constructor
  /- Membership in the set is definitionally the inequality
     3 ^ m < 2 ^ dyadic_threshold m. -/
  · change 3 ^ m < 2 ^ dyadic_threshold m
    /- Apply the threshold characterization at the threshold itself.
       Reflexivity gives
         dyadic_threshold m ≤ dyadic_threshold m,
       so strict dyadic dominance follows. -/
    exact
      (dyadic_threshold_le_iff
        m
        (dyadic_threshold m)).mp le_rfl
  /- Any exponent `k` already satisfying strict dyadic dominance must lie
     at or above the threshold. -/
  · intro k hk
    exact
      (dyadic_threshold_le_iff m k).mpr hk

/--
The integer dyadic threshold agrees with the real-logarithmic
floor formula used in the paper.

The threshold is defined intrinsically by
`Nat.log2 (3 ^ m) + 1`, avoiding real arithmetic in the basic
threshold theory. This theorem connects that discrete definition to
the equivalent expression B(m) = ⌊ m log₂ 3 ⌋ + 1.

Thus the integer threshold characterized by
`dyadic_threshold_le_iff` is the same sequence described by the
familiar logarithmic formula.
-/
theorem dyadic_threshold_eq_floor_log
    (m : ℕ) :
    dyadic_threshold m =
      ⌊(m : ℝ) * Real.logb 2 3⌋₊ + 1 := by
  /- Expand the intrinsic definition
     B(m) = log2(3 ^ m) + 1. -/
  unfold dyadic_threshold
  /- Both sides have the same final successor. It remains to identify
     the integer logarithm with the natural floor of the real logarithm. -/
  congr 1
  /- Express the specialized base-two natural logarithm as the general
     natural logarithm with base `2`. -/
  rw [Nat.log2_eq_log_two]
  /- Replace `Nat.log 2 (3 ^ m)` by the natural-number floor of the
     corresponding real base-two logarithm:
      log_2(3 ^ m) ↦ ⌊Real.logb 2 (3 ^ m)⌋₊. -/
  rw [← Real.natFloor_logb_natCast 2 (3 ^ m)]
  /- Push the coercion from `ℕ` to `ℝ` through exponentiation, turning
     the cast of the natural power `3 ^ m` into the real power
     `(3 : ℝ) ^ m`. -/
  push_cast
  /- Apply the real logarithmic power law:
      log_2(3 ^ m) = m * log_2(3).
     The expressions inside the two natural floors are now identical. -/
  rw [Real.logb_pow]

/--
The linear coefficient, or slope, of the affine map associated with
a division word `ω`.

The numerator `3 ^ ω.length` records the cumulative ternary growth,
while the denominator `2 ^ total_division_count ω` records the
cumulative dyadic division. Thus
  affine_slope ω = 3^|ω| / 2^D(ω).

The slope is defined over `ℚ`, consistently with the formal affine
map, so it is defined for every division word independently of
realizability.
-/
def affine_slope (ω : division_word) : ℚ :=
  (3 : ℚ) ^ ω.length /
    (2 : ℚ) ^ total_division_count ω

/--
The affine slope is strictly less than one exactly when the total
division count reaches the dyadic threshold for the word length.

Since affine_slope(ω) = 3^|ω| / 2^D(ω), the inequality
`affine_slope ω < 1` is equivalent to strict dyadic dominance,
3^|ω| < 2^D(ω).

By `dyadic_threshold_le_total_iff`, this in turn is equivalent to
B(|ω|) ≤ D(ω).

The proof crosses between two numeric domains: `affine_slope` is defined
over `ℚ`, while the dyadic-threshold theorem is stated intrinsically over
`ℕ`. The casts between the corresponding power inequalities are handled
explicitly with `exact_mod_cast`.
-/
theorem affine_slope_lt_one_iff
  (ω : division_word) :
  affine_slope ω < 1 ↔
    dyadic_threshold ω.length ≤
      total_division_count ω := by
  /- Expand the rational slope
     3^|ω| / 2^D(ω). -/
  unfold affine_slope
  /- The denominator is strictly positive, so a quotient is less than one
     exactly when its numerator is less than its denominator. -/
  rw [div_lt_one (by positivity)]
  constructor
  · intro h
    /- Convert the rational power inequality to the corresponding natural
       inequality, then apply the dyadic-threshold characterization. -/
    apply (dyadic_threshold_le_total_iff ω).mpr
    exact_mod_cast h
  · intro h
    /- First obtain strict dyadic dominance in `ℕ` from the threshold
       condition. -/
    have hnat :
        3 ^ ω.length <
          2 ^ total_division_count ω := by
      exact (dyadic_threshold_le_total_iff ω).mp h
    /- Cast the natural inequality into `ℚ`, which is the domain of the
       affine slope. -/
    exact_mod_cast hnat

/--
For every nonempty division word, the cumulative dyadic and ternary
factors are unequal.

The only common value of a power of `2` and a power of `3` in `ℕ`
is `1 = 2 ^ 0 = 3 ^ 0`. The hypothesis `0 < ω.length` excludes the
zero ternary exponent.

More explicitly, if `total_division_count ω = 0`, then the dyadic
factor is `1` while `3 ^ ω.length > 1`. If the total division count
is nonzero, then `2 ^ total_division_count ω` is divisible by `2`,
whereas `3 ^ ω.length` is odd. Hence equality is impossible.
-/
lemma dyadic_ternary_ne
    {ω : division_word}
    (hω : 0 < ω.length) :
    2 ^ total_division_count ω ≠ 3 ^ ω.length := by
  intro h
  by_cases hD : total_division_count ω = 0
  /- If the total division count is zero, the dyadic power is `1`.
     Since the word is nonempty, the ternary power is strictly
     greater than `1`. -/
  · rw [hD] at h
    simp only [pow_zero] at h
    have hlen0 : ω.length ≠ 0 := by
      omega
    have hgt : 1 < 3 ^ ω.length := by
      exact one_lt_pow₀ (by norm_num) hlen0
    omega
  /- A nonzero natural total division count is a successor. -/
  · obtain ⟨n, hn⟩ :
        ∃ n, total_division_count ω = n + 1 := by
      use total_division_count ω - 1
      omega
    /- Hence the dyadic power contains a factor of `2`. -/
    have hdiv :
        (2 : ℕ) ∣ 2 ^ total_division_count ω := by
      rw [hn, pow_succ]
      simp
    /- Every power of `3` is odd. -/
    have hodd :
        Odd (3 ^ ω.length) := by
      exact Odd.pow (by norm_num)
    /- An odd natural number cannot be divisible by `2`. -/
    have hnotdiv :
        ¬ (2 : ℕ) ∣ 3 ^ ω.length := by
      rintro ⟨q, hq⟩
      rcases hodd with ⟨r, hr⟩
      omega
    /- Equality of the two powers would transfer divisibility by `2`
       to the odd ternary power, giving the contradiction. -/
    have hdiv' :
        (2 : ℕ) ∣ 3 ^ ω.length := by
      rw [← h]
      exact hdiv
    exact hnotdiv hdiv'

/--
The affine slope of every nonempty division word is strictly separated
from one.

Since affine_slope(ω) = 3^|ω| / 2^D(ω), equality with `1` would force
3^|ω| = 2^D(ω).

The theorem `dyadic_ternary_ne` excludes this equality for every
nonempty division word. Hence the affine slope cannot equal one.

Because `ℚ` is linearly ordered, the remaining possibilities are
therefore exhaustive:
    affine_slope(ω) < 1 or 1 < affine_slope(ω).

This establishes a strict slope dichotomy only; it does not assert a
corresponding dichotomy between contractive and expansive affine maps.
-/
theorem affine_slope_lt_or_gt_one
    {ω : division_word}
    (hω : 0 < ω.length) :
    affine_slope ω < 1 ∨ 1 < affine_slope ω := by
  /- First show that the rational affine slope cannot equal `1`. -/
  have hslope_ne : affine_slope ω ≠ 1 := by
    intro h
    /- Expand the slope definition. Under the assumption that the quotient
       equals `1`, clear the nonzero dyadic denominator. -/
    unfold affine_slope at h
    have hpow :
        (3 : ℚ) ^ ω.length =
          (2 : ℚ) ^ total_division_count ω := by
      field_simp at h
      exact h
    /- Transport the equality of rational powers back to the corresponding
       equality in `ℕ`. -/
    have hpow_nat :
        3 ^ ω.length =
          2 ^ total_division_count ω := by
      exact_mod_cast hpow
    /- This contradicts the previously established fact that nonempty words
       cannot have equal dyadic and ternary factors. -/
    exact
      (dyadic_ternary_ne hω)
        hpow_nat.symm
  /- In a linear order, any value unequal to `1` is either strictly below
     or strictly above it. -/
  exact lt_or_gt_of_ne hslope_ne

/- End of the Collatz namespace. -/
end Collatz
