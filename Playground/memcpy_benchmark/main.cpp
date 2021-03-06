#include <string.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>


using namespace std::chrono;
typedef std::chrono::high_resolution_clock Clock;


long size = 1500;
long iterations = 20 * 1000 * 1000;
long cache_size  = 100;



static std::vector<void*> buf = []() -> std::vector<void*> {
    std::vector<void*> buf;
    buf.reserve(cache_size);
    while (static_cast<long>(buf.size()) < cache_size)
    {
        buf.push_back(malloc(size));
    }
    return buf;
}();


void* get(long size)
{
	static unsigned i = 0;
	return buf[i++ % buf.size()];
}


void* dst(long size) { return get(size); }
void* src(long size) { return get(size); }






int main()
{
    
    auto start = Clock::now();

    for (long i = 0; i != iterations; ++i)
    {
		memcpy(dst(size), src(size), size);
    }

    auto us = duration_cast<microseconds>(Clock::now() - start).count();


    auto mbps = 8.0 * iterations * size / us;
    auto gbps = static_cast<int>(0.5 + mbps / 10.0) / 100.0;
    std::cout << gbps << "Gbps" << std::endl;
}
