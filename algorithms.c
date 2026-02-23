#include "algorithms.h"

void sort_step(int arr[], int size, SortState *state, AlgoType type, Uint64 perf_freq) {
    if (!state->is_sorting) return;

    Uint64 start_algo = SDL_GetPerformanceCounter();

    switch (type) {
        // ---------------- BUBBLE SORT ----------------
        case BUBBLE:
            if (state->i < size - 1) {
                state->comparisons++;
                if (arr[state->j] > arr[state->j + 1]) {
                    int temp = arr[state->j];
                    arr[state->j] = arr[state->j + 1];
                    arr[state->j + 1] = temp;
                }
                state->j++;
                if (state->j >= size - 1 - state->i) {
                    state->j = 0;
                    state->i++;
                }
            } else state->is_sorting = false;
            break;

        // ---------------- SELECTION SORT ----------------
        case SELECTION:
            if (state->i < size - 1) {
                state->comparisons++;
                if (arr[state->j] < arr[state->min_idx]) {
                    state->min_idx = state->j;
                }
                state->j++;
                if (state->j >= size) {
                    int temp = arr[state->i];
                    arr[state->i] = arr[state->min_idx];
                    arr[state->min_idx] = temp;
                    state->i++;
                    state->min_idx = state->i;
                    state->j = state->i + 1;
                }
            } else state->is_sorting = false;
            break;

        // ---------------- INSERTION SORT ----------------
        case INSERTION:
            if (state->i < size) {
                state->comparisons++;
                if (state->j >= 0 && arr[state->j] > state->key) {
                    arr[state->j + 1] = arr[state->j];
                    state->j--;
                } else {
                    arr[state->j + 1] = state->key;
                    state->i++;
                    if (state->i < size) {
                        state->key = arr[state->i];
                        state->j = state->i - 1;
                    }
                }
            } else state->is_sorting = false;
            break;

        // ---------------- QUICK SORT (Iterative Stack) ----------------
        case QUICK:
            if (state->top >= 0 || state->qs_partitioning) {
                if (!state->qs_partitioning) {
                    // Pop from stack
                    state->qs_high = state->stack[state->top--];
                    state->qs_low = state->stack[state->top--];
                    
                    if (state->qs_low < state->qs_high) {
                        state->qs_pivot = arr[state->qs_high];
                        state->qs_i = state->qs_low - 1;
                        state->j = state->qs_low;
                        state->qs_partitioning = true;
                    }
                } else {
                    // Step-by-step Partitioning
                    if (state->j < state->qs_high) {
                        state->comparisons++;
                        if (arr[state->j] < state->qs_pivot) {
                            state->qs_i++;
                            int temp = arr[state->qs_i];
                            arr[state->qs_i] = arr[state->j];
                            arr[state->j] = temp;
                        }
                        state->j++;
                    } else {
                        // Place Pivot
                        state->qs_i++;
                        int temp = arr[state->qs_i];
                        arr[state->qs_i] = arr[state->qs_high];
                        arr[state->qs_high] = temp;

                        // Push right sub-array to stack
                        if (state->qs_i + 1 < state->qs_high) {
                            state->stack[++state->top] = state->qs_low; // Placeholder, overwritten
                            state->stack[state->top - 1] = state->qs_i + 1;
                            state->stack[state->top] = state->qs_high;
                        }
                        // Push left sub-array to stack
                        if (state->qs_low < state->qs_i - 1) {
                            state->stack[++state->top] = state->qs_low;
                            state->stack[++state->top] = state->qs_i - 1;
                        }
                        state->qs_partitioning = false;
                    }
                }
            } else state->is_sorting = false;
            break;

        // ---------------- MERGE SORT (Iterative Bottom-Up) ----------------
       case MERGE:
            if (state->ms_curr_size < size) {
                if (state->ms_step == 0) { // Setup merge window
                    state->ms_mid = state->ms_left_start + state->ms_curr_size - 1;
                    state->ms_right_end = state->ms_left_start + 2 * state->ms_curr_size - 1;
                    if (state->ms_right_end >= size) state->ms_right_end = size - 1;

                    // Only merge if there is actually a right half to merge with
                    if (state->ms_mid < size - 1 && state->ms_mid < state->ms_right_end) {
                        state->ms_i = state->ms_left_start;
                        state->ms_j = state->ms_mid + 1;
                        state->ms_k = state->ms_left_start;
                        state->ms_step = 1;
                    } else {
                        // Skip if nothing to merge
                        state->ms_left_start += 2 * state->ms_curr_size;
                        if (state->ms_left_start >= size - 1) {
                            state->ms_left_start = 0;
                            state->ms_curr_size *= 2;
                        }
                    }
                } 
                else if (state->ms_step == 1) { // Merge comparisons
                    state->comparisons++;
                    if (state->ms_i <= state->ms_mid && state->ms_j <= state->ms_right_end) {
                        if (arr[state->ms_i] <= arr[state->ms_j]) {
                            state->temp_arr[state->ms_k++] = arr[state->ms_i++];
                        } else {
                            state->temp_arr[state->ms_k++] = arr[state->ms_j++];
                        }
                    } else if (state->ms_i <= state->ms_mid) {
                        state->temp_arr[state->ms_k++] = arr[state->ms_i++];
                    } else if (state->ms_j <= state->ms_right_end) {
                        state->temp_arr[state->ms_k++] = arr[state->ms_j++];
                    } else {
                        state->ms_step = 2; // Move to copy back
                    }
                    state->j = state->ms_k; // Visual cursor
                } 
                else if (state->ms_step == 2) { 
                    // OPTIMIZATION: Instant copy-back to the main array
                    for(int c = state->ms_left_start; c <= state->ms_right_end; c++) {
                        arr[c] = state->temp_arr[c];
                    }
                    
                    state->ms_step = 0;
                    state->ms_left_start += 2 * state->ms_curr_size;
                    if (state->ms_left_start >= size - 1) {
                        state->ms_left_start = 0;
                        state->ms_curr_size *= 2;
                    }
                }
            } else state->is_sorting = false;
            break;

        // ---------------- RADIX SORT (LSD) ----------------
       case RADIX:
            if (state->rx_step == 0) { // Find maximum value
                if (state->rx_i < size) {
                    state->comparisons++; 
                    if (arr[state->rx_i] > state->rx_max) state->rx_max = arr[state->rx_i];
                    state->j = state->rx_i;
                    state->rx_i++;
                } else {
                    state->rx_exp = 1;
                    state->rx_step = 1;
                }
            } 
            else if (state->rx_max / state->rx_exp > 0) {
                if (state->rx_step == 1) { // Reset count array
                    for (int c = 0; c < 10; c++) state->count[c] = 0;
                    state->rx_i = 0;
                    state->rx_step = 2;
                } 
                else if (state->rx_step == 2) { // Count frequencies
                    if (state->rx_i < size) {
                        state->count[(arr[state->rx_i] / state->rx_exp) % 10]++;
                        state->j = state->rx_i;
                        state->rx_i++;
                    } else {
                        for (int c = 1; c < 10; c++) state->count[c] += state->count[c - 1];
                        state->rx_i = size - 1;
                        state->rx_step = 3;
                    }
                } 
                else if (state->rx_step == 3) { // Build output array
                    if (state->rx_i >= 0) {
                        int index = (arr[state->rx_i] / state->rx_exp) % 10;
                        state->temp_arr[state->count[index] - 1] = arr[state->rx_i];
                        state->count[index]--;
                        state->j = state->rx_i;
                        state->rx_i--;
                    } else {
                        state->rx_step = 4;
                    }
                } 
                else if (state->rx_step == 4) { 
                    // OPTIMIZATION: Instant copy-back
                    for(int c = 0; c < size; c++) {
                        arr[c] = state->temp_arr[c];
                    }
                    state->rx_exp *= 10;
                    state->rx_step = 1;
                }
            } else state->is_sorting = false;
            break;
        default: break;
    }

    Uint64 end_algo = SDL_GetPerformanceCounter();
    state->actual_cpu_time += (double)(end_algo - start_algo) / perf_freq;
}

