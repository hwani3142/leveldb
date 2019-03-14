// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "leveldb/options.h"

#include "leveldb/comparator.h"
#include "leveldb/env.h"

namespace leveldb {

Options::Options()
    : comparator(BytewiseComparator()),
      create_if_missing(false),
      error_if_exists(false),
      paranoid_checks(false),
      env(Env::Default()),
      info_log(nullptr),
      write_buffer_size(4<<20),

      max_open_files(1000), // determine table_cache_size
      // max_open_files(10),

      block_cache(nullptr),
      block_size(4096),
      block_restart_interval(16),
      max_file_size(2<<20),

      compression(kNoCompression),
      // compression(kSnappyCompression),
      
      reuse_logs(false),
      filter_policy(nullptr)

      /* sst implementation option */
      , sst_type(kPmemSST) // option 1
      // , sst_type(kFileDescriptorSST) // option 2

      /* Addtional cache option */
      , skiplist_cache(true)
      // , skiplist_cache(false)
       {
}

}  // namespace leveldb
