#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <string>

#include "address_book.hpp"

using namespace std::string_literals;

TEST_CASE("entries can be added and removed") {
	address_book ab;
	CHECK_FALSE(ab.has_entry("Jane Doe"));
	ab.add_entry("Jane Doe");
	CHECK(ab.has_entry("Jane Doe"));
	ab.remove_entry("Jane Doe");
	CHECK_FALSE(ab.has_entry("Jane Doe"));
}

TEST_CASE("names must be between 1 and 100 characters long") {
	address_book ab;
	std::string str_empty = "";
	CHECK(str_empty.length() == 0);
	CHECK_THROWS(ab.add_entry(str_empty));
	std::string str_101 = "AAAAAaaaaa"
						  "AAAAAaaaaa"
						  "AAAAAaaaaa"
						  "AAAAAaaaaa"
						  "AAAAAaaaaa"
						  "AAAAAaaaaa"
						  "AAAAAaaaaa"
						  "AAAAAaaaaa"
						  "AAAAAaaaaa"
						  "AAAAAaaaaaB";
	CHECK(str_101.length() == 101);
	CHECK_THROWS(ab.add_entry(str_101));
}