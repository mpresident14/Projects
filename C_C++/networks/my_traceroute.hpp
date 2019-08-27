#ifndef MY_TRACEROUTE_HPP_INCLUDED
#define MY_TRACEROUTE_HPP_INCLUDED 1

#include <vector>
#include <memory>

class MyTraceroute {
	public:
		std::unique_ptr<std::vector<std::string>> runTraceroute() const;
		int testFn();
	private:
};

#endif