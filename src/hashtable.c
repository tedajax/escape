#include "hashtable.h"

HashTable *hashtable_new(u32 buckets) {
	HashTable *self = (HashTable *)malloc(sizeof(HashTable));

	self->bucketCount = buckets;
	self->buckets = (Vector **)malloc(sizeof(Vector *) * buckets);

	for (u32 i = 0; i < self->bucketCount; ++i) {
		self->buckets[i] = NULL;
	}

	return self;
}

bool hashtable_insert(HashTable *self, const char *key, void *data) {
	assert(self);
	assert(key);
	assert(data);

	u64 hash = _hashtable_djb2(key);
	u32 index = _hashtable_index(self, key);
	Vector *bucket = NULL;

	if (!self->buckets[index]) {
		self->buckets[index] = vector_new(8);
	}

	bucket = self->buckets[index];

	//search for the key in the bucket and if we find it return false
	for (u32 i = 0; i < bucket->size; ++i) {
		void *pkvp = vector_index(bucket, i);

		HashTableNode *kvp = (HashTableNode *)pkvp;
		if (kvp->key == hash) {
			return false;
		}
	}

	//since we have ruled out key collisions we can now safely add to the table
	//wrap the data up in a key value pair and insert to the bucket
	HashTableNode *kvp = (HashTableNode *)malloc(sizeof(HashTableNode));
	kvp->key = hash;
	kvp->value = data;
	vector_add(bucket, kvp);

	return true;
}

void *hashtable_get(HashTable *self, const char *key) {
	assert(self);
	assert(key);

	u64 hash = _hashtable_djb2(key);
	u32 index = _hashtable_index(self, key);
	Vector *bucket = self->buckets[index];

	if (!bucket) {
		return NULL;
	}

	for (u32 i = 0; i < bucket->size; ++i) {
		void *pkvp = vector_index(bucket, i);
		HashTableNode *kvp = (HashTableNode *)pkvp;
		if (kvp->key == hash) {
			return kvp->value;
		}
	}

	return NULL;
}

void *hashtable_remove(HashTable *self, const char *key) {
	assert(self);
	assert(key);

	u64 hash = _hashtable_djb2(key);
	u32 index = _hashtable_index(self, key);
	Vector *bucket = self->buckets[index];

	if (!bucket) {
		return NULL;
	}

	for (u32 i = 0; i < bucket->size; ++i) {
		void *pkvp = vector_index(bucket, i);
		HashTableNode *kvp = (HashTableNode *)pkvp;
		if (kvp->key == hash) {
			void *presult = kvp->value;
			vector_removeAt(bucket, i);
			return presult;
		}
	}

	return NULL;
}

u64 _hashtable_djb2(const char *key) {
	u64 hash = 5381;
	i32 c;
	while ((c = *key++)) {
		hash = ((hash << 5) + hash) + c; //hash * 33 + c
	}
	return hash;
}

u32 _hashtable_index(HashTable *self, const char *key) {
	return _hashtable_djb2(key) % self->bucketCount;
}