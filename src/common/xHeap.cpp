//============================================================================
//
// Author: Evgeniy Golovin, egolovin@superscape.com
//
//============================================================================

#include "xHeap.h"

#include <malloc.h>
#include <string.h>

// #define ASSERT ASSERT
#define MALLOC malloc
#define FREE free
#define MEMSET memset
#define MEMCPY memcpy
#define STRLEN strlen
#define SNPRINTF sprintf_s

// #define USE_STD_MALLOC
#define FREE_FREEPAGES
#define FIND_BEST_FREE_BLOCK

#define DUMMY_SMALL_USED_ID_PRE  0xedededed
#define DUMMY_SMALL_USED_ID_POST 0xdededede

#define DUMMY_SMALL_FREE_ID_PRE  0xed5ded5d
#define DUMMY_SMALL_FREE_ID_POST 0xded5ded5

#define DUMMY_MEDIUM_USED_ID_PRE  0xedcdedcd
#define DUMMY_MEDIUM_USED_ID_POST 0xdedcdedc

#define DUMMY_MEDIUM_FREE_ID_PRE  0xed5cec5d
#define DUMMY_MEDIUM_FREE_ID_POST 0xded5cec5

#define DUMMY_LARGE_USED_ID_PRE  0xedadedad
#define DUMMY_LARGE_USED_ID_POST 0xdedadeda

#define DUMMY_LARGE_FREE_ID_PRE  0xed5aea5d
#define DUMMY_LARGE_FREE_ID_POST 0xded5aea5

#define SAVE_EFS_SIZE (1024*10)

void xHeap::SimpleStats::RegisterAlloc(uint32 usedSize, uint32 dataSize)
{
  allocCount++;
  this->usedSize += usedSize;
  this->dataSize += dataSize;

#ifdef DEBUG_APP_HEAP
  if(maxUsedSize < this->usedSize)
  {
    maxUsedSize = this->usedSize;
  }
  if(maxDataSize < this->dataSize)
  {
    maxDataSize = this->dataSize;
  }

  if(minBlockDataSize > dataSize)
  {
    minBlockDataSize = dataSize;
  }
  if(maxBlockDataSize < dataSize)
  {
    maxBlockDataSize = dataSize;
  }
#endif
}

void xHeap::SimpleStats::RegisterFree(uint32 usedSize, uint32 dataSize)
{
  freeCount++;
  this->usedSize -= usedSize;
  this->dataSize -= dataSize;
}

int xHeap::CeilPowerOfTwo(int x)
{
  x--;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x++;
  return x;
}

uint32 xHeap::SmallBlock::Size() const
{
  return sizeSlot * ALIGN + ALIGN;
}

inline uint32 xHeap::SmallBlock::DataSize() const
{ 
  return sizeSlot * ALIGN + ALIGN - sizeof(SmallBlock) - DUMMY_ID_SIZE;
}

#ifdef DEBUG_APP_HEAP
void xHeap::SmallBlock::ResetLink()
{
  prev = next = this;
}

void xHeap::SmallBlock::RemoveLink()
{
  prev->next = next;
  next->prev = prev;
}

void xHeap::SmallBlock::InsertAfter(SmallBlock * block)
{
  next = block->next;
  next->prev = this;
  block->next = this;
  prev = block;
}

void xHeap::SmallBlock::InsertBefore(SmallBlock * block)
{
  prev = block->prev;
  prev->next = this;
  block->prev = this;
  next = block;
}
#endif

