/* slabs memory allocation */
#ifndef SLABS_H
#define SLABS_H

/** Init the subsystem. 1st argument is the limit on no. of bytes to allocate,
    0 if no limit. 2nd argument is the growth factor; each slab will use a chunk
    size equal to the previous slab's chunk size times this factor.
    3rd argument specifies if the slab allocator should allocate all memory
    up front (if true), or allocate memory in chunks as it is needed (if false)
*/

/***
 * 初始化Slab内存管理子系统
 * @limit[IN]: memcached子系统可用的内存大小限制, 0表示不限制
 * @factor[IN]: chunk增长因子, 每个Slab的chunk块大小都是前一个Slab的Chunk大小的factor倍
 * @prealloc[IN]: 是否预先分配limit指定大小的内存, true表示预先分配, false表示需要才分配
 */
void slabs_init(const size_t limit, const double factor, const bool prealloc);


/**
 * Given object size, return id to use when allocating/freeing memory for object
 * 0 means error: can't store such a large object
 */

/***
 *返回size大小对应的slabclass索引clsid，即size大小的trunk将放入slabclass[clsid]中
 *@size[IN]: 对象的大小
 *@return: >0表示clsid, 0表示对象太大
 */
unsigned int slabs_clsid(const size_t size);

/** Allocate object of given length. 0 on error */ /*@null@*/
/***
 * 从slabclass[id]中分配一个size大小的chunk
 * @size[IN]: 请求的内存大小
 * @id[IN]: 指定的clsid, 用于索引slabclass
 * @return: 成功返回内存的地址, 失败返回0(NULL)
 */
void *slabs_alloc(const size_t size, unsigned int id);

/** Free previously allocated object */
/***
 * 释放ptr指向的大小为size的内存区域, 即将其加入slabclass[id]的空闲内存块数组(freelist)中
 * @ptr[IN]: 要释放的内存
 * @size[IN]: 内存的大小
 * @id[IN]: 指定的clsid, 用于索引slabclass
 */
void slabs_free(void *ptr, size_t size, unsigned int id);

/** Adjust the stats for memory requested */
void slabs_adjust_mem_requested(unsigned int id, size_t old, size_t ntotal);

/** Return a datum for stats in binary protocol */
bool get_stats(const char *stat_type, int nkey, ADD_STAT add_stats, void *c);

/** Fill buffer with stats */ /*@null@*/
void slabs_stats(ADD_STAT add_stats, void *c);

int start_slab_maintenance_thread(void);
void stop_slab_maintenance_thread(void);

enum reassign_result_type {
    REASSIGN_OK=0, REASSIGN_RUNNING, REASSIGN_BADCLASS, REASSIGN_NOSPARE,
    REASSIGN_SRC_DST_SAME
};

enum reassign_result_type slabs_reassign(int src, int dst);

void slabs_rebalancer_pause(void);
void slabs_rebalancer_resume(void);

#endif
