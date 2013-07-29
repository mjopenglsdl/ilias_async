#include <ilias/ll_list.h>
#include <ilias/refcnt.h>
#include <atomic>
#include <exception>
#include <iostream>

class test_obj
:	public ilias::ll_list_hook<>,
	public ilias::refcount_base<test_obj>
{
public:
	const unsigned int idx;

	test_obj() noexcept;
	~test_obj() noexcept;

	static void ensure_count(unsigned int);
	static unsigned int get_count() noexcept;
	void ensure_index(unsigned int);

	test_obj(const test_obj&) = delete;
	test_obj(test_obj&&) = delete;
	test_obj& operator=(const test_obj&) = delete;
	test_obj& operator=(test_obj&&) = delete;

private:
	static std::atomic<unsigned int> index_;
	static std::atomic<unsigned int> count_;
};

ilias::refpointer<test_obj>
new_test_obj()
{
	return new test_obj{};
}

using list = ilias::ll_smartptr_list<test_obj>;


void test();	/* forward declaration, filled in by each test */


int
main()
{
	test();

	const unsigned int count = test_obj::get_count();
	if (count != 0) {
		std::cerr << count << " dangling test objects." << std::endl;
		return 1;
	}

	return 0;
}


std::atomic<unsigned int> test_obj::index_{ 0 };
std::atomic<unsigned int> test_obj::count_{ 0 };

void
test_obj::ensure_count(unsigned int expected)
{
	const unsigned int count = test_obj::get_count();
	if (count != expected) {
		std::cerr << "Expected " << expected << " test objects, "
		    "but found " << count << " instead." << std::endl;
		std::terminate();
	}
}

unsigned int
test_obj::get_count() noexcept
{
	return test_obj::count_;
}

void
test_obj::ensure_index(unsigned int v)
{
	if (this->idx != v) {
		std::cerr << "Expected test object " << v << ", "
		    "but found " << this->idx << "instead." <<std::endl;
		std::terminate();
	}
}

test_obj::test_obj() noexcept
:	idx{ index_.fetch_add(1U, std::memory_order_relaxed) }
{
	std::cout << "Create test_obj." << std::endl;
	count_.fetch_add(1U, std::memory_order_relaxed);
}

test_obj::~test_obj() noexcept
{
	count_.fetch_sub(1U, std::memory_order_relaxed);
	std::cout << "Destroy test_obj." << std::endl;
}