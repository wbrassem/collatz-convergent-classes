/-
Copyright (c) 2026 Wayne Brassem. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.
Authors: Wayne Brassem
-/

import Collatz.Affine

/-!

# Realizability of the Affine Representation

This module develops the exact realizability theory for the affine
representation of division words.

The affine development in `Collatz.Affine` associates to every division
word `ω` the formal map

`F_ω(x) = (3^|ω| * x + c(ω)) / 2^(D(ω))`.

That representation is purely algebraic. The purpose of the present module
is to determine exactly when this formal affine data corresponds to an
actual finite trajectory of the fully accelerated Collatz map.

The central distinction is between two dyadic conditions.

Divisibility by

`2^(D(ω))`

guarantees only formal integrality of the affine numerator. Exact
realization requires one additional parity bit:

`3^|ω| * x + c(ω) ≡ 2^(D(ω)) mod 2^(D(ω)+1)`.

Equivalently, the affine numerator must have the form

`2^(D(ω)) * q`

with `q` odd. Thus the extra modulus bit records that the prescribed total
power of `2` has been removed exactly, rather than merely dividing the
numerator.

The development has the following main structure:

`realized word`
│
▼
actual accelerated iterate
│
▼
natural affine numerator identity
│
▼
exact congruence mod `2^(D(ω)+1)`

and, in the reverse direction,

`exact congruence mod 2^(D(ω)+1)`
│
▼
integrality mod `2^(D(ω))`
│
▼
integrality of every prefix
│
▼
legal first prescribed division
│
▼
exact congruence transported to the tail
│
▼
odd tail endpoint
│
▼
exact head valuation
│
▼
induction on the word
│
▼
`realizes x ω`

The first layer establishes several arithmetic bridges between formal affine
expressions and the actual accelerated map. In particular, positive
accelerated iterates are odd, and a realized word satisfies the natural
identity

`3^|ω| * x + c(ω) = 2^(D(ω)) * fully_accelerated^[|ω|] x`.

Since the realized endpoint is odd for every nonempty realized word, this
immediately yields the forward exact congruence modulo
`2^(D(ω)+1)`.

The second layer develops the arithmetic meaning of that congruence. A
number satisfies

`N ≡ 2^D mod 2^(D+1)`

if and only if

`N = 2^D * q`

for some odd natural number `q`. The same layer also proves the local
equivalence between an exact division count and the corresponding
congruence

`3 * x + 1 ≡ 2^d mod 2^(d+1)`.

The third layer establishes prefix integrality. Global divisibility of the
affine numerator by `2^(D(ω))` implies the corresponding divisibility
condition for every prefix of `ω`. The proof uses the concatenation formula
for affine constants together with coprimality of powers of `2` and powers
of `3`.

The fourth layer proves the reverse realizability theorem. From the global
exact congruence, the first prescribed power `2^d` is shown to divide
`3 * x + 1`. After writing

`3 * x + 1 = 2^d * y`,

the exact congruence is transported to the tail word starting at `y`.
Induction shows that `y` is odd and realizes the tail. This oddness upgrades
the first legal division to the exact valuation `d`, allowing the full word
to be reconstructed dynamically.

These results are packaged in the main equivalence:

for every admissible division word `ω`,

`realizes x ω`

if and only if

`3^|ω| * x + c(ω)
    ≡ 2^(D(ω)) mod 2^(D(ω)+1)`.

The final layer develops the residue structure implied by this criterion.
Because `3^|ω|` is coprime to every power of `2`, the exact affine
congruence has a unique canonical solution modulo `2^(D(ω)+1)`.
Consequently every admissible division word has a unique exact realization
residue class, every admissible word is realizable, and realization classes
of prefixes are nested. More generally, realization sets of arbitrary
division words are either nested or disjoint whenever they intersect through
a common realized trajectory.

This module therefore provides the bridge from the formal affine encoding
to the exact congruence and residue structure used in the subsequent
development.
-/

/- Open the Collatz namespace. -/
namespace Collatz

/--
Every strictly positive iterate of the fully accelerated Collatz map is odd.

The proof is by induction on the number of additional iterates. The starting
value `x` is generalized so that the induction hypothesis can be applied
after replacing `x` by `fully_accelerated x`.

For `n = 0`, this is exactly the fact that one application of
`fully_accelerated` always produces an odd value. For the inductive step,
one iterate is peeled off and the induction hypothesis is applied to the
new starting value.
-/
lemma fully_accelerated_iterate_succ_odd
    (x n : ℕ) :
    Odd ((fully_accelerated^[n.succ]) x) := by
  induction n generalizing x with
  | zero =>
      /- One positive iterate is odd by `fully_accelerated_odd`. -/
      simpa [Function.iterate_succ_apply] using
        fully_accelerated_odd x
  | succ n ih =>
      /- Peel off the first iterate, then apply the induction hypothesis
         to the new starting value `fully_accelerated x`. -/
      rw [Function.iterate_succ_apply]
      exact ih (fully_accelerated x)

/--
A realized-length iterate is odd whenever the division word is nonempty.

Since `ω.length > 0`, the word cannot be empty, so its length has the form
`n + 1`. The result then follows immediately from
`fully_accelerated_iterate_succ_odd`.

This lemma packages the positivity-of-length condition needed later when a
realized word is used to show that the endpoint of the corresponding
accelerated trajectory is odd.
-/
lemma fully_accelerated_iterate_length_odd
    (x : ℕ) {ω : division_word}
    (hω : 0 < ω.length) :
    Odd ((fully_accelerated^[ω.length]) x) := by
  cases ω with
  | nil =>
      /- The empty word contradicts the positive-length hypothesis. -/
      simp at hω
  | cons d ω =>
      /- A nonempty word has successor length, so the general positive-
         iterate oddness lemma applies directly. -/
      simp only [List.length_cons]
      exact fully_accelerated_iterate_succ_odd x ω.length

/--
If `x` realizes `ω`, then the affine numerator factors exactly as the
prescribed dyadic denominator times the actual accelerated endpoint.

Starting from the realized affine identity

`affine_map ω x = fully_accelerated^[ω.length] x`,

the definition of `affine_map` is unfolded and the denominator
`2^(D(ω))` is cleared. The resulting equality is then cast back from `ℚ`
to `ℕ`.

This identity is the key arithmetic bridge used to derive the exact
realization congruence modulo `2^(D(ω)+1)`.
-/
lemma affine_numerator_eq_of_realizes
    {x : ℕ} {ω : division_word}
    (h : realizes x ω) :
    3 ^ ω.length * x + affine_constant ω =
      2 ^ total_division_count ω *
        ((fully_accelerated^[ω.length]) x) := by
  /- A realized word agrees with its affine map over `ℚ`. -/
  have hq := affine_map_realized h
  /- Expose the affine numerator and denominator. -/
  unfold affine_map at hq
  /- Clear the rational denominator `2^(D(ω))`. -/
  field_simp at hq
  /- The resulting equality is between natural-number expressions,
     so cast it back from `ℚ` to `ℕ`. -/
  exact_mod_cast hq

/--
A realized nonempty division word satisfies the exact affine congruence
modulo one power of `2` beyond the formal denominator.

If `x` realizes `ω`, then

`3^|ω| * x + c(ω)
    ≡ 2^(D(ω)) mod 2^(D(ω)+1)`.

The proof starts from the natural affine numerator identity

`3^|ω| * x + c(ω)
    = 2^(D(ω)) * y`,

where

`y = fully_accelerated^[ω.length] x`

is the actual endpoint of the realized trajectory segment.

Because `ω` is nonempty, the endpoint `y` is odd. Hence

`y ≡ 1 mod 2`.

Scaling this congruence by `2^(D(ω))` gives

`2^(D(ω)) * y
    ≡ 2^(D(ω)) mod 2^(D(ω)+1)`,

which is exactly the desired affine congruence.

