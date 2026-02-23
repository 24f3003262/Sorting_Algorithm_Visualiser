#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <SDL2/SDL.h>
#include<stdbool.h>

#define MAX_ARRAY_SIZE 2000 //buffer for auxiliary array in merge n quick sort

typedef enum{
    INSERTION,SELECTION,MERGE,QUICK,RADIX,BUBBLE
} AlgoType;

typedef struct{
    //General
    int i,j,min_idx,key;
    int comparisons;
    double actual_cpu_time;
    bool is_sorting;
    int complete_sweep;

    //Quick sort state
    int stack[MAX_ARRAY_SIZE];
    int top;
    int qs_low,qs_high,qs_pivot,qs_i;
    bool qs_partitioning;

    //Merge Sort State
    int ms_curr_size, ms_left_start;
    int ms_mid,ms_right_end;
    int ms_i,ms_j,ms_k;
    int ms_step; //0:setup 1:merge 2:copy back

    //Radix Sort State
    int rx_exp,rx_max,rx_i;
    int count[10];
    int rx_step; // 0: find max, 1: init count, 2: count freq, 3: build output, 4: copy back

    //Shared Temp Array (Used by Merge and Radix)
    int temp_arr[MAX_ARRAY_SIZE];


} SortState;

void sort_step(int arr[],int size,SortState *state,AlgoType type,Uint64 perf_freq);
const char* get_algo_name(AlgoType type);
void reset_sort_state(SortState *state,int arr[],int size,AlgoType type);


#endif