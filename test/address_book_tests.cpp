#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <stdio.h>

#include "address_book.hpp"

class mock_synchronization_provider : public synchronization_provider {
	std::vector<std::string> entries;
	
	public:
		std::vector<std::string> synchronize(std::vector<std::string> serialized_entries) override {
			entries =  merge_entries(entries, serialized_entries);
			return entries;
		}

		

};

TEST_CASE("test address_book::syncronize") {
	address_book ab;
	mock_synchronization_provider provider = mock_synchronization_provider();

	ab.add_entry("Jane Doe");
	ab.synchronize(provider);
	ab.remove_entry("Jane Doe");

	ab.synchronize(provider);
	CHECK(ab.has_entry("Jane Doe"));
}

TEST_CASE("entries can be added and removed") {
	address_book ab;
	CHECK_FALSE(ab.has_entry("Jane Doe"));
	ab.add_entry("Jane Doe");
	CHECK(ab.has_entry("Jane Doe"));
	ab.remove_entry("Jane Doe");
	CHECK_FALSE(ab.has_entry("Jane Doe"));
}

TEST_CASE("entries are identified by their names -> no second entry with same name can be added") {
	address_book ab;
	ab.add_entry("name1");
	CHECK(ab.has_entry("name1"));
	ab.add_entry("name1");

	CHECK(ab.get_entries().size() == 1);
}


TEST_CASE("names are case-insensitive") {
	address_book ab;
	ab.add_entry("kleiner name");
	CHECK(ab.has_entry("kleiner name"));
	CHECK(ab.has_entry("kleiner Name"));
}

TEST_CASE("Attempting to add an entry with a name violating these restrictions throws an exception") {
	address_book ab;
	CHECK_THROWS_WITH(ab.add_entry("asdflöasdfkjashfoihasdoifnskfndjabsdfioasofdnasölfdknsödfhsoaifhnaläskdfnjksdfoiashüfdopinsalökfdnsaidfihjüaspofnkasöfnüaishdfanfdöasdflöasdfkjashfoihasdoifnskfndjabsdfioasofdnasölfdknsödfhsoaifhnaläskdfnjksdfoiashüfdopinsalökfdnsaidfihjüaspofnkasöfnüaishdfanfdö"), "Name too long");
	CHECK_THROWS_WITH(ab.add_entry(""), "Name may not be empty");
}

TEST_CASE("get_entries returns all entries ordered alphabetically (capital letter)") {
	address_book ab;
	ab.add_entry("C");
	ab.add_entry("A");
	ab.add_entry("B");

	auto entries = ab.get_entries();
	REQUIRE(entries.size() == 3);
	CHECK(entries[0] == "A");
	CHECK(entries[1] == "B");
	CHECK(entries[2] == "C");
}

TEST_CASE("Names returned by the address book have the first letter of each word capitalized") {
	address_book ab;
	ab.add_entry("jane m doe");

	auto entries = ab.get_entries();
	REQUIRE(entries.size() == 1);
	CHECK(entries[0] == "Jane M Doe");
}

TEST_CASE("A phone number and birthday can be set for each entry.") {
	address_book ab;
	ab.add_entry("a");

	ab.set_phone_number("a", 123);

}

TEST_CASE("Attempting to set a phone number or birthday on a non-existent entry throws an exception") {
	address_book ab;
	CHECK_THROWS(ab.set_phone_number("a", 123));

	unsigned int month = 2;
	unsigned int day = 3;
	CHECK_THROWS(ab.set_birthday("a", std::chrono::month_day{std::chrono::month{month}, std::chrono::day{day}}));
}

TEST_CASE("Attempting to set an invalid date as birthday throws an exception") {
	address_book ab;
	ab.add_entry("name");

	unsigned int month = 14;
	unsigned int day = 3;
	CHECK_THROWS(ab.set_birthday("name", std::chrono::month_day{std::chrono::month{month}, std::chrono::day{day}}));
}

