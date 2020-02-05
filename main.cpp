/*
advanced computer architecture
created by: hui wang, peidong sun
date 11/29/19
*/
#include <iostream>
#include <cstring>
#include <stdio.h>
#include "cachesimulator.h"
#include <fstream>
using namespace std;


int main(int argc, char * argv[]) {
    int line_size[] = {1,2,4,8, 16, 32, 64};
    int ways[] = {1, 2, 4, 8,16, 32};
    int cache_size[] = {0x800,0x1000,0x2000,0x4000,0x8000,0x10000,0x20000,0x40000};
    int i,j,m;
    ofstream outfile;
    outfile.open("result.txt",ios::app);
    for (m = 0;m<sizeof(cache_size)/sizeof(int);m++){
        for (i=0; i<sizeof(line_size)/sizeof(int); i++){
            for (j=0; j<sizeof(ways)/sizeof(int); j++){
                for (int k = CACHE_SWAP_FIFO; k <= CACHE_SWAP_RAND; ++k) {
                    CacheSim cache(cache_size[m], line_size[i], ways[j]);
                    cache.set_swap_style(k);
                    cache.load_trace();
                    outfile<<cache.cache_size<<","<<cache.cache_line_size<<","<<cache.cache_mapping_ways<<","<<cache.swap_style<<","<<cache.cache_miss_count<<","<<cache.cache_hit_count<<"."<<endl;
                    //delete cache;
                }
            }
        }
    }
    outfile.close();
    return 0;
}