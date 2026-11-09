#include "test_helpers.h"
#include <cstdlib>
#include <new>

int Track::alive = 0;
int Track::copies_left = -1;
int Track::reads_left = -1;
bool Track::throw_output = false;
bool Track::throw_equality = false;
int TrackCompare::calls_left = -1;
int TrackCompare::copies_left = -1;

static bool allocation_failure = false;
void fail_next_allocation() { allocation_failure = true; }
void allow_allocations() { allocation_failure = false; }

void* operator new(std::size_t size) {
    if (allocation_failure) {
        allocation_failure = false;
        throw std::bad_alloc();
    }
    void* memory = std::malloc(size ? size : 1);
    if (!memory) throw std::bad_alloc();
    return memory;
}
void operator delete(void* memory) noexcept { std::free(memory); }
void operator delete(void* memory, std::size_t) noexcept { std::free(memory); }
void* operator new[](std::size_t size) { return ::operator new(size); }
void operator delete[](void* memory) noexcept { ::operator delete(memory); }
void operator delete[](void* memory, std::size_t) noexcept { ::operator delete(memory); }

std::string fixture_text(Order order) {
    const int sequence[6][7] = {
        {4,2,1,3,6,5,7}, {4,6,7,5,2,3,1}, {1,2,3,4,5,6,7},
        {7,6,5,4,3,2,1}, {1,3,2,5,7,6,4}, {7,5,6,3,1,2,4}
    };
    const char* names[] = {"NLR","NRL","LNR","RNL","LRN","RLN"};
    const char* paths[] = {"","LL","L","LR",".","RL","R","RR"};
    std::string text = std::string(names[int(order)]) + "\n";
    for (int i = 0; i < 7; ++i) {
        int key = sequence[int(order)][i];
        int color = key == 2 || key == 4 || key == 6 ? 1 : 0;
        text += record_text(paths[key], color, std::to_string(key), std::to_string(key * 10));
    }
    return text;
}

