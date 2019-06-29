//
// @todo: change implementation so there's no dead memory -- in pool
// initialization, it should reserve the requested size plus the necessary
// space for internal usage: begin, end, first and last units
//
struct VariableSizeMemoryUnit {
    VariableSizeMemoryUnit* prevUnit = nullptr;         // < Used on freeMemory
    VariableSizeMemoryUnit* nextFreeUnit = nullptr;     // < Used only if unit ever has idleSpace
    VariableSizeMemoryUnit* prevFreeUnit = nullptr;     // < Used only if unit ever has idleSpace
    unsigned long contentSize = 0;
    unsigned long idleSpace = 0;
};

struct VariableSizeMemoryPool {
    void* poolBegin = nullptr;                          // < Pointer to the begining of the pool
    VariableSizeMemoryUnit* firstUnit = nullptr;        // < Pointer to the begining of the pool
    VariableSizeMemoryUnit* bottomUnit = nullptr;       // < 
    VariableSizeMemoryUnit* freeBegin = nullptr;        // < Pointer to list head. freeBegin->nextFreeUnit is the actual first free unit
    VariableSizeMemoryUnit* freeEnd = nullptr;          // < Pointer to list end. freeEnd never changes after initialization because all
                                                        //   insertions are of the 'push_front' sort
    unsigned long long grossSize = 0;
    unsigned long long usedSpace = 0;
    unsigned long long unitsCount = 0;
    unsigned long long freeUnitsCount = 0;
    unsigned long deadMemory = 0;
};

VariableSizeMemoryUnit* getNextUnit(VariableSizeMemoryUnit* unit) {
    unsigned long offset = sizeof(VariableSizeMemoryUnit) + unit->contentSize + unit->idleSpace;
    return (VariableSizeMemoryUnit*) ((unsigned char*)unit + offset);
}

unsigned long long initialize(VariableSizeMemoryPool* pool, unsigned long long grossSize) {
    pool->poolBegin = malloc(grossSize);
    // @todo: implement assert
    assert(pool->poolBegin);
    pool->grossSize = grossSize;
    
#if 1
    // @note: clear memory to 0
    unsigned char* at = (unsigned char*) pool->poolBegin;
    for (unsigned long long i = 0; i < grossSize; ++i)
        at[i] = 0;
#endif
    
    pool->freeBegin = (VariableSizeMemoryUnit*) pool->poolBegin;
    pool->freeEnd = (VariableSizeMemoryUnit*) ((unsigned char*)pool->poolBegin + sizeof(VariableSizeMemoryUnit));
    pool->firstUnit = (VariableSizeMemoryUnit*) ((unsigned char*)pool->poolBegin + 2*sizeof(VariableSizeMemoryUnit));
    pool->bottomUnit = (VariableSizeMemoryUnit*) ((unsigned char*)pool->poolBegin + grossSize - sizeof(VariableSizeMemoryUnit));
    *pool->bottomUnit = {};
    pool->bottomUnit->prevUnit = pool->firstUnit;
    
    pool->freeBegin->prevUnit = nullptr;
    pool->freeBegin->nextFreeUnit = pool->freeEnd;
    pool->freeBegin->prevFreeUnit = nullptr;
    pool->freeBegin->contentSize = 0;
    pool->freeBegin->idleSpace = 0;
    
    pool->freeEnd->prevUnit = nullptr;
    pool->freeEnd->nextFreeUnit = nullptr;
    pool->freeEnd->prevFreeUnit = pool->freeBegin;
    pool->freeEnd->contentSize = 0;
    pool->freeEnd->idleSpace = 0;
    
    pool->firstUnit->prevUnit = nullptr;
    pool->firstUnit->nextFreeUnit = pool->freeEnd;
    pool->firstUnit->prevFreeUnit = pool->freeBegin;
    pool->firstUnit->contentSize = 0;
    pool->firstUnit->idleSpace = grossSize - 4*sizeof(VariableSizeMemoryUnit);
    
    pool->freeBegin->nextFreeUnit = pool->firstUnit;
    pool->freeEnd->prevFreeUnit = pool->firstUnit;
    
    pool->freeUnitsCount = 1;
    pool->unitsCount = 1;
    
    pool->usedSpace = 0;
    
    pool->deadMemory = 4*sizeof(VariableSizeMemoryUnit);
    // @note: returns the used space in memory pool
    return pool->deadMemory;
}

