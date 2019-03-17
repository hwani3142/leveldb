/*
 * [190220][JH][Tiered LSM-tree]
 * LAYOUT header file
 */

#ifndef LAYOUT_H
#define LAYOUT_H

#include <libpmemobj.h>
// #include "pmem/ds/skiplist_map.h"
/* Single skiplist */
#define SKIPLIST_PATH "/home/hwan/pmem_dir/skiplist"
#define SKIPLIST_POOL_SIZE 30 * (1 << 20) // temp setting

/* Multiple skiplists controlled by manager */
#define SKIPLIST_MANAGER_PATH "/home/hwan/pmem_dir/skiplist_manager"
#define SKIPLIST_MANAGER_PATH_0 "/home/hwan/pmem_dir/skiplist_manager_0"
#define SKIPLIST_MANAGER_PATH_1 "/home/hwan/pmem_dir/skiplist_manager_1"
#define SKIPLIST_MANAGER_PATH_2 "/home/hwan/pmem_dir/skiplist_manager_2"
#define SKIPLIST_MANAGER_PATH_3 "/home/hwan/pmem_dir/skiplist_manager_3"
#define SKIPLIST_MANAGER_PATH_4 "/home/hwan/pmem_dir/skiplist_manager_4"
#define SKIPLIST_MANAGER_PATH_5 "/home/hwan/pmem_dir/skiplist_manager_5"
#define SKIPLIST_MANAGER_PATH_6 "/home/hwan/pmem_dir/skiplist_manager_6"
#define SKIPLIST_MANAGER_PATH_7 "/home/hwan/pmem_dir/skiplist_manager_7"
#define SKIPLIST_MANAGER_PATH_8 "/home/hwan/pmem_dir/skiplist_manager_8"
#define SKIPLIST_MANAGER_PATH_9 "/home/hwan/pmem_dir/skiplist_manager_9"
/* 
 * FIXME: Seek proper pool size
 * 1.0 : 75-23961
 * 1.95 : 148-12508
 * 3 : 228-23674
 */
// #define SKIPLIST_MANAGER_POOL_SIZE 300 * (1 << 20)
#define SKIPLIST_MANAGER_POOL_SIZE 2 * (1 << 30)
#define SKIPLIST_MANAGER_LIST_SIZE 100
// #define SKIPLIST_MANAGER_LIST_SIZE 10
#define MAX_SKIPLIST_NODE_SIZE 30720 // Compaction Output file max
// #define SKIPLIST_BULK_INSERT_NUM 10
#define NUM_OF_SKIPLIST_MANAGER 10

#define BUFFER_PATH "/home/hwan/pmem_dir/pmem_buffer"
#define BUFFER_PATH_0 "/home/hwan/pmem_dir/pmem_buffer_0"
#define BUFFER_PATH_1 "/home/hwan/pmem_dir/pmem_buffer_1"
#define BUFFER_PATH_2 "/home/hwan/pmem_dir/pmem_buffer_2"
#define BUFFER_PATH_3 "/home/hwan/pmem_dir/pmem_buffer_3"
#define BUFFER_PATH_4 "/home/hwan/pmem_dir/pmem_buffer_4"
#define BUFFER_PATH_5 "/home/hwan/pmem_dir/pmem_buffer_5"
#define BUFFER_PATH_6 "/home/hwan/pmem_dir/pmem_buffer_6"
#define BUFFER_PATH_7 "/home/hwan/pmem_dir/pmem_buffer_7"
#define BUFFER_PATH_8 "/home/hwan/pmem_dir/pmem_buffer_8"
#define BUFFER_PATH_9 "/home/hwan/pmem_dir/pmem_buffer_9"

#define BUFFER_POOL_SIZE 2 * (1 << 30)
#define NUM_OF_BUFFER 10
#define NUM_OF_CONTENTS 300
#define EACH_CONTENT_SIZE 4 << 20 // FIXME: 4MB
#define MAX_CONTENTS_SIZE (NUM_OF_CONTENTS * EACH_CONTENT_SIZE)

// TEST:
#define HASHMAP_PATH "/home/hwan/pmem_dir/pmem_hashmap"
#define HASHMAP_POOL_SIZE 3 * (1 << 20)

namespace levedb {

// POBJ_LAYOUT_ROOT(root_skiplist_map, struct skiplist_map_node);

// struct root_skiplist_map {
//  	TOID(struct skiplist_map_node) map;
// };
// TOID_DECLARE_ROOT(struct root_skiplist_map);

// typedef struct Root_skiplist_map {
//  	char buf[3000];
// } root_skiplist_map;

// TOID_DECLARE_ROOT(struct root_skiplist_map);

}
#endif