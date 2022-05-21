size_t jenkins_one_at_a_time_hash(const size_t* key, size_t length) {
  size_t i = 0;
  size_t hash = 0;
  while (i != length) {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash % length;
}