Thus the extra modulus bit records the oddness of the realized endpoint:
divisibility by `2^(D(ω))` alone gives formal integrality, while the
congruence modulo `2^(D(ω)+1)` certifies that the prescribed total dyadic
division is exact.
-/
theorem realizes_modEq_succ
    {x : ℕ} {ω : division_word}
    (h : realizes x ω)
    (hω : 0 < ω.length) :
    Nat.ModEq
      (2 ^ (total_division_count ω + 1))
      (3 ^ ω.length * x + affine_constant ω)
      (2 ^ total_division_count ω) := by
  /- Replace the affine numerator by `2^D` times the actual realized
     endpoint. -/
  rw [affine_numerator_eq_of_realizes h]
  /- A positive number of fully accelerated iterations ends at an odd
     natural number. -/
  have hodd :
      Odd ((fully_accelerated^[ω.length]) x) := by
    exact fully_accelerated_iterate_length_odd x hω
  /- An odd natural number is congruent to `1` modulo `2`. -/
  have hmod2 :
      Nat.ModEq 2
        ((fully_accelerated^[ω.length]) x)
        1 := by
    rcases hodd with ⟨q, hq⟩
    rw [hq]
    exact Nat.ModEq.modulus_mul_add
  /- Multiply the congruence by `2^D`. `mul_left'` scales not only both
     sides but also the modulus:
       y ≡ 1 [MOD 2]
       ⇒
       2^D y ≡ 2^D [MOD 2^D * 2]. -/
  have hscaled :=
    hmod2.mul_left'
      (2 ^ total_division_count ω)
  /- `2^D * 2 = 2^(D+1)`, so this is exactly the desired congruence. -/
  simpa [pow_succ] using hscaled

/--
Characterizes the exact dyadic congruence modulo one additional power of `2`.

For natural numbers `N` and `D`,

`N ≡ 2^D mod 2^(D+1)`

if and only if

`N = 2^D * q`

for some odd natural number `q`.

The forward direction first shows that `2^D` divides `N`, since `N` is
congruent to `2^D` modulo a multiple of `2^D`. After writing

`N = 2^D * q`,

the common factor `2^D` is cancelled from the congruence, leaving

`q ≡ 1 mod 2`,

which is exactly the statement that `q` is odd.

Conversely, if

`N = 2^D * q`

with `q` odd, then `q ≡ 1 mod 2`. Scaling that congruence by `2^D`
produces

`N ≡ 2^D mod 2^(D+1)`.

This lemma gives the precise arithmetic interpretation of the additional
modulus bit used throughout the realizability theory: the congruence modulo
`2^(D+1)` does not merely assert divisibility by `2^D`; it asserts that
the quotient after removing `2^D` is odd.
-/
lemma modEq_pow_succ_iff_eq_pow_mul_odd
    (N D : ℕ) :
    Nat.ModEq
        (2 ^ (D + 1))
        N
        (2 ^ D)
      ↔
    ∃ q : ℕ,
      Odd q ∧
      N = 2 ^ D * q := by
  constructor
  · intro h
    /- The lower power `2^D` divides the modulus `2^(D+1)`. -/
    have hpow_dvd :
        2 ^ D ∣ 2 ^ (D + 1) := by
      rw [pow_succ]
      exact dvd_mul_right (2 ^ D) 2
    /- Congruent numbers modulo a multiple of `2^D` have the same
       divisibility by `2^D`. Since `2^D` divides itself, it divides `N`. -/
    have hdiv :
        2 ^ D ∣ N := by
      exact
        (h.dvd_iff hpow_dvd).mpr
          (dvd_refl (2 ^ D))
    /- Factor the full dyadic power from `N`. -/
    rcases hdiv with ⟨q, hq⟩
    /- After substituting `N = 2^D * q`, cancel the common factor `2^D`
       from both sides and from the modulus. This leaves
         q ≡ 1 [MOD 2]. -/
    have hmod2 :
        Nat.ModEq 2 q 1 := by
      apply Nat.ModEq.mul_left_cancel'
        (by positivity : 2 ^ D ≠ 0)
      simpa [pow_succ, hq] using h
    /- Congruence to `1` modulo `2` is exactly oddness. -/
    have hodd : Odd q := by
      simpa [Nat.ModEq, Nat.odd_iff] using hmod2
    exact ⟨q, hodd, hq⟩
  · rintro ⟨q, hodd, hq⟩
    /- An odd number is congruent to `1` modulo `2`. -/
    have hmod2 :
        Nat.ModEq 2 q 1 := by
      simpa [Nat.ModEq, Nat.odd_iff] using hodd
    /- Scale the congruence by `2^D`, including the modulus:
         q ≡ 1 [MOD 2]
       gives
         2^D q ≡ 2^D [MOD 2^D * 2]. -/
    have hscaled :=
      hmod2.mul_left' (2 ^ D)
    /- Replace `N` by its factorization and recognize
       `2^D * 2 = 2^(D+1)`. -/
    simpa [hq, pow_succ] using hscaled

/--
Global affine integrality for a concatenated word descends to its left
prefix.

If the affine numerator for `ω ++ η` is divisible by the full dyadic
denominator

`2^(D(ω ++ η))`,

then the affine numerator for the prefix `ω` is divisible by

`2^(D(ω))`.

The proof first reduces the full congruence to the smaller prefix modulus.
Using the affine concatenation formula, the full numerator is rewritten as

`3^|η| * (3^|ω| * x + c(ω))
    + 2^(D(ω)) * c(η)`.

The second summand vanishes modulo `2^(D(ω))`, leaving divisibility of

`3^|η| * (3^|ω| * x + c(ω))`

by `2^(D(ω))`. Since powers of `2` are coprime to powers of `3`, the
factor `3^|η|` can be cancelled. This yields divisibility of the prefix
affine numerator itself.

This lemma is the key structural step behind prefix integrality: a global
formal integrality condition is not merely an endpoint condition, but
forces the corresponding integrality condition for every left prefix.
-/
lemma affine_integrality_append_left
    (x : ℕ) (ω η : division_word)
    (h :
      Nat.ModEq
        (2 ^ total_division_count (ω ++ η))
        (3 ^ (ω ++ η).length * x +
          affine_constant (ω ++ η))
        0) :
    Nat.ModEq
      (2 ^ total_division_count ω)
      (3 ^ ω.length * x + affine_constant ω)
      0 := by
  /- The prefix modulus divides the modulus of the concatenated word. -/
  have hpow_dvd :
      2 ^ total_division_count ω ∣
        2 ^ total_division_count (ω ++ η) := by
    unfold total_division_count
    rw [List.sum_append, pow_add]
    exact dvd_mul_right _ _
  /- Therefore the full congruence also holds modulo the smaller
     prefix modulus. -/
  have hsmall :
      Nat.ModEq
        (2 ^ total_division_count ω)
        (3 ^ (ω ++ η).length * x +
          affine_constant (ω ++ η))
        0 := by
    exact h.of_dvd hpow_dvd
  /- Rewrite the full affine numerator using the concatenation formula. -/
  have hnum :
      3 ^ (ω ++ η).length * x +
          affine_constant (ω ++ η)
        =
      3 ^ η.length *
          (3 ^ ω.length * x + affine_constant ω)
        +
      2 ^ total_division_count ω *
          affine_constant η := by
    rw [List.length_append]
    rw [affine_constant_append]
    rw [pow_add]
    ring
  rw [hnum] at hsmall
  /- The second summand is already a multiple of the prefix modulus,
     so it can be discarded modulo `2^D(ω)`. -/
  have hremove :
      Nat.ModEq
        (2 ^ total_division_count ω)
        (3 ^ η.length *
            (3 ^ ω.length * x + affine_constant ω) +
          2 ^ total_division_count ω *
            affine_constant η)
        (3 ^ η.length *
            (3 ^ ω.length * x + affine_constant ω)) := by
    simp [add_comm]
  have hprod :
      Nat.ModEq
        (2 ^ total_division_count ω)
        (3 ^ η.length *
          (3 ^ ω.length * x + affine_constant ω))
        0 := by
    exact hremove.symm.trans hsmall
  /- Convert the congruence into divisibility. -/
  have hdvdprod :
      2 ^ total_division_count ω ∣
        3 ^ η.length *
          (3 ^ ω.length * x + affine_constant ω) := by
    exact
      (hprod.dvd_iff (dvd_refl _)).mpr
        (dvd_zero _)
  /- Powers of `2` and powers of `3` are coprime, so the factor
     `3^|η|` can be cancelled from the divisibility statement. -/
  have hcop :
      Nat.Coprime
        (2 ^ total_division_count ω)
        (3 ^ η.length) := by
    exact
      (by norm_num : Nat.Coprime 2 3).pow
        (total_division_count ω)
        η.length
  have hdvdprefix :
      2 ^ total_division_count ω ∣
        3 ^ ω.length * x + affine_constant ω := by
    exact hcop.dvd_of_dvd_mul_left hdvdprod
  /- Repackage divisibility as congruence to zero. -/
  change
    (3 ^ ω.length * x + affine_constant ω) %
        (2 ^ total_division_count ω)
      =
    0 % (2 ^ total_division_count ω)
  simp only [Nat.zero_mod]
  exact Nat.dvd_iff_mod_eq_zero.mp hdvdprefix