void* reserveMemory(VariableSizeMemoryPool* pool, unsigned long size) {
    // @note: assert pool has been initialized
    assert(pool->poolBegin && "Uninitialized VariableSizeMemoryPool");
    assert(pool->freeBegin->nextFreeUnit != pool->freeEnd);
    
    // @note: search unit with enough idle space to fit a new unit within it.
    VariableSizeMemoryUnit* freeUnit = pool->freeBegin->nextFreeUnit;
    unsigned long long grossNewUnitSize = sizeof(VariableSizeMemoryUnit) + size;
    while (freeUnit->idleSpace < grossNewUnitSize) {
        freeUnit = freeUnit->nextFreeUnit;
        assert(freeUnit != pool->freeEnd);
    }
    
    // @note: create newUnit
    unsigned long long offset = sizeof(VariableSizeMemoryUnit) + freeUnit->contentSize;
    VariableSizeMemoryUnit* newUnit = (VariableSizeMemoryUnit*) ((unsigned char*)freeUnit + offset);
    newUnit->prevUnit = freeUnit;
    newUnit->contentSize = size;
    newUnit->idleSpace = freeUnit->idleSpace - grossNewUnitSize;

    
    // @note: remove freeUnit from freeUnit list
    freeUnit->prevFreeUnit->nextFreeUnit = freeUnit->nextFreeUnit;
    freeUnit->nextFreeUnit->prevFreeUnit = freeUnit->prevFreeUnit;
    freeUnit->idleSpace = 0;
    --pool->freeUnitsCount;
    
    // @note: push front newUnit in freeUnit list, if it does have enough idleSpace
    if (newUnit->idleSpace > sizeof(VariableSizeMemoryUnit)) {
        newUnit->prevFreeUnit = pool->freeBegin;
        newUnit->nextFreeUnit = pool->freeBegin->nextFreeUnit;
        pool->freeBegin->nextFreeUnit->prevFreeUnit = newUnit;
        pool->freeBegin->nextFreeUnit = newUnit;
        ++pool->freeUnitsCount;
    }
    
    // @note: nextUnit needs to know that unitToFree is no longer its previous unit
    VariableSizeMemoryUnit* nextUnit = getNextUnit(newUnit);
    nextUnit->prevUnit = newUnit;
    
    ++pool->unitsCount;
    pool->usedSpace += sizeof(VariableSizeMemoryUnit) + newUnit->contentSize;
    
    return (void*) ((unsigned char*)newUnit + sizeof(VariableSizeMemoryUnit));
}

void freeMemory(VariableSizeMemoryPool* pool, void* p) {
    assert(p > pool->poolBegin && p < (void*) ((unsigned char*)pool->poolBegin + pool->grossSize));
    
    VariableSizeMemoryUnit* unitToFree = (VariableSizeMemoryUnit*) (((unsigned char*)p) - sizeof(VariableSizeMemoryUnit));
    
    if (unitToFree->idleSpace > sizeof(VariableSizeMemoryUnit)) {
        // @note: remove unitToFree from freeUnits list, if in it
        unitToFree->prevFreeUnit->nextFreeUnit = unitToFree->nextFreeUnit;
        unitToFree->nextFreeUnit->prevFreeUnit = unitToFree->prevFreeUnit;
        --pool->freeUnitsCount;
    }
    //
    // @todo: modify VariableSizeMemory implementation in a way that 
    // this if is always true, i.e., every unit has a prevUnit
    //
    if (unitToFree->prevUnit) {
        // @note: insert prevUnit in freeUnits list, if not in it
        if (unitToFree->prevUnit->idleSpace <= sizeof(VariableSizeMemoryUnit)) {
            unitToFree->prevUnit->prevFreeUnit = pool->freeBegin;
            unitToFree->prevUnit->nextFreeUnit = pool->freeBegin->nextFreeUnit;
            pool->freeBegin->nextFreeUnit->prevFreeUnit = unitToFree->prevUnit;
            pool->freeBegin->nextFreeUnit = unitToFree->prevUnit;
            ++pool->freeUnitsCount;
        }
        unitToFree->prevUnit->idleSpace += sizeof(VariableSizeMemoryUnit) + unitToFree->contentSize + unitToFree->idleSpace;
    }
    
    // @note: nextUnit needs to know that unitToFree is no longer its previous unit
    VariableSizeMemoryUnit* nextUnit = getNextUnit(unitToFree);
    nextUnit->prevUnit = unitToFree->prevUnit;
    
    --pool->unitsCount;
    pool->usedSpace -= sizeof(VariableSizeMemoryUnit) + unitToFree->contentSize;
    
#if VARIABLE_SIZE_MEMORY_POOL_FREE_TO_ZERO
    // @note: clear memory to 0
    unsigned long grossUnitSize = sizeof(VariableSizeMemoryUnit) + unitToFree->contentSize;
    unsigned char* at = (unsigned char*) unitToFree;
    for (unsigned long long i = 0; i < grossUnitSize; ++i)
        at[i] = 0;
#endif
}

void* shrink(VariableSizeMemoryPool* pool, void* p, unsigned long bytesToShrink) {
    // @todo 
    // @todo 
    // @todo 
    assert(p > pool->poolBegin && p < (void*) ((unsigned char*)pool->poolBegin + pool->grossSize));
    
    return nullptr;
}













