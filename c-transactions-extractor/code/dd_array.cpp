
#include <stdlib.h>
#include <memory.h>

#define arrheader(arr)                  (*(_arrheader*) (((char*)(arr)) - sizeof(_arrheader)))
#define arrcount(arr)                   arrheader(arr).count
#define arrcap(arr)                     arrheader(arr).capacity
#define arrtypesize(arr)                arrheader(arr).typesize
#define arrlast(arr)                    arr[arrcount(arr)-1]
#define arradd(arr, instance)           _arrAdd((void*&)arr, (void*)&instance)
#define arralloc(type, capacity)        (type*) _arrMalloc(sizeof(type), capacity)
#define arrfree(arr)                    _arrFree((void*&)arr)
#define arrclear(arr)                   _arrClear((void*&)arr)
#define arrremove(arr, index)           _arrRemove((void*&)arr, index)
#define arrremovematch(arr, instance)   _arrRemove((void*&)arr, _arrFind((void*&)arr, (void*)&instance))
#define arrfind(arr, instance)          _arrFind((void*&)arr, (void*)&instance)

struct _arrheader {
    int typesize;
    int capacity;
    int count;
};

void* _arrMalloc(int typesize, int capacity) {
    _arrheader* header = (_arrheader*) malloc(sizeof(_arrheader) + typesize*capacity);
    header->typesize = typesize;
    header->capacity = capacity;
    header->count = 0;
    return (void*) (header+1);
}

void _arrFree(void*& arr) {
    free(((char*)(arr)) - sizeof(_arrheader));
    arr = 0;
}

void _arrAdd(void*& array, void* instance) {
    if (arrcount(array) + 1 > arrcap(array)) {
        void* newarray = _arrMalloc(arrtypesize(array), arrcount(array) + 1);
        memcpy(newarray, array, arrcount(array) * arrtypesize(array));
        arrcount(newarray) = arrcount(array);
        _arrFree(array);
        array = newarray;
    }
    char* dest = ((char*)(array)) + arrcount(array)*arrtypesize(array);
    memcpy(dest, instance, arrtypesize(array));
    ++arrcount(array);
}

void _arrRemove(void*& array, int index) {
    for (int i = index+1; i < arrcount(array); ++i) {
        char* dest   = ((char*)array) + (i-1)*arrtypesize(array);
        char* source = ((char*)array) + i*arrtypesize(array);
        memcpy(dest, source, arrtypesize(array));
    }
    --arrcount(array);
}

int _arrFind(void*& array, void* instance) {
    for (int i = 0; i < arrcount(array); ++i) {
        char* testItem = ((char*)array) + i*arrtypesize(array);
        if (memcmp(testItem, instance, arrtypesize(array)) == 0) {
            return i;
        }
    }
    return -1;
}

void _arrClear(void*& array) {
    void* newArray = _arrMalloc(arrtypesize(array), 0);
    _arrFree(array);
    array = newArray;
}
