import Std
--import Mathlib.Tactic

section
variable (P Q : Prop)

-- This theorem states that conjunction is commutative
theorem my_theorem : P ∧ Q → Q ∧ P := by
  rintro h : P ∧ Q
  apply And.intro
  . exact And.right h
  . exact And.left h
end

section
variable (A B : Prop)
-- BEGIN
variable (h : A ∧ ¬ B)

#check And.left h
#check And.right h
#check And.intro (And.right h) (And.left h)
-- END
end

section
variable (A B C : Prop)
-- Here goes....
example (A B C : Prop) : ( A → ( B → C ) ) → ( ( A ∧ B ) → C ) := by
  intro (h2 : A → ( B → C )) (h1 : A ∧ B)
  exact h2 (And.left h1) (And.right h1)
end

section
variable (A B : Prop)

-- BEGIN
#check (fun h : A ∧ ¬ B ↦ And.intro (And.right h) (And.left h))
-- END

end

-- Good use of check to verify the types of expressions. The following
-- section demonstrates how to use variables and check their types in Lean.
section
  variable (A B C D : Prop)
  variable (h1: A → (B → C))
  variable (h2 : D → A)
  variable (h3 : D)
  variable (h4 : B)

  #check h2 h3
  #check h1 (h2 h3)
  #check h1 (h2 h3) h4
end

section
  variable (A B C : Prop)
  variable (h1: A → B)
  variable (h2: B → C)

#check (fun h3 : A ↦ h2 (h1 h3))
  example : A → C :=
    fun h3 : A ↦
    show C from h2 (h1 h3)
end

section
  variable (A B C : Prop)
  variable (h1: A ∧ B)
  variable (h2: A → (B → C))

  example : ( A ∧ B ) → C :=
    fun h3 : A ∧ B ↦
    show C from h2 (And.left h3) (And.right h3)
end

section
  example (A1 B : Prop) : A1 ∧ ¬ B → ¬ B ∧ A1 :=
  fun h : A1 ∧ ¬ B ↦
  show ¬ B ∧ A1 from And.intro
    (show ¬ B from And.right h)
    (show A1 from And.left h)
end

section
  variable (h2: A ∧ ¬ B)
  example (A B : Prop) : A ∧ ¬ B → ¬ B ∧ A :=
  fun h : A ∧ ¬ B ↦
--  show ¬ B from h.right
--  And.intro (And.right h) (And.left h)
  And.intro
    ( show ¬ B from (And.right h) )
    ( show A from (And.left h) )
#check And.intro h2.left
end

section
  variable (h : A)

  example : A ∨ B := Or.inl h
end

section
  variable (h : B)

  example : A ∨ B := Or.inr h
end

section
  variable (h : A ∨ B) (ha : A → C) (hb : B → C)
  example : C :=
  Or.elim h
    (fun h1 : A ↦
      show C from ha h1)
    (fun h1 : B ↦
      show C from hb h1)
  end

-- Bi-implication introduction example
section
  variable (A B : Prop) (h1 : A → B) (h2 : B → A)
  example : A ↔ B :=
  Iff.intro
    (fun h : A ↦
      show B from h1 h)
    (fun h : B ↦
      show A from h2 h)
  end

-- Bi-implication elimination examples
section
  variable (h1 : A ↔ B)
  variable (h2 : A)

  example : B := Iff.mp h1 h2
end

section
  variable (h1 : A ↔ B)
  variable (h2 : B)

  example : A := Iff.mpr h1 h2
end

section
  example (A B C : Prop) : (A → ( B → C ) ) → ( A ∧ B ) → C :=
  fun h1 : A → ( B → C ) ↦
  fun h2 : A ∧ B ↦
  show C from h1 (And.left h2) (And.right h2)
end

section
  example (A B C : Prop) : A ∧ (B ∨ C) → (A ∧ B) ∨ (A ∧ C) :=
  fun h1 : A ∧ (B ∨ C) ↦
  have h2 : A := And.left h1
  have h3 : B ∨ C := And.right h1
  show (A ∧ B) ∨ (A ∧ C) from
    Or.elim h3
      (fun h4 : B ↦
        have h5 : A ∧ B := And.intro h2 h4
        show (A ∧ B) ∨ (A ∧ C) from Or.inl h5)
      (fun h4 : C ↦
        have h5 : A ∧ C := And.intro h2 h4
        show (A ∧ B) ∨ (A ∧ C) from Or.inr h5)
  end

-- tactical proof of the same theorem
section
example (A B C : Prop) : A ∧ (B ∨ C) → (A ∧ B) ∨ (A ∧ C) := by
  intro (h1 : A ∧ (B ∨ C))
  cases h1 with
  | intro h1 h2 => cases h2 with
    | inl h2 =>
      apply Or.inl
      apply And.intro
      exact h1
      exact h2
    | inr h2 =>
      apply Or.inr
      apply And.intro
      exact h1
      exact h2
end

-- exercises
section
  example : A ∧ (A → B) → B :=
  fun h : A ∧ (A → B) ↦
  show B from (h.right) (h.left)

  example : A → ¬ (¬ A ∧ B) :=
  fun h1 : A ↦
  show ¬ (¬ A ∧ B) from
    fun h2 : ¬ A ∧ B ↦
      have h3 : ¬ A := h2.left
      show False from h3 h1

  example : ¬ (A ∧ B) → (A → ¬ B) :=
  fun h1 : ¬ (A ∧ B) ↦
  show (A → ¬ B) from
    fun h2 : A ↦
      show ¬ B from
        fun h3 : B ↦
          have h4 : A ∧ B := And.intro h2 h3
          show False from h1 h4

  example (h₁ : A ∨ B) (h₂ : A → C) (h₃ : B → D) : C ∨ D :=
  sorry

  example (h : ¬ A ∧ ¬ B) : ¬ (A ∨ B) :=
  sorry

  example : ¬ (A ↔ ¬ A) :=
  sorry
end
