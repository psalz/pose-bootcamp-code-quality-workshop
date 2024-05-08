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

TEST_CASE("entries can be added") {
	address_book ab;
    ab.add_entry("huuu");
	ab.add_entry("buh");
    ab.add_entry("waaah");
	CHECK(ab.has_entry("buh"));
	CHECK(ab.has_entry("BUH"));
	CHECK_THROWS(ab.add_entry(""));
	CAPTURE(ab.get_entries());
	CHECK("Buh" == ab.get_entries()[0]);
//	FAIL();
}

class variable_synchronization_provider : public synchronization_provider {
  public:
	variable_synchronization_provider(std::string path);
	std::vector<std::string> synchronize(std::vector<std::string> serialized_entries) override;

  private:
	std::string m_path;
};

variable_synchronization_provider::variable_synchronization_provider(std::string path) : m_path(path) {}

std::vector<std::string> variable_synchronization_provider::synchronize(std::vector<std::string> serialized_entries) {
	std::vector<std::string> merged_entries = serialized_entries;

	return merged_entries;
}

TEST_CASE("sychronized database tests") {
	address_book ab;
	variable_synchronization_provider provider{""};
	ab.synchronize(provider);
	CHECK_FALSE(ab.has_entry("buh"));

	ab.add_entry("buh");
	CHECK(ab.has_entry("buh"));
	ab.synchronize(provider);
	CHECK(ab.has_entry("buh"));
	ab.remove_entry("buh");
	CHECK_FALSE(ab.has_entry("buh"));
	ab.synchronize(provider);
	//CHECK(ab.has_entry("buh"));

}