const char* get_algo_name(AlgoType type) {
    switch(type) {
        case INSERTION: return "Insertion Sort";
        case SELECTION: return "Selection Sort";
        case BUBBLE:    return "Bubble Sort";
        case MERGE:     return "Merge Sort";
        case QUICK:     return "Quick Sort";
        case RADIX:     return "Radix Sort";
        default:        return "Unknown";
    }
}

// ---------------- RESET LOGIC ----------------
void reset_sort_state(SortState *state, int arr[], int size,AlgoType type) {
    // Reset Shared Stats
    state->comparisons = 0;
    state->actual_cpu_time = 0.0;
    state->is_sorting = false;
    state->complete_sweep = -1;

    // Algorithm-Specific Setup
    if (type == INSERTION) {
        state->i = 1;
        state->j = 0;
        state->key = arr[1];
    } else if (type == SELECTION) {
        state->i = 0;
        state->j = 1;
        state->min_idx = 0;
    } else { // BUBBLE and others
        state->i = 0;
        state->j = 0;
    }

    // Quick Sort reset
    state->top = -1;
    state->stack[++state->top] = 0;
    state->stack[++state->top] = size - 1;
    state->qs_partitioning = false;

    // Merge Sort reset
    state->ms_curr_size = 1;
    state->ms_left_start = 0;
    state->ms_step = 0;

    // Radix Sort reset
    state->rx_max = 0;
    state->rx_i = 0;
    state->rx_step = 0;
}