// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "db/table_cache.h"

#include "db/filename.h"
#include "leveldb/env.h"
#include "leveldb/table.h"
#include "util/coding.h"

// JH
#include <chrono>
#include <iostream>

namespace leveldb {

struct TableAndFile {
  RandomAccessFile* file;
  Table* table;
};

static void DeleteEntry(const Slice& key, void* value) {
  TableAndFile* tf = reinterpret_cast<TableAndFile*>(value);
  delete tf->table;
  delete tf->file;
  delete tf;
}
// JH
static void DeletePmemEntry(const Slice& key, void* value) {
  PmemIterator* pmem_iterator = reinterpret_cast<PmemIterator*>(value);
  delete pmem_iterator;
}

static void UnrefEntry(void* arg1, void* arg2) {
  Cache* cache = reinterpret_cast<Cache*>(arg1);
  Cache::Handle* h = reinterpret_cast<Cache::Handle*>(arg2);
  cache->Release(h);
}

TableCache::TableCache(const std::string& dbname,
                       const Options& options,
                       int entries)
    : env_(options.env),
      dbname_(dbname),
      options_(options),
      cache_(NewLRUCache(entries)) {
}

TableCache::~TableCache() {
  delete cache_;
}

Status TableCache::FindTable(uint64_t file_number, uint64_t file_size,
                             Cache::Handle** handle) {
  Status s;
  char buf[sizeof(file_number)];
  EncodeFixed64(buf, file_number);
  Slice key(buf, sizeof(buf));
  *handle = cache_->Lookup(key);
  if (*handle == nullptr) {
    std::string fname = TableFileName(dbname_, file_number);
    RandomAccessFile* file = nullptr;
    Table* table = nullptr;
    s = env_->NewRandomAccessFile(fname, &file);
    if (!s.ok()) {
      std::string old_fname = SSTTableFileName(dbname_, file_number);
      if (env_->NewRandomAccessFile(old_fname, &file).ok()) {
        s = Status::OK();
      }
    }
    if (s.ok()) {
      s = Table::Open(options_, file, file_size, &table);
    }

    if (!s.ok()) {
      assert(table == nullptr);
      delete file;
      // We do not cache error results so that if the error is transient,
      // or somebody repairs the file, we recover automatically.
    } else {
      TableAndFile* tf = new TableAndFile;
      tf->file = file;
      tf->table = table;
      *handle = cache_->Insert(key, tf, 1, &DeleteEntry);
    }
  }
  return s;
}
// PROGRESS:
Status TableCache::FindSkiplist(uint64_t file_number, Cache::Handle** handle) {
  Status s;
  char buf[sizeof(file_number)];
  EncodeFixed64(buf, file_number);
  Slice key(buf, sizeof(buf));
  *handle = cache_->Lookup(key);
  if (*handle == nullptr) {
    // printf("Cache Insert %d\n", file_number);
    PmemSkiplist* pmem_skiplist = options_.pmem_skiplist[file_number%10];
    PmemIterator* pmem_iterator = new PmemIterator(file_number, pmem_skiplist); 
    *handle = cache_->Insert(key, 
          pmem_iterator, 
          1, 
          &DeletePmemEntry);
  } else {
    // printf("Cache Lookup success! %d\n", file_number);
  }
  return s;
}

Iterator* TableCache::NewIterator(const ReadOptions& options,
                                  uint64_t file_number,
                                  uint64_t file_size,
                                  Table** tableptr) {
  if (tableptr != nullptr) {
    *tableptr = nullptr;
  }

  Cache::Handle* handle = nullptr;
  Status s = FindTable(file_number, file_size, &handle);
  if (!s.ok()) {
    return NewErrorIterator(s);
  }

  Table* table = reinterpret_cast<TableAndFile*>(cache_->Value(handle))->table;
  Iterator* result = table->NewIterator(options);
  result->RegisterCleanup(&UnrefEntry, cache_, handle);
  if (tableptr != nullptr) {
    *tableptr = table;
  }
  return result;
}
// JH
/* TODO: Compaction based on pmem */
Iterator* TableCache::NewIteratorFromPmem(const ReadOptions& options,
                                  uint64_t file_number,
                                  uint64_t file_size,
                                  Table** tableptr) {
                                    
  bool on_cache =true;
  Iterator* result;
  if (options_.skiplist_cache) {
    if (tableptr != nullptr) {
      *tableptr = nullptr;
    }
    Cache::Handle* handle = nullptr;
    Status s = FindSkiplist(file_number, &handle); 

    result = reinterpret_cast<Iterator*>(cache_->Value(handle));
    result->SeekToFirst();
    
    result->RegisterCleanup(&UnrefEntry, cache_, handle);

  } else {
    PmemSkiplist* pmem_skiplist = options_.pmem_skiplist[file_number%10];
    result = new PmemIterator(file_number, pmem_skiplist);
    result->SeekToFirst();
  }

  return result;
}
Status TableCache::Get(const ReadOptions& options,
                       uint64_t file_number,
                       uint64_t file_size,
                       const Slice& k,
                       void* arg,
                       void (*saver)(void*, const Slice&, const Slice&)) {
  Cache::Handle* handle = nullptr;
  Status s = FindTable(file_number, file_size, &handle);
  if (s.ok()) {
    Table* t = reinterpret_cast<TableAndFile*>(cache_->Value(handle))->table;
    s = t->InternalGet(options, k, arg, saver);
    cache_->Release(handle);
  }
  return s;
}
// JH
/* SOLVE: Get based on pmem */
Status TableCache::GetFromPmem(const Options& options,
                   uint64_t file_number,
                   const Slice& k,
                   void* arg,
                   void (*saver)(void*, const Slice&, const Slice&)) {
  Status s; 
  // FIXME: Checks cache logic
  // bool on_cache = true;
  bool on_cache = false;
  if (on_cache) {
    Cache::Handle* handle = nullptr;
    s = FindSkiplist(file_number, &handle); 

    PmemIterator* pmem_iterator = 
                          reinterpret_cast<PmemIterator*>(cache_->Value(handle));
    pmem_iterator->Seek(k);
    (*saver)(arg, pmem_iterator->key(), pmem_iterator->value());
    cache_->Release(handle);

  } else {
    PmemIterator* pmem_iterator = options.pmem_internal_iterator[file_number%10]; 
    pmem_iterator->SetIndexAndSeek(file_number, k);
    (*saver)(arg, pmem_iterator->key(), pmem_iterator->value());
  }
  return s;
}


void TableCache::Evict(uint64_t file_number) {
  // printf("TableCache Eviction 1\n");
  char buf[sizeof(file_number)];
  EncodeFixed64(buf, file_number);
  cache_->Erase(Slice(buf, sizeof(buf)));
  // printf("TableCache Eviction 2\n");
}

}  // namespace leveldb
