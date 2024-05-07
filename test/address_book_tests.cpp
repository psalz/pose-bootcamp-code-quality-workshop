#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "address_book.hpp"

TEST_CASE("entries can be added and removed") {
	address_book ab;
	CHECK_FALSE(ab.has_entry("Jane Doe"));
	ab.add_entry("Jane Doe");
	CHECK(ab.has_entry("Jane Doe"));
	ab.remove_entry("Jane Doe");
	CHECK_FALSE(ab.has_entry("Jane Doe"));
}

TEST_CASE("names are case-insensitive") {
	address_book ab;
	ab.add_entry("Jane Doe");
	CHECK(ab.has_entry("jAnE dOe"));
}

TEST_CASE("names are returned in a prettified form") {
	address_book ab;
	ab.add_entry("jane doe");
	CHECK(ab.get_entries()[0] == "Jane Doe");
}

TEST_CASE("a name has to be at least one character long") {
	address_book ab;
	CHECK_THROWS_WITH(ab.add_entry(""), "Name may not be empty");
	CHECK_NOTHROW(ab.add_entry("a"));
}

TEST_CASE("a name has to be at most 100 characters long") {
	address_book ab;
	CHECK_NOTHROW(ab.add_entry(std::string(max_name_length, 'a')));
	CHECK_THROWS_WITH(ab.add_entry(std::string(max_name_length + 1, 'a')), "Name too long");
}

TEST_CASE("get_entries() returns a sorted list of all entries") {
	address_book ab;
	ab.add_entry("Zacharias Doe");
	ab.add_entry("Alexander Doe");
	ab.add_entry("Harriet Doe");
	std::vector<std::string> entries = ab.get_entries();
	REQUIRE(entries.size() == 3);
	CHECK(entries[0] == "Alexander Doe");
	CHECK(entries[1] == "Harriet Doe");
	CHECK(entries[2] == "Zacharias Doe");
}

TEST_CASE("phone numbers can be set and retrieved") {
	address_book ab;
	ab.add_entry("Jane Doe");
	ab.set_phone_number("Jane Doe", 1234567890);
	CHECK(ab.get_phone_number("Jane Doe") == 1234567890);
	ab.set_phone_number("Jane Doe", 999);
	CHECK(ab.get_phone_number("Jane Doe") == 999);
}

TEST_CASE("trying to set a phone number on a non-existing entry throws") {
	address_book ab;
	CHECK_THROWS_WITH(ab.set_phone_number("Jane Doe", 1234567890), "Entry not found");
}

TEST_CASE("birthdays can be set and retrieved") {
	address_book ab;
	ab.add_entry("Jane Doe");
	ab.set_birthday("Jane Doe", std::chrono::month_day{std::chrono::month{5}, std::chrono::day{11}});
	std::chrono::month_day birthday = ab.get_birthday("Jane Doe");
	CHECK(birthday.month() == std::chrono::month{5});
	CHECK(birthday.day() == std::chrono::day{11});
	ab.set_birthday("Jane Doe", std::chrono::month_day{std::chrono::month{6}, std::chrono::day{12}});
	birthday = ab.get_birthday("Jane Doe");
	CHECK(birthday.month() == std::chrono::month{6});
	CHECK(birthday.day() == std::chrono::day{12});
}

TEST_CASE("trying to set an invalid birthday throws") {
	address_book ab;
	ab.add_entry("Jane Doe");
	CHECK_THROWS_WITH(ab.set_birthday("Jane Doe", std::chrono::month_day{std::chrono::month{13}, std::chrono::day{32}}),
	    "Invalid birthday");
}

TEST_CASE("trying to set a birthday on a non-existing entry throws") {
	address_book ab;
	CHECK_THROWS_WITH(ab.set_birthday("Jane Doe", std::chrono::month_day{std::chrono::month{5}, std::chrono::day{11}}),
	    "Entry not found");
}

TEST_CASE("the next birthday can be queried") {
	address_book ab;
	ab.add_entry("Jane Doe");
	ab.add_entry("John Doe");
	ab.add_entry("Zacharias Doe");
	std::chrono::month_day today = current_date();
	ab.set_birthday("Jane Doe", std::chrono::month_day{today.month() + std::chrono::months(2), today.day()});
	ab.set_birthday("John Doe", std::chrono::month_day{today.month() + std::chrono::months(3), today.day()});
	ab.set_birthday("Zacharias Doe", std::chrono::month_day{today.month() + std::chrono::months(1), today.day()});
	CHECK(ab.get_next_birthday() == "Zacharias Doe");
}

TEST_CASE("get_next_birthday throws if the address book is empty") {
	address_book ab;
	CHECK_THROWS_WITH(ab.get_next_birthday(), "No birthdays set");
}

class mock_synchronization_provider : public synchronization_provider {
  public:
	std::vector<std::string> synchronize(std::vector<std::string> serialized_entries) override {
		m_entries = merge_entries(serialized_entries, m_entries);
		return m_entries;
	}

  private:
	std::vector<std::string> m_entries;
};

TEST_CASE("address book can be synchronized") {
	address_book ab_1;
	ab_1.add_entry("Jane Doe");
	ab_1.add_entry("John Doe");
	ab_1.add_entry("Zacharias Doe");
	ab_1.set_phone_number("Jane Doe", 1234567890);
	ab_1.set_phone_number("John Doe", 999);
	ab_1.set_phone_number("Zacharias Doe", 123);
	ab_1.set_birthday("Jane Doe", std::chrono::month_day{std::chrono::month{5}, std::chrono::day{11}});
	ab_1.set_birthday("John Doe", std::chrono::month_day{std::chrono::month{6}, std::chrono::day{12}});
	ab_1.set_birthday("Zacharias Doe", std::chrono::month_day{std::chrono::month{7}, std::chrono::day{13}});
	mock_synchronization_provider provider;
	ab_1.synchronize(provider);

	address_book ab_2;
	ab_2.synchronize(provider);
	CHECK(ab_2.has_entry("Jane Doe"));
	CHECK(ab_2.get_phone_number("Jane Doe") == 1234567890);
	CHECK(ab_2.get_birthday("Jane Doe") == std::chrono::month_day{std::chrono::month{5}, std::chrono::day{11}});
	CHECK(ab_2.has_entry("John Doe"));
	CHECK(ab_2.get_phone_number("John Doe") == 999);
	CHECK(ab_2.get_birthday("John Doe") == std::chrono::month_day{std::chrono::month{6}, std::chrono::day{12}});
	CHECK(ab_2.has_entry("Zacharias Doe"));
	CHECK(ab_2.get_phone_number("Zacharias Doe") == 123);
	CHECK(ab_2.get_birthday("Zacharias Doe") == std::chrono::month_day{std::chrono::month{7}, std::chrono::day{13}});
}

TEST_CASE("local entries override remote entries") {
	mock_synchronization_provider provider;
	provider.synchronize({"Jane Doe,1234567890,4/17", "John Doe,999,12/18"});
	address_book ab;
	ab.add_entry("Jane Doe");
	ab.set_phone_number("Jane Doe", 123);
	ab.set_birthday("Jane Doe", std::chrono::month_day{std::chrono::month{5}, std::chrono::day{11}});
	ab.synchronize(provider);
	CHECK(ab.get_phone_number("Jane Doe") == 123);
	CHECK(ab.get_birthday("Jane Doe") == std::chrono::month_day{std::chrono::month{5}, std::chrono::day{11}});
	CHECK(ab.get_phone_number("John Doe") == 999);
	CHECK(ab.get_birthday("John Doe") == std::chrono::month_day{std::chrono::month{12}, std::chrono::day{18}});
}