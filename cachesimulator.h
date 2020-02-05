/*
advanced computer architecture
created by: hui wang, peidong sun
date 11/29/19
*/
#ifndef CACHE_SIM
#define CACHE_SIM
typedef unsigned char _u8;
typedef unsigned long long _u64;

const unsigned char CACHE_FLAG_VALID = 0x01;
const unsigned char CACHE_FLAG_DIRTY = 0x02;
const unsigned char CACHE_FLAG_LOCK = 0x04;
const unsigned char CACHE_FLAG_MASK = 0xff;

const char OPERATION_READ = 'l';
const char OPERATION_WRITE = 's';
const char OPERATION_LOCK = 'k';
const char OPERATION_UNLOCK = 'u';

enum cache_swap_style {
    CACHE_SWAP_FIFO,
    CACHE_SWAP_LRU,
    CACHE_SWAP_RAND
};

//write method is write back by default
class Cache_Line {
public:
    _u64 tag;
    /**counter，FIFO the first visit time ，LRU last visit time*/
    union {
        _u64 count;
        _u64 lru_count;
        _u64 fifo_count;
    };
    _u8 flag;
    _u8 *buf;
};

class CacheSim{
public:
    /**whole cache size，byte*/
    _u64 cache_size;
    /**Cache block size*/
    _u64 cache_line_size;
    /**line number */
    _u64 cache_line_num;
    /**set way*/
    _u64 cache_mapping_ways;
    /**cache set number */
    _u64 cache_set_size;
    /** */
    _u64 cache_set_shifts;
    /**match address */
    _u64 cache_line_shifts;
    /**cache address*/
    Cache_Line *caches;
    /**count*/
    _u64 tick_count;
    /**cache buf*/
    _u8 *cache_buf;
    /**swap alg*/
    int swap_style;
    /** read and write count */
    _u64 cache_r_count, cache_w_count;
    /**cache hit, miss count*/
    _u64 cache_hit_count, cache_miss_count;
    /**cache line index*/
    _u64 cache_free_num;

    CacheSim(int a_cache_size,int a_cache_line_size, int a_mapping_ways);
    ~CacheSim();
    void set_swap_style(int a_swap_style);
  
    int check_cache_hit(_u64 set_base, _u64 addr);
    /**find cache set free line*/
    int get_cache_free_line(_u64 set_base);
    /**find right line and write data into the line */
    void set_cache_line(_u64 index, _u64 addr);
    /**cache operation */
    void do_cache_op(_u64 addr, char oper_style);
    /**read trace file*/
    void load_trace();
};	
#endif