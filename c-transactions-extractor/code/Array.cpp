
#ifndef ArrayMAlloc
#include "memory.h"
#define ArrayMAlloc(size) malloc(size)
#endif

#ifndef ArrayMFree
#include "memory.h"
#define ArrayMFree(size) free(size)
#endif

#define Array(T, size)                (T*) _mallocArray(sizeof(T), size)
#define ArraySize(array)              (*((int*) (((char*) array) - 1*sizeof(int))))
#define ArrayIsEmpty(array)           ArraySize(array) == 0
#define ArrayTypeSize(array)          (*((int*) (((char*) array) - 2*sizeof(int))))
#define ArrayAdd(array, typeInstance) _addToArray((void**)(&(array)), &(typeInstance))
#define ArrayClear(array)             _clearArray((void**)&(array))
#define ArrayFree(array)              _freeArray((void**)&(array))
#define ArrayRemoveItem(array, item)  _removeItem((void**)&(array), (void*) item)
#define ArrayRemoveIndex(array, index) _removeIndex((void**)&(array), index)

void* _mallocArray(int typeSize, int arraySize) {
    void* arrayHeader = ArrayMAlloc(2*sizeof(int) + arraySize*typeSize);
    ArraySize(((int*)arrayHeader + 2)) = arraySize;
    ArrayTypeSize(((int*)arrayHeader + 2)) = typeSize;
    return (void*) (((char*)arrayHeader) + 2*sizeof(int));
}

void _freeArray(void** array) {
    ArrayMFree((char*)(*array) - 2*sizeof(int));
    *array = 0;
}

void _addToArray(void** array, void* typeInstance) {
    void* newArray = _mallocArray(ArrayTypeSize(*array), ArraySize(*array) + 1);
    memcpy(newArray, *array, ArraySize(*array) * ArrayTypeSize(*array));
    memcpy((void*)((char*)newArray + (ArraySize(newArray)-1)*ArrayTypeSize(newArray)), typeInstance, ArrayTypeSize(newArray));
    _freeArray(array);
    *array = newArray;
}

void _clearArray(void** array) {
    void* newArray = _mallocArray(ArrayTypeSize(*array), 0);
    _freeArray(array);
    *array = newArray;
}


void _removeIndex(void** array, int index) {
    // @todo @leak: memory leak! The proper way to remove a position from an array is to either
    // remember the total capacity of that array (and keep the current method) and free the whole
    // thing whenever, or copy all the contents to a new place (without the removed item of course)
    // and free the old buffer.
    // @todo @optimize
    for (int i = index+1; i < ArraySize(*array); ++i) {
        char* dest   = ((char*)*array) + (i-1)*ArrayTypeSize(*array);
        char* source = ((char*)*array) + i*ArrayTypeSize(*array);
        memcpy(dest, source, ArrayTypeSize(*array));
    }
    ArraySize(*array) -= 1;
}

void _removeItem(void** array, void* item) {
    // @todo @leak: memory leak! The proper way to remove a position from an array is to either
    // remember the total capacity of that array (and keep the current method) and free the whole
    // thing whenever, or copy all the contents to a new place (without the removed item of course)
    // and free the old buffer.
    // @todo @optimize
    for (int i = 0; i < ArraySize(*array); ++i) {
        char* testItem = ((char*)*array) + i*ArrayTypeSize(*array);
        if (memcmp(testItem, item, ArrayTypeSize(*array)) == 0) {
            _removeIndex(array, i);
            break;
        }
    }
}


