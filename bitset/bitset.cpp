#include "bitset.hpp"

namespace BitSet
{

inline baseint set_bit(baseint number, baseint pos) {
	return number | ((baseint)1 << pos);
}

inline baseint check_bit(baseint number, baseint pos) {
	return (number>>pos) & 0x1;
}

BitSet::BitSet() {}

BitSet::BitSet(uint64_t _n_buckets) {
	init(_n_buckets);
}

void BitSet::init(uint64_t _n_buckets) {
	n_buckets = _n_buckets;
	size_bits = n_buckets * SIZE_BITS(baseint);

	buckets = new baseint[n_buckets]();
}

BitSet::~BitSet() {
	delete[] buckets;
}

void BitSet::set(uint64_t location) {
	location %= size_bits;

	uint64_t bucket_index = location / SIZE_BITS(baseint);
	uint64_t pos = location % SIZE_BITS(baseint);

	buckets[bucket_index] |= ((baseint)1 << pos);
}

int BitSet::get(uint64_t location) {
	location %= size_bits;

	uint64_t bucket_index = location / SIZE_BITS(baseint);
	uint64_t pos = location % SIZE_BITS(baseint);

	return check_bit(buckets[bucket_index], pos);
}


void BitSet::clear() {
	for (baseint i = 0; i < n_buckets; i++) {
		buckets[i] = 0x0;
	}
}
	
} // namespace BitArray
