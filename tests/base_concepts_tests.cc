#include "doctest.h"
#include "util/base_concepts.hh"

struct MyBase {};
struct MyChild : MyBase {};
struct MyGrandChild : MyChild {};
struct Unrelated {};

TEST_CASE("Derived concept: derived class satisfies Derived<T, Base>") {
	CHECK((Derived<MyChild, MyBase>) == true);
}

TEST_CASE("Derived concept: base class satisfies Derived<Base, Base> (is_base_of is reflexive)") {
	CHECK((Derived<MyBase, MyBase>) == true);
}

TEST_CASE("Derived concept: unrelated class does not satisfy Derived<Unrelated, Base>") {
	CHECK((Derived<Unrelated, MyBase>) == false);
}

TEST_CASE("Derived concept: inheritance chain") {
	CHECK((Derived<MyGrandChild, MyBase>) == true);
	CHECK((Derived<MyGrandChild, MyChild>) == true);
}