#ifdef DEBUG_APP_HEAP
void * xHeap::AllocSmall(uint32 size, const char * filename, int line)
{
#else
void * xHeap::AllocSmall(uint32 size)
{
#endif

  // uint32 saveSize = size;
  size = (size + ALIGN - 1 + sizeof(SmallBlock) + DUMMY_ID_SIZE) & ~(ALIGN-1);

  uint32 i = size / ALIGN - 1;
  ASSERT(i < SMALL_SLOT_COUNT);

  SmallBlock * smallBlock;
  FreeSmallBlock * first = freeSmallBlocks[i];
  if(first)
  {
    smallBlock = (SmallBlock*)first;
    freeSmallBlocks[i] = first->next;
    smallStats.hitCount++;
  }
  else
  {
    uint32 freePageSize = smallPage->size - smallPageOffs;
    if(freePageSize < size)
    {
      if(freePageSize > ((ALIGN + ALIGN - 1 + sizeof(SmallBlock) + DUMMY_ID_SIZE) & ~(ALIGN-1)))
      {
        uint32 i = freePageSize / ALIGN - 1;
        ASSERT(i < SMALL_SLOT_COUNT);

        smallBlock = (SmallBlock*)(((uint8*)smallPage) + smallPageOffs);
        smallBlock->sizeSlot = (uint8)i;

      #ifdef DEBUG_APP_HEAP
        uint8 * p = (uint8*)(smallBlock + 1);
        *(int*)p = DUMMY_SMALL_FREE_ID_PRE;
        *(int*)(p + smallBlock->DataSize() + sizeof(int)) = DUMMY_SMALL_FREE_ID_POST;
      #endif

        ((FreeSmallBlock*)smallBlock)->next = freeSmallBlocks[i];
        freeSmallBlocks[i] = (FreeSmallBlock*)smallBlock;
      }
      SmallPage * newSmallPage = (SmallPage*)MALLOC(smallPageSize);
      if(!newSmallPage)
      {
        return NULL;
      }
      newSmallPage->size = smallPageSize;

      smallPageOffs = sizeof(*newSmallPage);
      smallStats.allocSize += newSmallPage->size;

      newSmallPage->next = smallPage;
      smallPage = newSmallPage;
    }
    else
    {
      smallStats.hitCount++;
    }
    ASSERT("Heap corrupted!" && (smallPageOffs & 3) == 0);
    smallBlock = (SmallBlock*)(((uint8*)smallPage) + smallPageOffs);
    smallPageOffs += size;
  }

  smallBlock->sizeSlot = (uint8)i;
#ifdef DEBUG_APP_HEAP
  smallBlock->filename = filename;
  smallBlock->line = line;
  smallBlock->InsertBefore(dummySmallBlock.next);
#endif
  uint32 dataSize = smallBlock->DataSize();
  smallStats.RegisterAlloc(smallBlock->Size(), dataSize);

  uint8 * p = (uint8*)(smallBlock + 1);
#ifndef USE_APP_HEAP_SAVING_MODE
  p[-1] = BT_SMALL;
#else
  p[-1] = BLOCK_TYPE_MASK;
#endif

#ifdef DEBUG_APP_HEAP
  *(int*)p = DUMMY_SMALL_USED_ID_PRE;
  p += sizeof(int);
  *(int*)(p + dataSize) = DUMMY_SMALL_USED_ID_POST;
#endif

  MEMSET(p, 0, dataSize);
  
#if defined(DEBUG_APP_HEAP) && defined(AEE_SIMULATOR)
  // CheckMemory();
#endif

  return p;
}

void xHeap::FreeSmall(void * p)
{
  ASSERT("Trying to free NULL pointer" && p);
  ASSERT("Heap corrupted!" 
      && smallStats.allocCount > smallStats.freeCount);
#ifdef DEBUG_APP_HEAP
  p = (uint8*)p - sizeof(int);
  ASSERT("Heap corrupted or trying to free alien memory"
      && *(int*)p == DUMMY_SMALL_USED_ID_PRE);
  *(int*)p = DUMMY_SMALL_FREE_ID_PRE;
#endif

  SmallBlock * smallBlock = ((SmallBlock*)p) - 1;
  uint32 i = smallBlock->sizeSlot;
  ASSERT("Heap corrupted!" && i < SMALL_SLOT_COUNT);

#ifdef DEBUG_APP_HEAP
  {
    int * check_p = (int*)((uint8*)p + smallBlock->DataSize() +
        sizeof(int));
    ASSERT("Heap corrupted!" && *check_p == DUMMY_SMALL_USED_ID_POST);
    *check_p = DUMMY_SMALL_FREE_ID_POST;

    smallBlock->RemoveLink();
  }
#endif

  smallStats.RegisterFree(smallBlock->Size(), smallBlock->DataSize());

  ((FreeSmallBlock*)smallBlock)->next = freeSmallBlocks[i];
  freeSmallBlocks[i] = (FreeSmallBlock*)smallBlock;

#if defined(DEBUG_APP_HEAP) && defined(AEE_SIMULATOR)
  // CheckMemory();
#endif
}

uint32 xHeap::SizeSmall(void * p)
{
  ASSERT("Trying to free NULL pointer" && p);

#ifdef DEBUG_APP_HEAP
  p = (uint8*)p - sizeof(int);
  ASSERT("Heap corrupted!" && *(int*)p == DUMMY_SMALL_USED_ID_PRE);
#endif

  SmallBlock * smallBlock = ((SmallBlock*)p) - 1;

#ifdef DEBUG_APP_HEAP
  ASSERT("Heap corrupted!" && *(int*)((uint8*)p + smallBlock->DataSize() +
        sizeof(int)) == DUMMY_SMALL_USED_ID_POST);
#endif

  return smallBlock->DataSize();
}

inline uint32 xHeap::Block::DataSize() const
{
  ASSERT("Heap corrupted!" && ((size - sizeof(Block) - DUMMY_ID_SIZE) & 3)
      == 0);
  return size - sizeof(Block) - DUMMY_ID_SIZE;
}

void xHeap::Block::ResetLink()
{
  prev = next = this;
}

void xHeap::Block::RemoveLink()
{
  prev->next = next;
  next->prev = prev;
}

void xHeap::Block::InsertAfter(Block * block)
{
  next = block->next;
  next->prev = this;
  block->next = this;
  prev = block;
}

void xHeap::Block::InsertBefore(Block * block)
{
  prev = block->prev;
  prev->next = this;
  block->prev = this;
  next = block;
}

#ifndef USE_APP_HEAP_SAVING_MODE
void xHeap::FreeBlock::ResetFreeLink()
{
  prevFree = nextFree = this;
}

void xHeap::FreeBlock::RemoveFreeLink()
{
  prevFree->nextFree = nextFree;
  nextFree->prevFree = prevFree;
}

void xHeap::FreeBlock::InsertAfterFreeLink(FreeBlock * block)
{
  nextFree = block->nextFree;
  nextFree->prevFree = this;
  block->nextFree = this;
  prevFree = block;
}

void xHeap::FreeBlock::InsertBeforeFreeLink(FreeBlock * block)
{
  prevFree = block->prevFree;
  prevFree->nextFree = this;
  block->prevFree = this;
  nextFree = block;
}

#ifdef DEBUG_APP_HEAP
void * xHeap::AllocMedium(uint32 size, const char * filename, int line)
{
#else
void * xHeap::AllocMedium(uint32 size)
{
#endif

#ifdef DEBUG_APP_HEAP
  // static int step = 0;
  // step++;
#endif

  uint32 saveSize = size;
  size = (size + ALIGN - 1 + sizeof(Block) + DUMMY_ID_SIZE) & ~(ALIGN - 1);
  Block * block = dummyFree.nextFree;
  if(block->size < size) // block == &dummyFree => dummyFree.size == 0
  {
    if(size > pageSize / 2)
    {
#ifdef DEBUG_APP_HEAP
      return AllocLarge(saveSize, filename, line);
#else
      return AllocLarge(saveSize);
#endif
    }
    block = (Block*)MALLOC(pageSize);
    if(!block)
    {
      return NULL;
    }
#ifdef DEBUG_APP_HEAP
    block->filename = filename;
    block->line = line;
#endif
    block->page = nextPage++;
    block->size = pageSize;
    block->isFree = true;
    block->InsertBefore(dummyBlock.next);
    ((FreeBlock*)block)->InsertBeforeFreeLink(dummyFree.nextFree);

    mediumStats.allocSize += block->size;
  }
  else
  {
    mediumStats.hitCount++;
#ifdef FIND_BEST_FREE_BLOCK
    for(FreeBlock * next = ((FreeBlock*)block)->nextFree; next->size >= size; )
    {
      block = next;
      next = ((FreeBlock*)block)->nextFree;
    }
#endif
  }

  block->size -= size;
  if(block->size < MAX_SMALL_SIZE && (block->size < MAX_SMALL_SIZE/2 || block->next->page != block->page))
  {
#ifdef DEBUG_APP_HEAP
    block->filename = filename;
    block->line = line;
#endif
    block->size += size;
    ((FreeBlock*)block)->RemoveFreeLink();
  }
  else
  {
#ifdef DEBUG_APP_HEAP
    *(int*)(block+1) = DUMMY_MEDIUM_FREE_ID_PRE;
    *(int*)((uint8*)(block+1) + block->DataSize() + sizeof(int)) = DUMMY_MEDIUM_FREE_ID_POST;
#endif

    if(block->size < ((FreeBlock*)block)->nextFree->size)
    {
      ((FreeBlock*)block)->RemoveFreeLink();
      InsertFreeBlock((FreeBlock*)block);
    }
    ASSERT("Heap corrupted!" && (block->size & 3) == 0);
    Block * newBlock = (Block*)(((uint8*)block) + block->size);
#ifdef DEBUG_APP_HEAP
    newBlock->filename = filename;
    newBlock->line = line;
#endif
    newBlock->page = block->page;
    newBlock->size = size;
    newBlock->InsertAfter(block);    
    block = newBlock;
  }
  block->isFree = false;

  uint32 dataSize = block->DataSize();
  mediumStats.RegisterAlloc(block->size, dataSize);

  uint8 * p = (uint8*)(block+1);
  p[-1] = BT_MEDIUM;

#ifdef DEBUG_APP_HEAP
  *(int*)p = DUMMY_MEDIUM_USED_ID_PRE;
  p += sizeof(int);
  *(int*)(p + dataSize) = DUMMY_MEDIUM_USED_ID_POST;
#endif

  MEMSET(p, 0, dataSize);

#if defined(DEBUG_APP_HEAP) && defined(AEE_SIMULATOR)
  // CheckMemory();
#endif

  return p;
}

void xHeap::FreeMedium(void * p)
{
  ASSERT("Trying to free NULL pointer" && p);
  ASSERT("Heap corrupted!" && mediumStats.allocCount >
      mediumStats.freeCount);

#ifdef DEBUG_APP_HEAP
  p = (uint8*)p - sizeof(int);
  ASSERT("Heap corrupted!" && *(int*)p == DUMMY_MEDIUM_USED_ID_PRE);
  // *(int*)p = DUMMY_MEDIUM_FREE_ID_PRE;
#endif

  Block * block = ((Block*)p) - 1;
  ASSERT("Double deallocation!" && !block->isFree);

#ifdef DEBUG_APP_HEAP
  {
    ASSERT("Heap corrupted!"
        && *((int*)((uint8*)p + block->DataSize() + sizeof(int)))
        == DUMMY_MEDIUM_USED_ID_POST);
    // *check_p = DUMMY_MEDIUM_FREE_ID_POST;
  }
#endif

  mediumStats.RegisterFree(block->size, block->DataSize());

  Block * prev = block->prev;
  if(prev->isFree && prev->page == block->page)
  {
    ASSERT("Heap corrupted!"
        && ((uint8*)prev) + prev->size == (uint8*)block);
    prev->size += block->size;

    ((FreeBlock*)prev)->RemoveFreeLink();
    block->RemoveLink();

    block = prev;
    mediumStats.mergeCount++;
  }
  Block * next = block->next;
  if(next->isFree && next->page == block->page)
  {
    ASSERT("Heap corrupted!"
        && ((uint8*)block) + block->size == (uint8*)next);
    block->size += next->size;
    
    ((FreeBlock*)next)->RemoveFreeLink();
    next->RemoveLink();

    mediumStats.mergeCount++;
  }
  block->isFree = true;

#ifdef DEBUG_APP_HEAP
  *(int*)(block+1) = DUMMY_MEDIUM_FREE_ID_PRE;
  *(int*)((uint8*)(block+1) + block->DataSize() + sizeof(int)) = DUMMY_MEDIUM_FREE_ID_POST;
#endif

  InsertFreeBlock((FreeBlock*)block);

#if defined(DEBUG_APP_HEAP) && defined(AEE_SIMULATOR)
  // CheckMemory();
#endif
}

uint32 xHeap::SizeMedium(void * p)
{
  ASSERT("Trying to free NULL pointer" && p);

#ifdef DEBUG_APP_HEAP
  p = (uint8*)p - sizeof(int);
  ASSERT("Heap corrupted!" && *(int*)p == DUMMY_MEDIUM_USED_ID_PRE);
#endif

  Block * block = ((Block*)p) - 1;

#ifdef DEBUG_APP_HEAP
  ASSERT("Heap corrupted!" && *(int*)((uint8*)p + block->DataSize() +
        sizeof(int)) == DUMMY_MEDIUM_USED_ID_POST);
#endif

  return block->DataSize();
}

void xHeap::InsertFreeBlock(FreeBlock * freeBlock)
{
  for(FreeBlock * cur = dummyFree.nextFree; ; cur = cur->nextFree)
  {
    ASSERT("Heap corrupted!" && (freeBlock != cur));
    if(freeBlock->size >= cur->size)
    {
      freeBlock->InsertBeforeFreeLink(cur);

      #ifdef FREE_FREEPAGES
        FreeBlock * nextFree;
        if(freeBlock->size == pageSize 
            && cur->size == pageSize 
            && (nextFree = cur->nextFree)->size == pageSize 
            && (nextFree = nextFree->nextFree)->size == pageSize
            && nextFree->nextFree->size == pageSize
            && cur->page != cur->next->page
            && cur->page != cur->prev->page
            )
        {
          mediumStats.allocSize -= cur->size;
          mediumStats.freePageCount++;

          cur->RemoveFreeLink();
          cur->RemoveLink();

          FREE(cur);
        }
      #endif

      return;
    }
    ASSERT("Heap corrupted!" && cur != &dummyFree);
    ASSERT("Heap corrupted!" && cur != cur->nextFree);
  }
}
#endif // USE_APP_HEAP_SAVING_MODE

#ifdef DEBUG_APP_HEAP
void * xHeap::AllocLarge(uint32 size, const char * filename, int line)
{
#else
void * xHeap::AllocLarge(uint32 size)
{
#endif
  size = (size + ALIGN - 1 + sizeof(Block) + DUMMY_ID_SIZE) & ~(ALIGN - 1);
  Block * block = (Block *)MALLOC(size);
  if(!block)
  {
    return NULL;
  }

#ifdef DEBUG_APP_HEAP
  block->filename = filename;
  block->line = line;
#endif
  block->page = (uint16)-1;
  block->size = size;
  block->isFree = false;
  block->InsertBefore(dummyLargeBlock.next);

  largeStats.allocSize += size;
  
  uint32 dataSize = block->DataSize();
  largeStats.RegisterAlloc(block->size, dataSize);

  uint8 * p = (uint8*)(block + 1);
#ifndef USE_APP_HEAP_SAVING_MODE
  p[-1] = BT_LARGE;
#else
  p[-1] &= ~BLOCK_TYPE_MASK;
#endif

#ifdef DEBUG_APP_HEAP
  *(int*)p = DUMMY_LARGE_USED_ID_PRE;
  p += sizeof(int);
  *(int*)(p + dataSize) = DUMMY_LARGE_USED_ID_POST;
#endif

  MEMSET(p, 0, dataSize);

#if defined(DEBUG_APP_HEAP) && defined(AEE_SIMULATOR)
  // CheckMemory();
#endif

  return p;
}

void xHeap::FreeLarge(void * p)
{
  ASSERT("Trying to free NULL pointer" && p);
  ASSERT("Heap corrupted!"
      && largeStats.allocCount > largeStats.freeCount);
#ifdef DEBUG_APP_HEAP
  p = (uint8*)p - sizeof(int);
  ASSERT("Heap corrupted!" && *(int*)p == DUMMY_LARGE_USED_ID_PRE);
  *(int*)p = DUMMY_LARGE_FREE_ID_PRE;
#endif

  Block * block = ((Block*)p) - 1;
  ASSERT("Double deallocation!" && !block->isFree);

#ifdef DEBUG_APP_HEAP
  {
    int * check_p = (int*)((uint8*)p + block->DataSize() + sizeof(int));
    ASSERT("Heap corrupted!" && *check_p == DUMMY_LARGE_USED_ID_POST);
    *check_p = DUMMY_LARGE_FREE_ID_POST;
  }
#endif

  uint32 size = block->size;
  largeStats.RegisterFree(size, block->DataSize());
  largeStats.allocSize -= size;

  block->RemoveLink();

  // MEMSET(block, 0, size);
  FREE(block);
}

uint32 xHeap::SizeLarge(void * p)
{
  ASSERT("Trying to free NULL pointer" && p);

#ifdef DEBUG_APP_HEAP
  p = (uint8*)p - sizeof(int);
  ASSERT("Heap corrupted!" && *(int*)p == DUMMY_LARGE_USED_ID_PRE);
#endif

  Block * block = ((Block*)p) - 1;

#ifdef DEBUG_APP_HEAP
  ASSERT("Heap corrupted!" && *(int*)((uint8*)p + block->DataSize() +
        sizeof(int)) == DUMMY_LARGE_USED_ID_POST);
#endif

  return block->DataSize();
}

xHeap::xHeap()
{
  ASSERT(CeilPowerOfTwo(APP_HEAP_CHUNK_SIZE) == APP_HEAP_CHUNK_SIZE);

  pageSize = CeilPowerOfTwo(DEF_PAGE_SIZE < MAX_SMALL_SIZE*8 ? MAX_SMALL_SIZE*8 : DEF_PAGE_SIZE);
  smallPageSize = pageSize;

  MEMSET(&smallStats, 0, sizeof(smallStats));
  MEMSET(&largeStats, 0, sizeof(largeStats));
  
#ifdef DEBUG_APP_HEAP
  smallStats.minBlockDataSize = 0xFFFFFFFF;
  largeStats.minBlockDataSize = 0xFFFFFFFF;
#endif

  dummySmallPage.size = 0;
  dummySmallPage.next = NULL;
  smallPage = &dummySmallPage;
  MEMSET(freeSmallBlocks, 0, sizeof(freeSmallBlocks));
  smallPageOffs = 0;
#ifdef DEBUG_APP_HEAP
  dummySmallBlock.ResetLink();
  dummySmallBlock.sizeSlot = 0;
  dummySmallBlock.filename = "#dummy#";
  dummySmallBlock.line = 0;
#endif

#ifndef USE_APP_HEAP_SAVING_MODE
  mediumSizeMask = pageSize/2-1;
  MEMSET(&mediumStats, 0, sizeof(mediumStats));

#ifdef DEBUG_APP_HEAP
  mediumStats.minBlockDataSize = 0xFFFFFFFF;
#endif

  dummyBlock.ResetLink();
  dummyBlock.size = 0;
  dummyBlock.page = (uint16)-1;
  dummyBlock.isFree = true;
  // dummyBlock.type = BT_MEDIUM;
#ifdef DEBUG_APP_HEAP
  dummyBlock.filename = "#dummy#";
  dummyBlock.line = 0;
#endif

  dummyFree.ResetLink();
  dummyFree.ResetFreeLink();
  dummyFree.size = 0;
  dummyFree.page = (uint16)-1;
  dummyFree.isFree = false;
  // dummyFree.type = BT_MEDIUM;
#ifdef DEBUG_APP_HEAP
  dummyFree.filename = "#dummy#";
  dummyFree.line = 0;
#endif

  nextPage = 1;

#endif // USE_APP_HEAP_SAVING_MODE

  dummyLargeBlock.ResetLink();
  dummyLargeBlock.size = 0;
  dummyLargeBlock.page = (uint16)-1;
  dummyLargeBlock.isFree = false;
  // dummyLargeBlock.type = BT_LARGE;
#ifdef DEBUG_APP_HEAP
  dummyLargeBlock.filename = "#dummy#";
  dummyLargeBlock.line = 0;
#endif

  if(!instance)
  {
    *((xHeap**)(&instance)) = this;
  }
}

xHeap::~xHeap()
{
#if defined(_DEBUG) || defined(DEBUG_APP_HEAP_DUMP_LEAK_ON_EXIT)
  const char * dumpFilename = "dump-err-exit.log";
  if(smallStats.allocCount != smallStats.freeCount
    #ifndef USE_APP_HEAP_SAVING_MODE
      || mediumStats.allocCount != mediumStats.freeCount
    #endif // USE_APP_HEAP_SAVING_MODE
      || largeStats.allocCount != largeStats.freeCount
    )
  {
    DumpUsage(dumpFilename);
  }
  else
  {
    // IFILEMGR_Remove(fileMgr, dumpFilename);
  }
#endif

#ifdef _MSC_VER
  ASSERT("Memory leak found!" &&
      smallStats.allocCount == smallStats.freeCount);
  #ifndef USE_APP_HEAP_SAVING_MODE
  ASSERT("Memory leak found!" &&
      mediumStats.allocCount == mediumStats.freeCount);
  #endif // USE_APP_HEAP_SAVING_MODE
  ASSERT("Memory leak found!" &&
      largeStats.allocCount == largeStats.freeCount);
#endif // _MSC_VER

  // if(smallStats.allocCount == smallStats.freeCount)
  {
    while(smallPage != &dummySmallPage)
    {
      SmallPage * curPage = smallPage;
      smallPage = smallPage->next;
      FREE(curPage);
    }
  }

  // if(largeStats.allocCount == largeStats.freeCount)
  {
    while(dummyLargeBlock.next != &dummyLargeBlock)
    {
      void * p = (char*)(dummyLargeBlock.next+1) + DUMMY_ID_SIZE/2;
      Free(p);
    }
  }

#ifndef USE_APP_HEAP_SAVING_MODE
  // if(mediumStats.allocCount == mediumStats.freeCount)
  {
    for(Block * block = dummyBlock.next, * next; block != &dummyBlock; block = next)
    {
      next = block->next;
      if(!block->isFree)
      {
        void * p = (char*)(block+1) + DUMMY_ID_SIZE/2;
        Free(p);
      }
    }
    while(dummyBlock.next != &dummyBlock)
    {
      Block * curBlockPage = dummyBlock.next;
      curBlockPage->RemoveLink();
#ifdef _MSC_VER
      ASSERT("Heap corrupted!"
        && curBlockPage->page != curBlockPage->next->page);
      ASSERT("Heap corrupted!"
        && curBlockPage->page != curBlockPage->prev->page);
#endif // _MSC_VER
      FREE(curBlockPage);
    }
  }
#endif // USE_APP_HEAP_SAVING_MODE

  if(instance == this)
  {
    *((xHeap**)(&instance)) = NULL;
  }
}

#ifdef DEBUG_APP_HEAP
void * xHeap::Alloc(uint32 size, const char * filename, int line)
#else
void * xHeap::Alloc(uint32 size)
#endif
{
  if(!size)
  {
    return NULL;
  }

#ifndef USE_STD_MALLOC
  if(!(size & ~(MAX_SMALL_SIZE - 1)))
  {
#ifdef DEBUG_APP_HEAP
    return AllocSmall(size, filename, line);
#else
    return AllocSmall(size);
#endif
  }

#if defined(DEBUG_APP_HEAP) && defined(AEE_SIMULATOR)
  // CheckMemory();
#endif

#ifndef USE_APP_HEAP_SAVING_MODE
  if(!(size & ~mediumSizeMask))
  {
#ifdef DEBUG_APP_HEAP
    return AllocMedium(size, filename, line);
#else
    return AllocMedium(size);
#endif
  }
#endif

#ifdef DEBUG_APP_HEAP
  return AllocLarge(size, filename, line);
#else
  return AllocLarge(size);
#endif

#else
  return MALLOC(size);
#endif
}

void xHeap::Free(void * p)
{
  if(!p)
    return;

#ifndef USE_STD_MALLOC
  #ifndef USE_APP_HEAP_SAVING_MODE
    switch(((uint8*)p)[-1-(int)DUMMY_ID_SIZE/2])
    {
    case BT_SMALL:
      FreeSmall(p);
      break;

    case BT_MEDIUM:
    #if defined(DEBUG_APP_HEAP) && defined(AEE_SIMULATOR)
      // CheckMemory();
    #endif
      FreeMedium(p);
      break;

    case BT_LARGE:
    #if defined(DEBUG_APP_HEAP) && defined(AEE_SIMULATOR)
      // CheckMemory();
    #endif
      FreeLarge(p);
      break;

    default:
    #if defined(DEBUG_APP_HEAP) && defined(AEE_SIMULATOR)
      CheckMemory();
    #endif
      ASSERT(false);
    }
  #else
    if(((uint8*)p)[-1-(int)DUMMY_ID_SIZE/2] & BLOCK_TYPE_MASK)
    {
      FreeSmall(p);
    }
    else
    {
      FreeLarge(p);
    }
  #endif

#else
  FREE(p);
#endif
}

#ifdef DEBUG_APP_HEAP
void * xHeap::Realloc(void * p, uint32 size, const char * filename, int line)
#else
void * xHeap::Realloc(void * p, uint32 size)
#endif
{
  if(!p)
  {
#ifdef DEBUG_APP_HEAP
    return Alloc(size, filename, line);
#else
    return Alloc(size);
#endif
  }
  if(!size)
  {
    Free(p);
    return NULL;
  }
#ifdef DEBUG_APP_HEAP
  void * newData = Alloc(size, filename, line);
#else
  void * newData = Alloc(size);
#endif
  if(newData)
  {
    uint32 oldSize = Size(p);
    MEMCPY(newData, p, oldSize < size ? oldSize : size);
    Free(p);
    return newData;
  }
  return NULL;
}

uint32 xHeap::Size(void * p)
{
  if(!p)
    return 0;

  #ifndef USE_APP_HEAP_SAVING_MODE
    
    switch(((uint8*)p)[-1-(int)DUMMY_ID_SIZE/2])
    {
    case BT_SMALL:
      return SizeSmall(p);

    case BT_MEDIUM:
      return SizeMedium(p);

    case BT_LARGE:
      return SizeLarge(p);
    }
    ASSERT(false);
    return 0; // shut up compiler

  #else
    
    if(((uint8*)p)[-1-(int)DUMMY_ID_SIZE/2] & BLOCK_TYPE_MASK)
    {
      return SizeSmall(p);
    }
    return SizeLarge(p);

  #endif
}

uint32 xHeap::PageSize() const { return pageSize; }
/*
void xHeap::SetPageSize(uint32 value)
{
#ifndef USE_APP_HEAP_SAVING_MODE
  FreeBlock * block = dummyFree.nextFree, * next;
  for(; block != &dummyFree; block = next)
  {
    next = block->nextFree;

    if(block->page != block->next->page && block->page != block->prev->page)
    {
      block->RemoveFreeLink();
      block->RemoveLink();
      
      mediumStats.allocSize -= block->size;
      mediumStats.freePageCount++;

      FREE(block);
    }
  }
#endif

  pageSize = CeilPowerOfTwo(value < MAX_SMALL_SIZE*8 ? MAX_SMALL_SIZE*8 : value);
  smallPageSize = pageSize;

#ifndef USE_APP_HEAP_SAVING_MODE
  mediumSizeMask = pageSize/2-1;
#endif
}
*/

static const char MemBlockTypeNames[3][7] = {"small", "medium", "large"};

void xHeap::CheckMemory()
{
#ifdef DEBUG_APP_HEAP

  uint32 allocSize, usedSize, dataSize;

  allocSize = usedSize = dataSize = 0;
  for(SmallPage * smallPage = this->smallPage; smallPage;
      smallPage = smallPage->next)
  {
    allocSize += smallPage->size;
  }
  ASSERT("Heap corrupted!" && allocSize == smallStats.allocSize);

  for(SmallBlock * smallBlock = dummySmallBlock.next;
      smallBlock != &dummySmallBlock; smallBlock = smallBlock->next)
  {
    ASSERT("Heap corrupted!" && smallBlock->next->prev == smallBlock);
    ASSERT("Heap corrupted!" && smallBlock->prev->next == smallBlock);

    uint32 blockDataSize = smallBlock->DataSize();
    ASSERT("Heap corrupted!" && (blockDataSize & 3) == 0);
    ASSERT("Heap corrupted!" && blockDataSize <= MAX_SMALL_SIZE);
    ASSERT("Heap corrupted!" && blockDataSize >=
        smallStats.minBlockDataSize);
    ASSERT("Heap corrupted!" && blockDataSize <=
        smallStats.maxBlockDataSize);

    ASSERT("Heap corrupted!" && *(int *)(smallBlock + 1) ==
        DUMMY_SMALL_USED_ID_PRE);
    ASSERT("Heap corrupted!" && *(int *)((uint8 *)(smallBlock + 1) +
          blockDataSize + sizeof(int)) == DUMMY_SMALL_USED_ID_POST);

    usedSize += smallBlock->Size();
    dataSize += blockDataSize;
  }
  ASSERT("Heap corrupted!" && usedSize == smallStats.usedSize);
  ASSERT("Heap corrupted!" && dataSize == smallStats.dataSize);

  for(int i = 0; i < SMALL_SLOT_COUNT; i++)
  {
    for(FreeSmallBlock * freeSmallBlock = freeSmallBlocks[i];
        freeSmallBlock; freeSmallBlock = freeSmallBlock->next)
    {
#ifndef NDEBUG /* fixing warning with unused smallBlock variable */
      SmallBlock * smallBlock = (SmallBlock*)freeSmallBlock;
      ASSERT("Heap corrupted!" && (smallBlock->DataSize() & 3) == 0);
      ASSERT("Heap corrupted!" && smallBlock->sizeSlot == (uint8)i);
      ASSERT("Heap corrupted!" && *(int *)(smallBlock + 1) ==
          DUMMY_SMALL_FREE_ID_PRE);
      ASSERT("Heap corrupted!" && *(int *)((uint8 *)(smallBlock + 1) +
            smallBlock->DataSize() + sizeof(int)) ==
          DUMMY_SMALL_FREE_ID_POST);
#endif/*NDEBUG*/
    }
  }

#ifndef USE_APP_HEAP_SAVING_MODE

  allocSize = usedSize = dataSize = 0;
  for(Block * block = dummyBlock.next; block != &dummyBlock;
      block = block->next)
  {
    ASSERT("Heap corrupted!" && block->isFree == 0 || block->isFree == 1);
    ASSERT("Heap corrupted!" && block->page < nextPage);
    ASSERT("Heap corrupted!" && block->next->prev == block);
    ASSERT("Heap corrupted!" && block->prev->next == block);
    
    uint32 blockDataSize = block->DataSize();
    ASSERT("Heap corrupted!" && (blockDataSize & 3) == 0);
    if(!block->isFree)
    {
      ASSERT("Heap corrupted!" && blockDataSize >=
          mediumStats.minBlockDataSize);
      ASSERT("Heap corrupted!" && blockDataSize <=
          mediumStats.maxBlockDataSize);
      ASSERT("Heap corrupted!" && *(int *)(block + 1) ==
          DUMMY_MEDIUM_USED_ID_PRE);
      ASSERT("Heap corrupted!" && *(int *)((uint8 *)(block + 1) +
            blockDataSize + sizeof(int)) == DUMMY_MEDIUM_USED_ID_POST);
      
      usedSize += block->size;
      dataSize += blockDataSize;
    }
    else
    {
      // ASSERT("Heap corrupted!" && dataSize < pageSize);
      ASSERT("Heap corrupted!" && *(int *)(block + 1) ==
          DUMMY_MEDIUM_FREE_ID_PRE);
      ASSERT("Heap corrupted!" && *(int *)((uint8 *)(block + 1) +
            blockDataSize + sizeof(int)) == DUMMY_MEDIUM_FREE_ID_POST);
    }
    allocSize += block->size;
  }
  ASSERT("Heap corrupted!" && allocSize == mediumStats.allocSize);
  ASSERT("Heap corrupted!" && usedSize == mediumStats.usedSize);
  ASSERT("Heap corrupted!" && dataSize == mediumStats.dataSize);

  for(FreeBlock * block = dummyFree.nextFree; block != &dummyFree;
      block = block->nextFree)
  {
    ASSERT("Heap corrupted!" && block->isFree == 1);
    ASSERT("Heap corrupted!" && block->page < nextPage);
    ASSERT("Heap corrupted!" && block->nextFree->prevFree == block);
    ASSERT("Heap corrupted!" && block->prevFree->nextFree == block);
  }
#endif // USE_APP_HEAP_SAVING_MODE

  allocSize = usedSize = dataSize = 0;

  for(Block * largeBlock = dummyLargeBlock.next;
      largeBlock != &dummyLargeBlock; largeBlock = largeBlock->next)
  {
    ASSERT("Heap corrupted!" && largeBlock->next->prev == largeBlock);
    ASSERT("Heap corrupted!" && largeBlock->prev->next == largeBlock);

    uint32 blockDataSize = largeBlock->DataSize();
    ASSERT("Heap corrupted!" && blockDataSize >=
        largeStats.minBlockDataSize);
    ASSERT("Heap corrupted!" && blockDataSize <=
        largeStats.maxBlockDataSize);

    allocSize += largeBlock->size;
    usedSize += largeBlock->size;
    dataSize += blockDataSize;
  }
  ASSERT("Heap corrupted!" && allocSize == largeStats.allocSize);
  ASSERT("Heap corrupted!" && usedSize == largeStats.usedSize);
  ASSERT("Heap corrupted!" && dataSize == largeStats.dataSize);
#endif // DEBUG_APP_HEAP
}

void xHeap::WriteFile(FILE * f, const char * buf, uint32& freeSize)
{
  size_t len = STRLEN(buf);
  freeSize -= (uint32)len;
  fwrite(buf, len, 1, f);
}

void xHeap::WriteStats(FILE * f, uint32& freeSize)
{
  char buf[256];

  SNPRINTF(buf, sizeof(buf)-1, "PAGE SIZE:\t%d\n\n", PageSize());
  WriteFile(f, buf, freeSize);

#ifndef USE_APP_HEAP_SAVING_MODE
  #ifdef DEBUG_APP_HEAP
    const char * headerStr = "TYPE\tCOUNT\talloc\tfree\thit\tcur\tSIZE\tavg\talloc\tused\tdata\tused max\tdata max\tmin block\tmax block\n";
  #else
    const char * headerStr = "Type\tCOUNT\talloc\tfree\thit\tcur\tSIZE\tavg\talloc\tused\tdata\n";
  #endif
#else // USE_APP_HEAP_SAVING_MODE
  #ifdef DEBUG_APP_HEAP
    const char * headerStr = "TYPE\tCOUNT\talloc\tfree\tcur\tSIZE\tavg\talloc\tused\tdata\tused max\tdata max\tmin block\tmax block\n";
  #else
    const char * headerStr = "Type\tCOUNT\talloc\tfree\tcur\tSIZE\tavg\talloc\tused\tdata\n";
  #endif
#endif // USE_APP_HEAP_SAVING_MODE
  WriteFile(f, headerStr, freeSize);

  Stats stats[3];
  GetStats(stats[0], stats[1], stats[2]);
  for(int i = 0; i < 3; i++)
  {
    if(!stats[i].allocSize)
      continue;
    
    uint32 curCount = stats[i].allocCount - stats[i].freeCount;
    uint32 curAvgDataSize = curCount ? stats[i].dataSize / curCount : 0;
#ifndef USE_APP_HEAP_SAVING_MODE
  #ifdef DEBUG_APP_HEAP
    SNPRINTF(buf, sizeof(buf)-1, "%s\t\t%d\t%d\t%d\t%d\t\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", MemBlockTypeNames[i], 
      stats[i].allocCount, stats[i].freeCount, stats[i].hitCount, curCount, curAvgDataSize,
      stats[i].allocSize, stats[i].usedSize, stats[i].dataSize, stats[i].maxUsedSize, stats[i].maxDataSize,
      stats[i].minBlockDataSize, stats[i].maxBlockDataSize);
  #else
    SNPRINTF(buf, sizeof(buf)-1, "%s\t\t%d\t%d\t%d\t%d\t\t%d\t%d\t%d\t%d\n", MemBlockTypeNames[i], 
      stats[i].allocCount, stats[i].freeCount, stats[i].hitCount, curCount, curAvgDataSize,
      stats[i].allocSize, stats[i].usedSize, stats[i].dataSize);
  #endif
#else // USE_APP_HEAP_SAVING_MODE
  #ifdef DEBUG_APP_HEAP
    SNPRINTF(buf, sizeof(buf)-1, "%s\t\t%d\t%d\t%d\t\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", MemBlockTypeNames[i], 
      stats[i].allocCount, stats[i].freeCount, curCount, curAvgDataSize,
      stats[i].allocSize, stats[i].usedSize, stats[i].dataSize, stats[i].maxUsedSize, stats[i].maxDataSize,
      stats[i].minBlockDataSize, stats[i].maxBlockDataSize);
  #else
    SNPRINTF(buf, sizeof(buf)-1, "%s\t\t%d\t%d\t%d\t\t%d\t%d\t%d\t%d\n", MemBlockTypeNames[i], 
      stats[i].allocCount, stats[i].freeCount, curCount, curAvgDataSize,
      stats[i].allocSize, stats[i].usedSize, stats[i].dataSize);
  #endif
#endif // USE_APP_HEAP_SAVING_MODE
    WriteFile(f, buf, freeSize);
  }

  SummaryStats summaryStats;
  GetStats(summaryStats);
  uint32 curCount = summaryStats.allocCount - summaryStats.freeCount;
#ifndef USE_APP_HEAP_SAVING_MODE
  #ifdef DEBUG_APP_HEAP
    SNPRINTF(buf, sizeof(buf)-1, "SUMMARY\t\t%d\t%d\t%d\t%d\t\t\t%d\t%d\t%d\t%d\t%d\n\n", 
      summaryStats.allocCount, summaryStats.freeCount, summaryStats.hitCount, curCount, 
      summaryStats.allocSize, summaryStats.usedSize, summaryStats.dataSize, 
      summaryStats.maxUsedSize, summaryStats.maxDataSize);
  #else
    SNPRINTF(buf, sizeof(buf)-1, "SUMMARY\t\t%d\t%d\t%d\t%d\t\t\t%d\t%d\t%d\n\n", 
      summaryStats.allocCount, summaryStats.freeCount, summaryStats.hitCount, curCount, 
      summaryStats.allocSize, summaryStats.usedSize, summaryStats.dataSize);
  #endif
#else // USE_APP_HEAP_SAVING_MODE
  #ifdef DEBUG_APP_HEAP
    SNPRINTF(buf, sizeof(buf)-1, "SUMMARY\t\t%d\t%d\t%d\t\t\t%d\t%d\t%d\t%d\t%d\n\n", 
      summaryStats.allocCount, summaryStats.freeCount, curCount, 
      summaryStats.allocSize, summaryStats.usedSize, summaryStats.dataSize, 
      summaryStats.maxUsedSize, summaryStats.maxDataSize);
  #else
    SNPRINTF(buf, sizeof(buf)-1, "SUMMARY\t\t%d\t%d\t%d\t\t\t%d\t%d\t%d\n\n", 
      summaryStats.allocCount, summaryStats.freeCount, curCount, 
      summaryStats.allocSize, summaryStats.usedSize, summaryStats.dataSize);
  #endif
#endif // USE_APP_HEAP_SAVING_MODE
  WriteFile(f, buf, freeSize);
}

// ==============================================================================================

#ifdef DEBUG_APP_HEAP
void xHeap::WriteSmallBlockHeader(FILE * f, uint32& freeSize)
{
  char buf[256];
  SNPRINTF(buf, sizeof(buf)-1, "== BLOCKS TYPE: %s ===============================================================\nSIZE\tLINE\tFILENAME\n", MemBlockTypeNames[0]);
  WriteFile(f, buf, freeSize);
}

void xHeap::WriteSmallBlock(FILE * f, SmallBlock * block, uint32& freeSize)
{
  char buf[256];
  SNPRINTF(buf, sizeof(buf)-1, "%d\t%d\t%s\n", block->DataSize(), block->line, block->filename);
  WriteFile(f, buf, freeSize);
}

void xHeap::WriteSmallBlocks(FILE * f, uint32& freeSize)
{
  SmallBlock * block = dummySmallBlock.next;
  for(; block != &dummySmallBlock; block = block->next)
  {
    WriteSmallBlock(f, block, freeSize);
  }
  WriteFile(f, "\n", freeSize);
}
#endif

// ==============================================================================================

void xHeap::WriteBlockHeader(FILE * f, int type, uint32& freeSize)
{
  char buf[256];
#ifdef DEBUG_APP_HEAP
  SNPRINTF(buf, sizeof(buf)-1, "== BLOCKS TYPE: %s ===============================================================\nSIZE\tPAGE\tLINE\tFILENAME\n", MemBlockTypeNames[type]);
#else
  SNPRINTF(buf, sizeof(buf)-1, "== BLOCKS TYPE: %s ===============================================================\nSIZE\tPAGE\n", MemBlockTypeNames[type]);
#endif
  WriteFile(f, buf, freeSize);
}

void xHeap::WriteBlock(FILE * f, Block * block, uint32& freeSize)
{
  char buf[256];
#ifdef DEBUG_APP_HEAP
  SNPRINTF(buf, sizeof(buf)-1, "%d\t%d\t%d\t%s\n", block->DataSize(), block->page, block->line, block->filename);
#else
  SNPRINTF(buf, sizeof(buf)-1, "%d\t%d\n", block->DataSize(), block->page);
#endif
  WriteFile(f, buf, freeSize);
}

void xHeap::WriteBlocks(FILE * f, Block * dummyBlock, uint32& freeSize)
{
  Block * block = dummyBlock->next;
  for(; block != dummyBlock; block = block->next)
  {
    if(!block->isFree)
    {
      WriteBlock(f, block, freeSize);
    }
  }
  WriteFile(f, "\n", freeSize);
}

// ==============================================================================================

#ifndef USE_APP_HEAP_SAVING_MODE

void xHeap::WriteFreeBlockHeader(FILE * f, uint32& freeSize)
{
  char buf[256];
#ifdef DEBUG_APP_HEAP
  SNPRINTF(buf, sizeof(buf)-1, "== FREE BLOCKS ===============================================================\nSIZE\tPAGE\tLINE\tFILENAME\n");
#else
  SNPRINTF(buf, sizeof(buf)-1, "== FREE BLOCKS ===============================================================\nSIZE\tPAGE\n");
#endif
  WriteFile(f, buf, freeSize);
}

void xHeap::WriteFreeBlocks(FILE * f, uint32& freeSize)
{
  FreeBlock * block = dummyFree.nextFree;
  for(; block != &dummyFree; block = block->nextFree)
  {
    WriteBlock(f, block, freeSize);
  }
  WriteFile(f, "\n", freeSize);
}

#endif // USE_APP_HEAP_SAVING_MODE

void xHeap::DumpUsage(const char * filename)
{
  FILE * f = fopen(filename, "wt");
  if(!f)
    return;

  uint32 freeSize = 0x7fffffff;

  WriteStats(f, freeSize);

#ifndef USE_APP_HEAP_SAVING_MODE
  WriteFreeBlockHeader(f, freeSize);
  WriteFreeBlocks(f, freeSize);
#endif // USE_APP_HEAP_SAVING_MODE

#ifdef DEBUG_APP_HEAP
  WriteSmallBlockHeader(f, freeSize);
  WriteSmallBlocks(f, freeSize);
#endif

#ifndef USE_APP_HEAP_SAVING_MODE
  WriteBlockHeader(f, 1, freeSize);
  WriteBlocks(f, &dummyBlock, freeSize);
#endif // USE_APP_HEAP_SAVING_MODE

  WriteBlockHeader(f, 2, freeSize);
  WriteBlocks(f, &dummyLargeBlock, freeSize);

  fclose(f);
}

void xHeap::GetStats(Stats& smallStats, Stats& mediumStats, Stats& largeStats)
{
  *(SimpleStats*)&smallStats = this->smallStats;
  smallStats.mergeCount = 0;

#ifndef USE_APP_HEAP_SAVING_MODE
  mediumStats = this->mediumStats;
#else
  MEMSET(&mediumStats, 0, sizeof(mediumStats));
#endif

  *(SimpleStats*)&largeStats = this->largeStats;
  largeStats.mergeCount = 0;

#ifdef DEBUG_APP_HEAP
  if(smallStats.minBlockDataSize > smallStats.maxBlockDataSize)
  {
    smallStats.minBlockDataSize = smallStats.maxBlockDataSize = 0;
  }
  if(mediumStats.minBlockDataSize > mediumStats.maxBlockDataSize)
  {
    mediumStats.minBlockDataSize = mediumStats.maxBlockDataSize = 0;
  }
  if(largeStats.minBlockDataSize > largeStats.maxBlockDataSize)
  {
    largeStats.minBlockDataSize = largeStats.maxBlockDataSize = 0;
  }
#endif
}

void xHeap::GetStats(SummaryStats& stats)
{
#ifndef USE_APP_HEAP_SAVING_MODE
  stats.allocCount = smallStats.allocCount + mediumStats.allocCount + largeStats.allocCount;
  stats.allocSize = smallStats.allocSize + mediumStats.allocSize + largeStats.allocSize;
  stats.usedSize = smallStats.usedSize + mediumStats.usedSize + largeStats.usedSize;
  stats.dataSize = smallStats.dataSize + mediumStats.dataSize + largeStats.dataSize;
  stats.freeCount = smallStats.freeCount + mediumStats.freeCount + largeStats.freeCount;
  stats.hitCount = smallStats.hitCount + mediumStats.hitCount + largeStats.hitCount;
  stats.mergeCount = mediumStats.mergeCount;

#ifdef DEBUG_APP_HEAP
  stats.maxUsedSize = smallStats.maxUsedSize + mediumStats.maxUsedSize + largeStats.maxUsedSize;
  stats.maxDataSize = smallStats.maxDataSize + mediumStats.maxDataSize + largeStats.maxDataSize;

  if(smallStats.minBlockDataSize > smallStats.maxBlockDataSize)
  {
    stats.minSmallBlockDataSize = stats.maxSmallBlockDataSize = 0;
  }
  else
  {
    stats.minSmallBlockDataSize = smallStats.minBlockDataSize;
    stats.maxSmallBlockDataSize = smallStats.maxBlockDataSize;
  }

  if(mediumStats.minBlockDataSize > mediumStats.maxBlockDataSize)
  {
    stats.minMediumBlockDataSize = stats.maxMediumBlockDataSize = 0;
  }
  else
  {
    stats.minMediumBlockDataSize = mediumStats.minBlockDataSize;
    stats.maxMediumBlockDataSize = mediumStats.maxBlockDataSize;
  }

  if(largeStats.minBlockDataSize > largeStats.maxBlockDataSize)
  {
    stats.minLargeBlockDataSize = stats.maxLargeBlockDataSize = 0;
  }
  else
  {
    stats.minLargeBlockDataSize = largeStats.minBlockDataSize;
    stats.maxLargeBlockDataSize = largeStats.maxBlockDataSize;
  }
#endif

#else // USE_APP_HEAP_SAVING_MODE

  stats.allocCount = smallStats.allocCount + largeStats.allocCount;
  stats.allocSize = smallStats.allocSize + largeStats.allocSize;
  stats.usedSize = smallStats.usedSize + largeStats.usedSize;
  stats.dataSize = smallStats.dataSize + largeStats.dataSize;
  stats.freeCount = smallStats.freeCount + largeStats.freeCount;
  stats.hitCount = smallStats.hitCount + largeStats.hitCount;
  stats.mergeCount = 0;

#ifdef DEBUG_APP_HEAP
  stats.maxUsedSize = smallStats.maxUsedSize + largeStats.maxUsedSize;
  stats.maxDataSize = smallStats.maxDataSize + largeStats.maxDataSize;

  if(smallStats.minBlockDataSize > smallStats.maxBlockDataSize)
  {
    stats.minSmallBlockDataSize = stats.maxSmallBlockDataSize = 0;
  }
  else
  {
    stats.minSmallBlockDataSize = smallStats.minBlockDataSize;
    stats.maxSmallBlockDataSize = smallStats.maxBlockDataSize;
  }

  if(largeStats.minBlockDataSize > largeStats.maxBlockDataSize)
  {
    stats.minLargeBlockDataSize = stats.maxLargeBlockDataSize = 0;
  }
  else
  {
    stats.minLargeBlockDataSize = largeStats.minBlockDataSize;
    stats.maxLargeBlockDataSize = largeStats.maxBlockDataSize;
  }
#endif

#endif // USE_APP_HEAP_SAVING_MODE
}

xHeap * xHeap::instance = NULL;

void xHeap::Init()
{
  if(!instance)
  {
    volatile xHeap * heap = new xHeap();
    atexit(Shutdown);
  }
}

void __cdecl xHeap::Shutdown()
{
  if(instance)
  {
    delete instance;
  }
}

// =============================================

struct __app_heap_manager_start__
{
  __app_heap_manager_start__()
  { 
    xHeap::Init();
  }
} __app_heap_manager_start__;

// =============================================