/--
Global affine integrality propagates to every prefix of the division word.

If the full affine numerator for `ω` is congruent to `0` modulo
`2^(D(ω))`, then for every prefix length `j ≤ ω.length`, the affine
numerator of the prefix `ω.take j` is congruent to `0` modulo

`2^(D(ω.take j))`.

The proof writes `ω` as

`ω.take j ++ ω.drop j`

and applies `affine_integrality_append_left` to the left factor. The bound
`j ≤ ω.length` is then used only to identify the length of `ω.take j`
with `j`.

Thus a single global integrality condition determines the corresponding
formal integrality condition at every prefix stage.
-/
lemma affine_congruence_prefix
    {x : ℕ} {ω : division_word}
    (h :
      Nat.ModEq
        (2 ^ total_division_count ω)
        (3 ^ ω.length * x + affine_constant ω)
        0) :
    ∀ j ≤ ω.length,
      Nat.ModEq
        (2 ^ total_division_count (ω.take j))
        (3 ^ j * x + affine_constant (ω.take j))
        0 := by
  intro j hj
  have hsplit := h
  /- Regard `ω` as its first `j` entries followed by the remaining
     suffix. -/
  rw [← List.take_append_drop j ω] at hsplit
  have hp :=
    affine_integrality_append_left
      x
      (ω.take j)
      (ω.drop j)
      hsplit
  /- Since `j ≤ |ω|`, the prefix `take j ω` has length exactly `j`. -/
  simpa [List.length_take, Nat.min_eq_left hj] using hp

/--
Forgets the extra parity bit in an exact dyadic congruence.

If

`N ≡ 2^D mod 2^(D+1)`,

then reducing modulo the smaller power `2^D` gives

`N ≡ 0 mod 2^D`.

This lemma is used in the reverse realizability argument to pass from the
exact congruence modulo `2^(D+1)` to the ordinary affine integrality
condition required by the prefix-integrality machinery.
-/
lemma modEq_pow_succ_implies_integrality
    (N D : ℕ)
    (h :
      Nat.ModEq
        (2 ^ (D + 1))
        N
        (2 ^ D)) :
    Nat.ModEq
      (2 ^ D)
      N
      0 := by
  /- The smaller modulus divides the exactness modulus. -/
  have hpow_dvd :
      2 ^ D ∣ 2 ^ (D + 1) := by
    rw [pow_succ]
    exact dvd_mul_right _ _
  /- Reduce the exact congruence modulo `2^D`. -/
  have hsmall :
      Nat.ModEq
        (2 ^ D)
        N
        (2 ^ D) := by
    exact h.of_dvd hpow_dvd
  /- Modulo `2^D`, the value `2^D` itself is congruent to zero. -/
  have hzero :
      Nat.ModEq
        (2 ^ D)
        (2 ^ D)
        0 := by
    simp
  exact hsmall.trans hzero

/--
The global exact congruence for a nonempty word forces the first prescribed
dyadic division to be legal.

For a word `d :: ω`, assume

`3^|d :: ω| * x + c(d :: ω)
    ≡ 2^(D(d :: ω)) mod 2^(D(d :: ω)+1)`.

Then

`2^d ∣ 3 * x + 1`.

The proof first forgets the final parity bit, reducing the exact congruence
to ordinary affine integrality modulo `2^(D(d :: ω))`. Prefix integrality
is then applied to the one-element prefix `[d]`. The corresponding affine
numerator is simply

`3 * x + 1`,

and its total division count is `d`, yielding the desired divisibility.

This lemma establishes only legality of the first prescribed division.
Exactness of the valuation is recovered later by showing that the quotient
after removing `2^d` is odd.
-/
lemma affine_exact_congruence_head_dvd
    {x d : ℕ} {ω : division_word}
    (h :
      Nat.ModEq
        (2 ^ (total_division_count (d :: ω) + 1))
        (3 ^ (d :: ω).length * x +
          affine_constant (d :: ω))
        (2 ^ total_division_count (d :: ω))) :
    2 ^ d ∣ 3 * x + 1 := by
  /- Forget the final parity bit. The exact congruence first gives the
     ordinary integrality congruence modulo the total dyadic factor. -/
  have hint :
      Nat.ModEq
        (2 ^ total_division_count (d :: ω))
        (3 ^ (d :: ω).length * x +
          affine_constant (d :: ω))
        0 := by
    exact
      modEq_pow_succ_implies_integrality
        (3 ^ (d :: ω).length * x +
          affine_constant (d :: ω))
        (total_division_count (d :: ω))
        h
  /- Apply prefix integrality to the first entry of the word. -/
  have hp :=
    affine_congruence_prefix
      (x := x)
      (ω := d :: ω)
      hint
      1
      (by simp)
  /- Convert the resulting prefix congruence into divisibility. -/
  have hdvd :
      2 ^ total_division_count ((d :: ω).take 1) ∣
        3 ^ 1 * x +
          affine_constant ((d :: ω).take 1) := by
    exact
      (hp.dvd_iff (dvd_refl _)).mpr
        (dvd_zero _)
  /- The one-element prefix is `[d]`, whose total division count is `d`
     and whose affine constant is `1`. -/
  simpa [total_division_count, affine_constant] using hdvd

/--
Transports the exact affine congruence from a nonempty word to its tail
after the first prescribed dyadic division has been factored off.

Suppose

`3 * x + 1 = 2^d * y`

and the full word `d :: ω` satisfies its exact realization congruence.
Then the tail word `ω`, now starting from `y`, satisfies the corresponding
exact congruence

`3^|ω| * y + c(ω)
    ≡ 2^(D(ω)) mod 2^(D(ω)+1)`.

The proof rewrites the full affine numerator as

`2^d * (3^|ω| * y + c(ω))`.

Likewise,

`D(d :: ω) = d + D(ω)`,

so both sides and the modulus of the original congruence contain the common
factor `2^d`. Cancelling that nonzero factor yields exactly the tail
congruence.

This is the recursive transport step in the reverse realizability proof:
once the first prescribed division is known to be legal, the same exact
congruence structure reappears for the remaining word.
-/
lemma affine_exact_congruence_tail
    {x y d : ℕ} {ω : division_word}
    (hy : 3 * x + 1 = 2 ^ d * y)
    (h :
      Nat.ModEq
        (2 ^ (total_division_count (d :: ω) + 1))
        (3 ^ (d :: ω).length * x +
          affine_constant (d :: ω))
        (2 ^ total_division_count (d :: ω))) :
    Nat.ModEq
      (2 ^ (total_division_count ω + 1))
      (3 ^ ω.length * y + affine_constant ω)
      (2 ^ total_division_count ω) := by
  /- After substituting the legal first division
       `3x + 1 = 2^d y`,
     the full affine numerator factors as `2^d` times the tail numerator. -/
  have hnum :
      3 ^ (d :: ω).length * x +
          affine_constant (d :: ω)
        =
      2 ^ d *
        (3 ^ ω.length * y + affine_constant ω) := by
    simp only [List.length_cons, affine_constant]
    rw [pow_succ]
    calc
      3 ^ ω.length * 3 * x +
            (3 ^ ω.length + 2 ^ d * affine_constant ω)
          =
        3 ^ ω.length * (3 * x + 1) +
            2 ^ d * affine_constant ω := by
          ring
      _ =
        3 ^ ω.length * (2 ^ d * y) +
            2 ^ d * affine_constant ω := by
          rw [hy]
      _ =
        2 ^ d *
          (3 ^ ω.length * y + affine_constant ω) := by
          ring
  rw [hnum] at h
  /- The total dyadic count of `d :: ω` splits into the head exponent
     plus the total dyadic count of the tail. -/
  have hD :
      total_division_count (d :: ω) =
        d + total_division_count ω := by
    simp [total_division_count]
  /- Cancel the common nonzero factor `2^d` from both sides and from
     the modulus. What remains is precisely the exact tail congruence. -/
  apply Nat.ModEq.mul_left_cancel'
    (by positivity : 2 ^ d ≠ 0)
  simpa [hD, pow_add, add_assoc] using h

/--
A positive division count forces the starting value to be odd.

If

`1 ≤ division_count x`,

