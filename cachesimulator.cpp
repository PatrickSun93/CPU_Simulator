/*
advanced computer architecture
created by: hui wang, peidong sun
date 11/29/19
*/

#include "cachesim.h"
#include <cstdlib>
#include <cstring>
#include <math.h>
#include <cstdio>
#include <time.h>
#include <climits>
#include <fstream>
using namespace std;
CacheSim::CacheSim(int a_cache_size, int a_cache_line_size, int a_mapping_ways) {
    cache_size = (_u64) a_cache_size;
    cache_line_size = (_u64) a_cache_line_size;
	// way= cache size / ways size 
    cache_line_num = (_u64) a_cache_size / a_cache_line_size;
    cache_line_shifts = (_u64) log2(a_cache_line_size);
    // way sets
    cache_mapping_ways = (_u64) a_mapping_ways;
    // total set
    cache_set_size = cache_line_num / a_mapping_ways;
    // 
    cache_set_shifts = (_u64) log2(cache_set_size);
    // free block
    cache_free_num = cache_line_num;
    cache_hit_count = 0;
    cache_miss_count = 0;
    cache_r_count = 0;
    cache_w_count = 0;
    tick_count = 0;
    cache_buf = (_u8 *) malloc(this->cache_size);
    memset(cache_buf, 0, this->cache_size);
    // allocate way space 
    caches = (Cache_Line *) malloc(sizeof(Cache_Line) * cache_line_num);
    memset(caches, 0, sizeof(Cache_Line) * cache_line_num);
    //default swap alg
    swap_style = CACHE_SWAP_LRU;
    srand((unsigned) time(NULL));
}
CacheSim::~CacheSim() {
    free(caches);
    free(cache_buf);
}

void CacheSim::set_swap_style(int sty) {
    this->swap_style = sty;
}

int CacheSim::check_cache_hit(_u64 set_base, _u64 addr) {
    /**find in cache*/
    _u64 i;
    for (i = 0; i < cache_mapping_ways; ++i) {
        if ((caches[set_base + i].flag & CACHE_FLAG_VALID) &&
            (caches[set_base + i].tag == ((addr >> (cache_set_shifts + cache_line_shifts))))) {
            return set_base + i;
        }
    }
    return -1;
}
/*find free line*/
int CacheSim::get_cache_free_line(_u64 set_base) {
    _u64 i, min_count, j;
    int free_index;

    for (i = 0; i < cache_mapping_ways; ++i) {
        if (!(caches[set_base + i].flag & CACHE_FLAG_VALID)) {
            if (cache_free_num > 0)
                cache_free_num--;
            return set_base + i;
        }
    }
    free_index = -1;
    if (swap_style == CACHE_SWAP_RAND) {
        free_index = rand() % cache_mapping_ways;
    } else {
        min_count = UINT_MAX;
        for (j = 0; j < cache_mapping_ways; ++j) {
            if (caches[set_base + j].count < min_count && !(caches[set_base + j].flag &CACHE_FLAG_LOCK)) {
                min_count = caches[set_base + j].count;
                free_index = j;
            }
        }
    }
    if(free_index < 0){
        min_count = UINT_MAX;
        for (j = 0; j < cache_mapping_ways; ++j) {
            if (caches[set_base + j].count < min_count) {
                min_count = caches[set_base + j].count;
                free_index = j;
            }
        }
    }
    //
    if(free_index >= 0){
        free_index += set_base;
        if (caches[free_index].flag & CACHE_FLAG_DIRTY) {
            caches[free_index].flag &= ~CACHE_FLAG_DIRTY;
            cache_w_count++;
        }
    }
    return free_index;
}

/*write into cache line*/
void CacheSim::set_cache_line(_u64 index, _u64 addr) {
    Cache_Line *line = caches + index;
    line->buf = cache_buf + cache_line_size * index;
    // update line tage 
    line->tag = addr >> (cache_set_shifts + cache_line_shifts);
    line->flag = (_u8) ~CACHE_FLAG_MASK;
    line->flag |= CACHE_FLAG_VALID;
    line->count = tick_count;
}

void CacheSim::do_cache_op(_u64 addr, char oper_style) {
    _u64 set, set_base;
    long long index;
    set = (addr >> cache_line_shifts) % cache_set_size;
    set_base = set * cache_mapping_ways;
    tick_count++;
    index = check_cache_hit(set_base, addr);
    //hit
    if (index >= 0) {
        cache_hit_count++;
        //only LRU update time stamp, 
        if (CACHE_SWAP_LRU == swap_style)
            caches[index].lru_count = tick_count;

        if (oper_style == OPERATION_WRITE)
            caches[index].flag |= CACHE_FLAG_DIRTY;
    //miss
    } else {
        index = get_cache_free_line(set_base);
        if(index >= 0 ){
            set_cache_line((_u64) index, addr);
            if (oper_style == OPERATION_READ ) {
                cache_r_count++;
            } else {
                cache_w_count++;
            }
            cache_miss_count++;
        }
        
    }
}

/**load file */
void CacheSim::load_trace() {
    char buf[32];
    FILE *fin;
    // record trace op
    _u64 rcount = 0, wcount = 0;
    fin = fopen("gcc.trace", "r");
    if (!fin) {
        printf("load_file failed\n");
        return;
    }
    while (fgets(buf, sizeof(buf), fin)) {
        _u8 style = 0;
        _u64 addr = 0;
        sscanf(buf, "%c %x", &style, &addr);
        do_cache_op(addr, style);
        switch (style) {
            case 'l' :rcount++;break;
            case 's' :wcount++;break;
        }
    }
    printf("\ncache_size: %d Bytes\tline_size: %d\t mapping ways %d \t swap_style %d \n", cache_size,cache_line_size, cache_mapping_ways, swap_style);
    // op count 
    printf("all r/w/sum: %d %d %d\n",
           rcount, wcount, tick_count
           
    );
    // miss and hit number
    printf("miss/hit: %d/%d\t \n",cache_miss_count, cache_hit_count);
    fclose(fin);

}


