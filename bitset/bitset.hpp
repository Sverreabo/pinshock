#pragma once

#include <cstdint>

#define SIZE_BITS(type) (sizeof(type) * 8)

namespace BitSet
{

using baseint = uint64_t;


class BitSet {
public:
	uint64_t size_bits;
	uint64_t n_buckets;
	baseint* buckets = nullptr;

	BitSet();

	BitSet(uint64_t _n_buckets);

	~BitSet();

	void init(uint64_t _n_buckets);

	void set(uint64_t location);

	int get(uint64_t location);

	void clear();
};

} // namespace BitSet