then the `2`-adic valuation of `3 * x + 1` is positive, so

`2 ∣ 3 * x + 1`.

Thus `3 * x + 1` is even. Since `1` is odd, `3 * x` must be odd, and
because `3` itself is odd, `x` is odd.

This lemma is used in the reverse realizability proof after the exact head
valuation has been recovered. Admissibility gives positivity of that
valuation, which then implies oddness of the current starting value.
-/
lemma odd_of_division_count_pos
    (x : ℕ)
    (hpos : 1 ≤ division_count x) :
    Odd x := by
  unfold division_count at hpos
  /- Positive `2`-adic valuation means that `2` divides `3*x + 1`. -/
  have hdiv :
      (2 : ℕ) ∣ 3 * x + 1 := by
    exact
      (padicValNat_dvd_iff_le
        (p := 2)
        (a := 3 * x + 1)
        (n := 1)
        (by omega)).mpr hpos
  /- Hence `3*x + 1` is even. -/
  have heven :
      Even (3 * x + 1) := by
    exact even_iff_two_dvd.mpr hdiv
  /- Since `1` is odd, evenness of `3*x + 1` forces `3*x` to be odd. -/
  have h3x :
      Odd (3 * x) := by
    exact (Nat.even_add'.mp heven).mpr odd_one
  /- Since `3` is odd, `x` must be odd. -/
  exact (Nat.odd_mul.mp h3x).2

/--
The global exact affine congruence, together with positivity of the prescribed
division counts, forces both oddness of the starting value and exact realization
of the entire division word.

Assume every entry of `ω` is positive and

`3^|ω| * x + c(ω)
    ≡ 2^(D(ω)) mod 2^(D(ω)+1)`.

The proof proceeds by induction on `ω`.

For the empty word, the congruence reduces to

`x ≡ 1 mod 2`,

so `x` is odd, while realization of the empty word is structural.

For a nonempty word `d :: ω`, the exact congruence first implies that

`2^d ∣ 3 * x + 1`.

Thus

`3 * x + 1 = 2^d * y`

for some natural number `y`. The exact congruence then transports to the
tail word `ω` starting at `y`. By the induction hypothesis, `y` is odd and
realizes the tail.

Since the quotient `y` is odd, the factorization

`3 * x + 1 = 2^d * y`

shows that `d` is not merely a legal division exponent but the exact
`2`-adic valuation of `3 * x + 1`. Hence the symbolic head step agrees
with the actual fully accelerated Collatz step. Positivity of `d` also
forces `x` itself to be odd.

The exact head valuation and the realized tail can then be reassembled to
show that `x` realizes the full word.

This theorem is the reverse implication underlying the exact realization
criterion: the single global congruence modulo `2^(D(ω)+1)` contains enough
information to recover every prescribed valuation in the word.
-/
theorem exact_congruence_implies_odd_and_realizes
    {x : ℕ} {ω : division_word}
    (hpos : ∀ d ∈ ω, 1 ≤ d)
    (h :
      Nat.ModEq
        (2 ^ (total_division_count ω + 1))
        (3 ^ ω.length * x + affine_constant ω)
        (2 ^ total_division_count ω)) :
    Odd x ∧ realizes x ω := by
  induction ω generalizing x with
  | nil =>
      /- For the empty word, the exact congruence reduces to
           x ≡ 1 [MOD 2],
         so `x` is odd. Realization of the empty word is structural. -/
      have hxmod :
          Nat.ModEq
            (2 ^ (0 + 1))
            x
            (2 ^ 0) := by
        simpa [total_division_count, affine_constant] using h
      rcases
          (modEq_pow_succ_iff_eq_pow_mul_odd x 0).mp hxmod
        with ⟨q, hqodd, hxq⟩
      have hxq' : x = q := by
        simpa using hxq
      constructor
      · rw [hxq']
        exact hqodd
      · simp [realizes, division_counts]
  | cons d ω ih =>
      /- Positivity of the word separates into positivity of the head
         exponent and positivity of every exponent in the tail. -/
      have hdpos : 1 ≤ d := by
        exact hpos d (by simp)
      have htailpos :
          ∀ e ∈ ω, 1 ≤ e := by
        intro e he
        exact hpos e (by simp [he])
      /- The global exact congruence forces the first prescribed
         division by `2^d` to be integral. -/
      have hdiv :
          2 ^ d ∣ 3 * x + 1 := by
        exact affine_exact_congruence_head_dvd h
      /- Introduce the natural-number state obtained after that prescribed
         symbolic division. -/
      rcases hdiv with ⟨y, hy⟩
      /- After peeling off the first legal symbolic step, the same exact
         congruence reproduces itself on the tail. -/
      have htail :
          Nat.ModEq
            (2 ^ (total_division_count ω + 1))
            (3 ^ ω.length * y + affine_constant ω)
            (2 ^ total_division_count ω) := by
        exact affine_exact_congruence_tail hy h
      /- Apply the induction hypothesis to the tail. It tells us both
         that the next state `y` is odd and that it realizes `ω`. -/
      have ihtail :
          Odd y ∧ realizes y ω := by
        exact ih htailpos htail
      rcases ihtail with ⟨hyodd, hyrealizes⟩
      /- Since
           3*x + 1 = 2^d * y
         and `y` is odd, `d` is the exact 2-adic valuation. -/
      have hval :
          padicValNat 2 (3 * x + 1) = d := by
        exact
          valuation_exponent_of_odd_factor
            hy
            hyodd
            (by omega)
      have hdcount :
          d = division_count x := by
        unfold division_count
        exact hval.symm
      /- Therefore the symbolic quotient `y` is exactly the actual
         fully accelerated successor of `x`. -/
      have hyfa :
          fully_accelerated x = y := by
        unfold fully_accelerated
        rw [← hdcount]
        rw [hy]
        simp
      /- Since the exact head count is positive, the starting state is
         itself odd. -/
      have hxodd : Odd x := by
        apply odd_of_division_count_pos x
        rw [← hdcount]
        exact hdpos
      constructor
      · exact hxodd
      /- Reassemble realization of the whole word from the exact head
         division count and realization of the tail from the next
         accelerated state. -/
      · unfold realizes
        simp only [
          division_counts,
          List.length_cons,
          List.iterate,
          List.map_cons,
          List.cons.injEq
        ]
        constructor
        · exact hdcount
        · rw [hyfa]
          unfold realizes at hyrealizes
          unfold division_counts at hyrealizes
          exact hyrealizes

/--
Exact realization criterion for admissible division words.

For an admissible word `ω`, a natural number `x` realizes `ω` if and only if

`3^|ω| * x + c(ω)
    ≡ 2^(D(ω)) mod 2^(D(ω)+1)`.

The forward implication is `realizes_modEq_succ`: realization of a nonempty
word forces the affine numerator to be `2^(D(ω))` times an odd endpoint.

The reverse implication is `exact_congruence_implies_odd_and_realizes`:
positivity of every prescribed division count together with the exact global
congruence recursively recovers the exact head valuation and realization of
the tail.

Thus, for admissible words, the single congruence modulo `2^(D(ω)+1)` is
equivalent to the full sequence of exact valuation constraints defining
realization.
-/
theorem realizes_iff_exact_congruence
    {x : ℕ} {ω : division_word}
    (hω : admissible_division_word ω) :
    realizes x ω ↔
      Nat.ModEq
        (2 ^ (total_division_count ω + 1))
        (3 ^ ω.length * x + affine_constant ω)
        (2 ^ total_division_count ω) := by
  constructor
  /- Forward direction: admissibility supplies nonemptiness, so realization
     gives the exact congruence modulo `2^(D(ω)+1)`. -/
  · intro hrealizes
    rcases hω with ⟨hvalid, _⟩
    exact realizes_modEq_succ hrealizes hvalid
  /- Reverse direction: admissibility supplies positivity of every
     prescribed division count, allowing the global exact congruence to
     reconstruct the full realized trajectory. -/
  · intro hcong
    rcases hω with ⟨_, hpos⟩
    exact
      (exact_congruence_implies_odd_and_realizes
        hpos
        hcong).2

/--
Characterizes an exact local division count by a dyadic congruence.

For any natural numbers `x` and `d`,

`division_count x = d`

if and only if

`3 * x + 1 ≡ 2^d mod 2^(d+1)`.

Equivalently, the numerator `3 * x + 1` has the exact factorization

`3 * x + 1 = 2^d * q`

with `q` odd.

The forward direction factors `3 * x + 1` into its maximal power of `2`
times an odd part and identifies that exponent with `division_count x`.
The reverse direction reads the exact congruence as precisely such a
factorization and recovers the `2`-adic valuation.

This is the local exactness principle used throughout the realizability
theory: divisibility by `2^d` alone is insufficient, while congruence
modulo `2^(d+1)` records that no additional factor of `2` remains.
-/
lemma division_count_eq_iff_exact_modEq
    (x d : ℕ) :
    division_count x = d ↔
      Nat.ModEq
        (2 ^ (d + 1))
        (3 * x + 1)
        (2 ^ d) := by
  constructor
  · intro hcount
    /- Factor `3*x + 1` into its exact power of `2` times an odd part. -/
    have hn0 : 3 * x + 1 ≠ 0 := by
      omega
    obtain ⟨k, q, hqodd, hn⟩ :=
      Nat.exists_eq_two_pow_mul_odd hn0
    /- The exponent appearing in this factorization is exactly the
       `2`-adic valuation. -/
    have hval :
        padicValNat 2 (3 * x + 1) = k := by
      exact
        valuation_exponent_of_odd_factor
          hn
          hqodd
          hn0
    /- Since `division_count x = d`, the factorization exponent is `d`. -/
    have hk : k = d := by
      unfold division_count at hcount
      omega
    /- Therefore `3*x + 1 = 2^d * q` with `q` odd, which is equivalent
       to the exact congruence modulo `2^(d+1)`. -/
    apply
      (modEq_pow_succ_iff_eq_pow_mul_odd
        (3 * x + 1)
        d).mpr
    refine ⟨q, hqodd, ?_⟩
    simpa [hk] using hn
  · intro hmod
    /- The exact congruence says that `3*x + 1` is `2^d` times an
       odd natural number. -/
    rcases
        (modEq_pow_succ_iff_eq_pow_mul_odd
          (3 * x + 1)
          d).mp hmod
      with ⟨q, hqodd, hn⟩
    /- Hence the exact `2`-adic valuation is `d`. -/
    unfold division_count
    exact
      valuation_exponent_of_odd_factor
        hn
        hqodd
        (by omega)

/--
The ternary coefficient in the affine numerator is coprime to the exact
dyadic realization modulus.

Since `3` and `2` are coprime, any powers of them are also coprime. Hence

`3^|ω|`

is coprime to

`2^(D(ω)+1)`.

This is the arithmetic fact that makes the affine realization congruence
a uniquely solvable linear congruence in `x`.
-/
lemma ternary_coprime_exact_modulus
    (ω : division_word) :
    Nat.Coprime
      (3 ^ ω.length)
      (2 ^ (total_division_count ω + 1)) := by
  exact
    (by norm_num : Nat.Coprime 3 2).pow
      ω.length
      (total_division_count ω + 1)

/--
Any two realizations of the same admissible division word are congruent
modulo the exact realization modulus.

If `x` and `y` both realize `ω`, then

`x ≡ y mod 2^(D(ω)+1)`.

Both realizations satisfy the same exact affine congruence

`3^|ω| * z + c(ω)
    ≡ 2^(D(ω)) mod 2^(D(ω)+1)`.

Therefore their affine numerators are congruent to each other. Cancelling
the common affine constant leaves

`3^|ω| * x ≡ 3^|ω| * y`.

Since `3^|ω|` is coprime to the power-of-two modulus, it can be cancelled,
yielding congruence of the starting values themselves.

This establishes the uniqueness side of the realization residue structure:
all realizations of an admissible word lie in one residue class modulo
`2^(D(ω)+1)`.
-/
theorem realizations_modEq
    {ω : division_word}
    (hω : admissible_division_word ω)
    {x y : ℕ}
    (hx : realizes x ω)
    (hy : realizes y ω) :
    Nat.ModEq
      (2 ^ (total_division_count ω + 1))
      x y := by
  /- Both realizations satisfy the exact affine congruence associated
     with the same division word. -/
  have hxc :=
    (realizes_iff_exact_congruence hω).mp hx
  have hyc :=
    (realizes_iff_exact_congruence hω).mp hy
  /- Since both affine numerators are congruent to the same value
     `2^D`, they are congruent to each other. -/
  have haff :
      Nat.ModEq
        (2 ^ (total_division_count ω + 1))
        (3 ^ ω.length * x + affine_constant ω)
        (3 ^ ω.length * y + affine_constant ω) := by
    exact hxc.trans hyc.symm
  /- Cancel the common affine constant. -/
  have hmul :
      Nat.ModEq
        (2 ^ (total_division_count ω + 1))
        (3 ^ ω.length * x)
        (3 ^ ω.length * y) := by
    exact
      Nat.ModEq.add_right_cancel'
        (affine_constant ω)
        haff
  /- The ternary coefficient is coprime to the power-of-two modulus. -/
  have hcop :
      Nat.Coprime
        (2 ^ (total_division_count ω + 1))
        (3 ^ ω.length) := by
    exact (ternary_coprime_exact_modulus ω).symm
  /- Therefore the common factor `3^|ω|` can be cancelled from the
     congruence, leaving the two starting values congruent modulo the
     exact realization modulus. -/
  exact
    Nat.ModEq.cancel_left_of_coprime
      hcop
      hmul

/--
Solves a linear congruence with invertible coefficient as a unique residue
class.

Let `A`, `B`, `C`, and `M` be natural numbers with `M > 0` and
`Nat.Coprime A M`. Then there exists a canonical representative `r < M`
such that, for every natural number `x`,

`A * x + C ≡ B [MOD M]`

if and only if

`x ≡ r [MOD M]`.

The proof works in the finite ring `ZMod M`. Coprimality of `A` and `M`
makes `A` a unit, so the linear equation

`A * x + C = B`

has the formal solution

`x = A⁻¹ * (B - C)`.

The canonical natural representative of this element of `ZMod M` is taken
as `r`. The remainder of the proof shows that natural-number congruence is
equivalent to equality with this element in `ZMod M`, in both directions.

This lemma is intentionally generic. In the realizability development it is
applied with

`A = 3^|ω|`,
`B = 2^(D(ω))`,
`C = c(ω)`, and
`M = 2^(D(ω)+1)`,

thereby converting the exact affine realization congruence into a single
canonical residue class of starting values.
-/
lemma coprime_linear_modEq_residue
    (A B C M : ℕ)
    (hM : 0 < M)
    (hcop : Nat.Coprime A M) :
    ∃ r : ℕ,
      r < M ∧
      ∀ x : ℕ,
        Nat.ModEq M (A * x + C) B ↔
          Nat.ModEq M x r := by
  /- Work in the finite ring `ZMod M`. Since `M > 0`, every element
     has a canonical natural representative strictly below `M`. -/
  let : NeZero M := ⟨Nat.ne_of_gt hM⟩
  /- Since `A` is coprime to `M`, it is invertible modulo `M`.
     Solve
         A*x + C = B
     formally as
         x = A⁻¹ * (B - C). -/
  let z : ZMod M :=
    (A : ZMod M)⁻¹ *
      ((B : ZMod M) - (C : ZMod M))
  /- Choose the canonical natural representative of that residue. -/
  let r : ℕ := z.val
  refine ⟨r, ?_, ?_⟩
  · exact ZMod.val_lt z
  · intro x
    /- Translate natural-number congruences into equalities in `ZMod M`. -/
    rw [← ZMod.natCast_eq_natCast_iff]
    rw [← ZMod.natCast_eq_natCast_iff]
    /- Push the cast through the natural-number addition and
       multiplication in the affine expression. -/
    push_cast
    /- The canonical representative `r = z.val` represents exactly `z`
       in `ZMod M`. -/
    have hrz : (r : ZMod M) = z := by
      dsimp [r]
      exact ZMod.natCast_zmod_val z
    rw [hrz]
    change
      (A : ZMod M) * (x : ZMod M) + (C : ZMod M) =
          (B : ZMod M)
        ↔
      (x : ZMod M) = z
    /- Coprimality makes `A` invertible in `ZMod M`, so multiplication
       by `A` can be cancelled. -/
    have hunit :
        IsUnit (A : ZMod M) := by
      exact (ZMod.isUnit_iff_coprime A M).2 hcop
    /- Conversely, substitute the canonical solution and use
       `A * A⁻¹ = 1` to recover the original linear congruence. -/
    constructor
    · intro hx
      have hAx :
          (A : ZMod M) * (x : ZMod M) =
            (B : ZMod M) - (C : ZMod M) := by
        exact eq_sub_of_add_eq hx
      calc
        (x : ZMod M)
            = 1 * x := by simp
        _ =
            ((A : ZMod M)⁻¹ * (A : ZMod M)) * x := by
              rw [ZMod.inv_mul_of_unit _ hunit]
        _ =
            (A : ZMod M)⁻¹ *
              ((A : ZMod M) * x) := by
              ring
        _ =
            (A : ZMod M)⁻¹ *
              ((B : ZMod M) - (C : ZMod M)) := by
              rw [hAx]
        _ = z := by
              rfl
    · intro hx
      rw [hx]
      dsimp [z]
      calc
        (A : ZMod M) *
              ((A : ZMod M)⁻¹ *
                ((B : ZMod M) - (C : ZMod M))) +
              (C : ZMod M)
            =
          ((A : ZMod M) * (A : ZMod M)⁻¹) *
              ((B : ZMod M) - (C : ZMod M)) +
              (C : ZMod M) := by
                ring
        _ =
          1 * ((B : ZMod M) - (C : ZMod M)) +
              (C : ZMod M) := by
                rw [ZMod.mul_inv_of_unit _ hunit]
        _ = (B : ZMod M) := by
              ring

/--
The exact affine congruence associated with any division word determines a
single canonical residue class of starting values.

For every division word `ω`, there exists a representative

`r < 2^(D(ω)+1)`

such that, for every natural number `x`,

`3^|ω| * x + c(ω)
    ≡ 2^(D(ω)) mod 2^(D(ω)+1)`

if and only if

`x ≡ r mod 2^(D(ω)+1)`.

The proof is an immediate specialization of
`coprime_linear_modEq_residue` with

`A = 3^|ω|`,
`B = 2^(D(ω))`,
`C = c(ω)`, and
`M = 2^(D(ω)+1)`.

The modulus is positive, and `3^|ω|` is coprime to it by
`ternary_coprime_exact_modulus`, so the linear congruence has a canonical
solution modulo the exact dyadic modulus.

This result is purely arithmetic and does not require admissibility.
Admissibility enters later, when the exact affine congruence is identified
with actual realization of the division word.
-/
lemma affine_exact_residue_exists
    (ω : division_word) :
    ∃ r : ℕ,
      r < 2 ^ (total_division_count ω + 1) ∧
      ∀ x : ℕ,
        Nat.ModEq
          (2 ^ (total_division_count ω + 1))
          (3 ^ ω.length * x + affine_constant ω)
          (2 ^ total_division_count ω)
        ↔
        Nat.ModEq
          (2 ^ (total_division_count ω + 1))
          x r := by
  exact
    coprime_linear_modEq_residue
      (3 ^ ω.length)
      (2 ^ total_division_count ω)
      (affine_constant ω)
      (2 ^ (total_division_count ω + 1))
      (by positivity)
      (ternary_coprime_exact_modulus ω)

/--
Every admissible division word has a unique canonical exact-realization
residue.

For an admissible word `ω`, there exists a unique natural number

`r < 2^(D(ω)+1)`

such that, for every natural number `x`,

`realizes x ω`

if and only if

`x ≡ r mod 2^(D(ω)+1)`.

Existence comes from `affine_exact_residue_exists`, which gives a canonical
residue representative for the exact affine congruence. Admissibility then
allows `realizes_iff_exact_congruence` to identify that affine congruence
with actual realization of `ω`.

For uniqueness, suppose another representative `s` below the same modulus
describes the same realization class. The canonical representative `r`
is congruent to itself, so it realizes `ω`; therefore it must also satisfy
the class condition for `s`, giving

`r ≡ s mod 2^(D(ω)+1)`.

Since both representatives lie strictly below the modulus, this congruence
reduces to literal equality.

This theorem establishes the unique canonical residue class associated with
an admissible division word.
-/
theorem realization_residue_exists_unique
    {ω : division_word}
    (hω : admissible_division_word ω) :
    ∃! r : ℕ,
      r < 2 ^ (total_division_count ω + 1) ∧
      ∀ x : ℕ,
        realizes x ω ↔
          Nat.ModEq
            (2 ^ (total_division_count ω + 1))
            x r := by
  /- The exact affine congruence has a canonical residue representative
     below the modulus. -/
  rcases affine_exact_residue_exists ω with
    ⟨r, hrlt, hraff⟩
  /- Convert the affine congruence class into the realization class using
     the exact-realization criterion for admissible words. -/
  have hrclass :
      ∀ x : ℕ,
        realizes x ω ↔
          Nat.ModEq
            (2 ^ (total_division_count ω + 1))
            x r := by
    intro x
    exact
      (realizes_iff_exact_congruence hω).trans
        (hraff x)
  refine ⟨r, ?_, ?_⟩
  /- `r` has the required bound and characterizes exactly the realizations
     of `ω`. -/
  · exact ⟨hrlt, hrclass⟩
  /- Any other bounded representative describing the same realization
     class must be equal to `r`. -/
  · intro s hs
    rcases hs with ⟨hslt, hsclass⟩
    /- The canonical representative `r` belongs to its own residue class,
       so the characterization above shows that `r` realizes `ω`. -/
    have hrrealizes :
        realizes r ω := by
      apply (hrclass r).mpr
      exact Nat.ModEq.refl r
    /- Since `s` is assumed to describe the same realization set, the
       realization `r` must also lie in the residue class represented
       by `s`. -/
    have hrs :
        Nat.ModEq
          (2 ^ (total_division_count ω + 1))
          r s := by
      exact (hsclass r).mp hrrealizes
    /- Congruent canonical representatives below the same modulus are
       equal as natural numbers. -/
    change
      r % (2 ^ (total_division_count ω + 1)) =
        s % (2 ^ (total_division_count ω + 1))
      at hrs
    rw [
      Nat.mod_eq_of_lt hrlt,
      Nat.mod_eq_of_lt hslt
    ] at hrs
    /- `∃!` asks for `s = r`, while the congruence reduction gives
       `r = s`. -/
    exact hrs.symm

/--
Canonical realization residues are compatible with word extension.

Suppose `rω` represents exactly the realizations of `ω`, and `rωη`
represents exactly the realizations of the extended word `ω ++ η`.
Then

`rωη ≡ rω mod 2^(D(ω)+1)`.

The proof uses the canonical representative `rωη` itself. Since it is
congruent to itself modulo the exact modulus for `ω ++ η`, it realizes
the full word `ω ++ η`. Realization of a concatenated word implies
realization of its left prefix `ω`. The residue characterization for `ω`
then places `rωη` in the class represented by `rω`.

Thus the canonical residue class of an extended word lies inside the
canonical residue class of its prefix.
-/
theorem realization_residue_append_left
    {ω η : division_word}
    {rω rωη : ℕ}
    (hrω :
      rω < 2 ^ (total_division_count ω + 1) ∧
      ∀ x : ℕ,
        realizes x ω ↔
          Nat.ModEq
            (2 ^ (total_division_count ω + 1))
            x rω)
    (hrωη :
      rωη < 2 ^ (total_division_count (ω ++ η) + 1) ∧
      ∀ x : ℕ,
        realizes x (ω ++ η) ↔
          Nat.ModEq
            (2 ^ (total_division_count (ω ++ η) + 1))
            x rωη) :
    Nat.ModEq
      (2 ^ (total_division_count ω + 1))
      rωη
      rω := by
  /- Extract the residue-class characterizations; the representative
     bounds are not needed for this nesting argument. -/
  rcases hrω with ⟨_, hrω_class⟩
  rcases hrωη with ⟨_, hrωη_class⟩
  /- The canonical representative of the longer word belongs to its own
     residue class, hence realizes the full extension `ω ++ η`. -/
  have hrωη_realizes :
      realizes rωη (ω ++ η) := by
    apply (hrωη_class rωη).mpr
    exact Nat.ModEq.refl rωη
  /- Any realization of `ω ++ η` also realizes the left prefix `ω`. -/
  have hrωη_prefix :
      realizes rωη ω := by
    exact realizes_append_left hrωη_realizes
  /- Therefore `rωη` lies in the canonical residue class for `ω`. -/
  exact (hrω_class rωη).mp hrωη_prefix

/--
Realization is inherited by every finite prefix.

If `x` realizes a division word `ω`, then for any natural number `j`,
the same starting value `x` realizes the prefix `ω.take j`.

The proof uses the decomposition

`ω = ω.take j ++ ω.drop j`

and then applies `realizes_append_left` to forget the suffix.

This is the basic prefix-closure property of realization.
-/
lemma realizes_take
    {x : ℕ} {ω : division_word}
    (h : realizes x ω)
    (j : ℕ) :
    realizes x (ω.take j) := by
  /- Split the word into its first `j` entries and the remaining suffix. -/
  rw [← List.take_append_drop j ω] at h
  /- A realization of the concatenated word realizes its left prefix. -/
  exact realizes_append_left h

/--
Canonical realization residues are compatible with arbitrary prefixes.

Suppose `rω` represents exactly the realizations of the full word `ω`,
and `rj` represents exactly the realizations of the prefix `ω.take j`.
Then

`rω ≡ rj mod 2^(D(ω.take j)+1)`.

The proof uses the full-word representative `rω` itself. Since `rω` belongs
to its own residue class, it realizes `ω`. Realization is inherited by every
prefix, so `rω` also realizes `ω.take j`. The residue characterization of
that prefix then places `rω` in the class represented by `rj`.

Thus the canonical residue of a full word reduces to the canonical residue
of every finite prefix.
-/
theorem realization_residue_take
    {ω : division_word}
    {j rj rω : ℕ}
    (hrj :
      rj < 2 ^ (total_division_count (ω.take j) + 1) ∧
      ∀ x : ℕ,
        realizes x (ω.take j) ↔
          Nat.ModEq
            (2 ^ (total_division_count (ω.take j) + 1))
            x rj)
    (hrω :
      rω < 2 ^ (total_division_count ω + 1) ∧
      ∀ x : ℕ,
        realizes x ω ↔
          Nat.ModEq
            (2 ^ (total_division_count ω + 1))
            x rω) :
    Nat.ModEq
      (2 ^ (total_division_count (ω.take j) + 1))
      rω
      rj := by
  /- Extract the residue-class characterizations. The representative
     bounds are not needed for this prefix-nesting argument. -/
  rcases hrj with ⟨_, hrj_class⟩
  rcases hrω with ⟨_, hrω_class⟩
  /- The canonical representative of the full word realizes the full word. -/
  have hrω_realizes :
      realizes rω ω := by
    apply (hrω_class rω).mpr
    exact Nat.ModEq.refl rω
  /- Hence it also realizes the first `j` entries. -/
  have hrω_prefix :
      realizes rω (ω.take j) := by
    exact realizes_take hrω_realizes j
  /- Therefore it lies in the canonical residue class of that prefix. -/
  exact (hrj_class rω).mp hrω_prefix

/--
Positive-length prefixes of an admissible division word are admissible.

If `ω` is admissible and

`0 < j ≤ ω.length`,

then the prefix `ω.take j` is also admissible.

Admissibility has two parts. First, the prefix is nonempty because its
length is exactly `j`, and `j > 0`. Second, every entry of the prefix is
also an entry of `ω`, so positivity of all division counts is inherited
from the original word.

This lemma allows admissibility-based realization and residue theorems to
be applied uniformly to nonempty prefixes.
-/
lemma admissible_division_word_take
    {ω : division_word}
    (hω : admissible_division_word ω)
    {j : ℕ}
    (hjpos : 0 < j)
    (hjle : j ≤ ω.length) :
    admissible_division_word (ω.take j) := by
  /- Extract positivity of every entry of the original admissible word. -/
  rcases hω with ⟨_, hpos⟩
  constructor
  /- Since `j ≤ ω.length`, the prefix has length exactly `j`; hence
     `j > 0` makes it a valid, nonempty division word. -/
  · unfold valid_division_word
    rw [List.length_take, Nat.min_eq_left hjle]
    exact hjpos
  /- Every entry of the prefix is an entry of the original word, so its
     positivity follows from admissibility of `ω`. -/
  · intro d hd
    apply hpos d
    exact List.mem_of_mem_take hd

/--
`realization_residue ω r` means that `r` is the canonical exact-realization
residue for the division word `ω`.

Concretely, `r` is required to lie in the canonical range

`r < 2^(D(ω)+1)`,

and its residue class modulo `2^(D(ω)+1)` must consist exactly of the
natural numbers that realize `ω`.

Thus `realization_residue ω r` packages both the canonical representative
condition and the exact characterization of the realization class.
-/
def realization_residue
    (ω : division_word) (r : ℕ) : Prop :=
  r < 2 ^ (total_division_count ω + 1) ∧
  ∀ x : ℕ,
    realizes x ω ↔
      Nat.ModEq
        (2 ^ (total_division_count ω + 1))
        x r

/--
Every admissible division word has a unique canonical realization residue,
expressed using the packaged predicate `realization_residue`.
-/
theorem realization_residue_exists_unique'
    {ω : division_word}
    (hω : admissible_division_word ω) :
    ∃! r : ℕ, realization_residue ω r := by
  exact realization_residue_exists_unique hω

/--
Residue lifting along the positive prefixes of an admissible division word.

Let `ω` be admissible. Then:

1. every positive prefix `ω.take j`, with `0 < j ≤ ω.length`, has a unique
   canonical exact-realization residue;

2. if `rj` and `rj1` are the canonical realization residues of consecutive
   prefixes `ω.take j` and `ω.take (j + 1)`, then

   `rj1 ≡ rj mod 2^(D(ω.take j)+1)`.

Thus the exact realization classes form a compatible nested system as the
prefix length increases.

The first statement follows because positive prefixes of an admissible word
are themselves admissible, so each has a unique canonical realization
residue.

For the second statement, the representative `rj1` realizes the longer
prefix `ω.take (j + 1)`. Realization is inherited by prefixes, so `rj1`
also realizes `ω.take j`. The residue characterization of the shorter
prefix therefore places `rj1` in the residue class represented by `rj`.

This theorem gives the prefix-by-prefix residue structure needed for the
subsequent nested realization-class development.
-/
theorem residue_lifting_theorem
    {ω : division_word}
    (hω : admissible_division_word ω) :
    (∀ j : ℕ,
      0 < j →
      j ≤ ω.length →
      ∃! r : ℕ,
        realization_residue (ω.take j) r)
    ∧
    (∀ j : ℕ,
      0 < j →
      j + 1 ≤ ω.length →
      ∀ rj rj1 : ℕ,
        realization_residue (ω.take j) rj →
        realization_residue (ω.take (j + 1)) rj1 →
        Nat.ModEq
          (2 ^ (total_division_count (ω.take j) + 1))
          rj1
          rj) := by
  constructor
  /- Every positive prefix of an admissible word is itself admissible,
     and therefore has a unique canonical exact-realization residue. -/
  · intro j hjpos hjle
    have hprefix :
        admissible_division_word (ω.take j) := by
      exact
        admissible_division_word_take
          hω hjpos hjle
    exact
      realization_residue_exists_unique'
        hprefix
  /- The canonical residue of an extended prefix lies in the realization
     class of the shorter prefix. Hence the two representatives are
     congruent modulo the shorter prefix's exact modulus. -/
  · intro j hjpos hj1le rj rj1 hrj hrj1
    rcases hrj with
      ⟨_, hrj_class⟩
    rcases hrj1 with
      ⟨_, hrj1_class⟩
    /- The longer-prefix representative belongs to its own class, so it
       realizes the longer prefix. -/
    have hrj1_realizes :
        realizes rj1 (ω.take (j + 1)) := by
      apply (hrj1_class rj1).mpr
      exact Nat.ModEq.refl rj1
    /- A realization of the longer prefix also realizes the shorter one. -/
    have hrj1_prefix :
        realizes rj1 (ω.take j) := by
      have htake :
          realizes rj1 ((ω.take (j + 1)).take j) := by
        exact realizes_take hrj1_realizes j
      simpa [List.take_take, Nat.min_eq_left (by omega)] using htake
    /- Therefore the longer representative lies in the shorter canonical
       residue class. -/
    exact
      (hrj_class rj1).mp hrj1_prefix

/--
Every admissible division word is realizable by some natural number.

If `ω` is admissible, then there exists `x : ℕ` such that

`realizes x ω`.

The proof uses the unique canonical realization residue `r` associated with
`ω`. Since `r` is congruent to itself modulo the exact realization modulus,
the residue characterization immediately implies that `r` realizes `ω`.

Thus admissibility is not merely a formal positivity condition: every
admissible finite division word actually occurs along some fully accelerated
Collatz trajectory.
-/
theorem admissible_division_word_realizable
    {ω : division_word}
    (hω : admissible_division_word ω) :
    ∃ x : ℕ, realizes x ω := by
  /- Obtain the unique canonical realization residue for `ω`. -/
  rcases realization_residue_exists_unique' hω with
    ⟨r, hr, _⟩
  rcases hr with ⟨_, hrclass⟩
  /- The canonical representative belongs to its own residue class,
     so it realizes the word. -/
  refine ⟨r, ?_⟩
  exact
    (hrclass r).mpr
      (Nat.ModEq.refl r)

/--
A starting value realizes at most one division word of any given length.

If `x` realizes both `ω` and `η`, and

`ω.length = η.length`,

then

`ω = η`.

Both words are equal to the same generated division-count prefix

`division_counts x ω.length`.

Thus realization is uniquely determined by the starting value together
with the word length.
-/
lemma realizes_eq_of_length_eq
    {x : ℕ} {ω η : division_word}
    (hω : realizes x ω)
    (hη : realizes x η)
    (hlen : ω.length = η.length) :
    ω = η := by
  calc
    ω = division_counts x ω.length := hω
    _ = division_counts x η.length := by rw [hlen]
    _ = η := hη.symm

/--
If a starting value realizes two division words and one is no longer than
the other, then the shorter word is exactly the corresponding prefix of
the longer word.

Suppose `x` realizes both `ω` and `η`, with

`ω.length ≤ η.length`.

Since realization is inherited by prefixes, `x` also realizes

`η.take ω.length`.

This prefix has the same length as `ω`. By uniqueness of the realized word
at a fixed length, the two must be equal:

`ω = η.take ω.length`.

Thus two realized words arising from the same starting value cannot disagree
before the shorter word ends.
-/
lemma realizes_prefix_of_length_le
    {x : ℕ} {ω η : division_word}
    (hω : realizes x ω)
    (hη : realizes x η)
    (hlen : ω.length ≤ η.length) :
    ω = η.take ω.length := by
  /- A realization of the longer word also realizes its prefix of the
     shorter word's length. -/
  have htake :
      realizes x (η.take ω.length) := by
    exact realizes_take hη ω.length
  /- Because `ω.length ≤ η.length`, that prefix has length exactly
     `ω.length`. -/
  have htake_length :
      (η.take ω.length).length = ω.length := by
    simp [List.length_take, Nat.min_eq_left hlen]
  /- Two words of the same length realized by the same starting value
     must be identical. -/
  exact
    realizes_eq_of_length_eq
      hω
      htake
      htake_length.symm

/--
Two division words realized by the same starting value are prefix-comparable.

If `x` realizes both `ω` and `η`, then either

`ω = η.take ω.length`

or

`η = ω.take η.length`.

Thus one realized word must be an initial segment of the other; two distinct
realized words arising from the same starting value cannot branch apart and
then later rejoin.

The proof compares the two word lengths. Whichever word is no longer than
the other is forced, by `realizes_prefix_of_length_le`, to be exactly the
corresponding prefix of the longer word.
-/
theorem common_realization_implies_prefix
    {x : ℕ} {ω η : division_word}
    (hω : realizes x ω)
    (hη : realizes x η) :
    ω = η.take ω.length ∨
      η = ω.take η.length := by
  /- One of the two word lengths is at most the other. -/
  rcases le_total ω.length η.length with hle | hle
  /- If `ω` is no longer than `η`, then `ω` is the corresponding
     prefix of `η`. -/
  · exact Or.inl
      (realizes_prefix_of_length_le hω hη hle)
  /- Otherwise `η` is the corresponding prefix of `ω`. -/
  · exact Or.inr
      (realizes_prefix_of_length_le hη hω hle)

/--
Realization classes of division words are either nested or disjoint.

For any two division words `ω` and `η`, one of the following
structural possibilities holds:

* every realization of `η` also realizes `ω`;
* every realization of `ω` also realizes `η`;
* no natural number realizes both words.

If the two realization classes have a common element, then
`common_realization_implies_prefix` shows that the corresponding words are
prefix-comparable. Realization of the longer word therefore implies
realization of the shorter prefix, giving one of the two nesting relations.

If no common realization exists, the two realization classes are disjoint.

Thus realization classes cannot partially overlap: any nonempty
intersection forces one class to be contained in the other.
-/
theorem realization_classes_nested_or_disjoint
    {ω η : division_word} :
    (∀ x : ℕ, realizes x η → realizes x ω) ∨
    (∀ x : ℕ, realizes x ω → realizes x η) ∨
    (∀ x : ℕ, ¬ (realizes x ω ∧ realizes x η)) := by
  /- Split according to whether the two words have any common realization. -/
  by_cases hcommon :
      ∃ x : ℕ, realizes x ω ∧ realizes x η
  · rcases hcommon with ⟨x, hxω, hxη⟩
    /- A common realization forces the two words to be prefix-comparable. -/
    rcases
        common_realization_implies_prefix hxω hxη
      with hprefix | hprefix
    /- If `ω` is a prefix of `η`, every realization of `η`
       also realizes `ω`. -/
    · left
      intro y hy
      rw [hprefix]
      exact realizes_take hy ω.length
    /- If `η` is a prefix of `ω`, every realization of `ω`
       also realizes `η`. -/
    · right
      left
      intro y hy
      rw [hprefix]
      exact realizes_take hy η.length
  /- If there is no common realization, the two classes are disjoint. -/
  · right
    right
    intro x hx
    exact hcommon ⟨x, hx.1, hx.2⟩

/--
Exact realization residue classes are either nested or disjoint.

Suppose `rω` and `rη` are canonical realization residues for the division
words `ω` and `η`. Then one of the following structural relations holds:

* every natural number in the exact residue class of `η` also lies in the
  exact residue class of `ω`;
* every natural number in the exact residue class of `ω` also lies in the
  exact residue class of `η`;
* the two exact residue classes have no common natural number.

The proof transfers the nested-or-disjoint theorem for realization classes
through the residue characterizations supplied by `hrω` and `hrη`.

Thus the prefix structure of realized division words is reflected directly
in the arithmetic structure of their exact dyadic residue classes: two such
classes cannot partially overlap.

This theorem is the arithmetic endpoint of the realizability development
and provides the natural handoff to the later nested residue-class and
cylinder structure.
-/
theorem exact_residue_classes_nested_or_disjoint
    {ω η : division_word}
    {rω rη : ℕ}
    (hrω : realization_residue ω rω)
    (hrη : realization_residue η rη) :
    (∀ x : ℕ,
      Nat.ModEq
        (2 ^ (total_division_count η + 1))
        x rη →
      Nat.ModEq
        (2 ^ (total_division_count ω + 1))
        x rω)
    ∨
    (∀ x : ℕ,
      Nat.ModEq
        (2 ^ (total_division_count ω + 1))
        x rω →
      Nat.ModEq
        (2 ^ (total_division_count η + 1))
        x rη)
    ∨
    (∀ x : ℕ,
      ¬ (
        Nat.ModEq
          (2 ^ (total_division_count ω + 1))
          x rω
        ∧
        Nat.ModEq
          (2 ^ (total_division_count η + 1))
          x rη)) := by
  /- Extract the equivalences between realization and membership in the
     corresponding exact residue classes. -/
  rcases hrω with ⟨_, hrω_class⟩
  rcases hrη with ⟨_, hrη_class⟩
  /- Realization classes are already known to be nested or disjoint. -/
  rcases
      realization_classes_nested_or_disjoint
        (ω := ω) (η := η)
    with hsubset | hrest
  /- If every realization of `η` realizes `ω`, then membership in the
     exact residue class of `η` implies membership in that of `ω`. -/
  · left
    intro x hx
    apply (hrω_class x).mp
    apply hsubset
    exact (hrη_class x).mpr hx
  · rcases hrest with hsubset | hdisjoint
    /- The opposite realization inclusion gives the opposite residue-class
       inclusion. -/
    · right
      left
      intro x hx
      apply (hrη_class x).mp
      apply hsubset
      exact (hrω_class x).mpr hx
    /- If no natural number realizes both words, then no natural number
       can belong to both exact realization residue classes. -/
    · right
      right
      intro x hx
      apply hdisjoint x
      constructor
      · exact (hrω_class x).mpr hx.1
      · exact (hrη_class x).mpr hx.2

/- End of the Collatz namespace. -/
end Collatz
