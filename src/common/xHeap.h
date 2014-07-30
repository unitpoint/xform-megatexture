#ifndef __X_HEAP_H__
#define __X_HEAP_H__

#include "xNewDecl.h"
#include <stdio.h>

#ifndef APP_HEAP_CHUNK_SIZE
  #ifdef USE_APP_HEAP_SAVING_MODE
    #define APP_HEAP_CHUNK_SIZE 256
  #else
    #define APP_HEAP_CHUNK_SIZE 128
  #endif // USE_APP_HEAP_SAVING_MODE
#endif // APP_HEAP_CHUNK_SIZE

#ifndef APP_HEAP_PAGE_SIZE
#define APP_HEAP_PAGE_SIZE (1024 * 32)
#endif // APP_HEAP_PAGE_SIZE

#ifdef DEBUG_APP_HEAP
#define DUMMY_ID_SIZE (sizeof(int)*2)
#else
#define DUMMY_ID_SIZE 0
#endif

class xHeap: public xStdAllocImpl
{
protected:

  class SimpleStats
  {
  public:

    uint32 allocSize;
    uint32 usedSize;
    uint32 dataSize;

#ifdef DEBUG_APP_HEAP
    uint32 maxUsedSize;
    uint32 maxDataSize;
    uint32 minBlockDataSize;
    uint32 maxBlockDataSize;
#endif

    uint32 allocCount;
    uint32 freeCount;
    uint32 hitCount;

  protected:

    friend class xHeap;

    void RegisterAlloc(uint32 usedSize, uint32 dataSize);
    void RegisterFree(uint32 usedSize, uint32 dataSize);
  };

  static int CeilPowerOfTwo(int x);

public:

  struct Stats: public SimpleStats
  {
    uint32 mergeCount;
    uint32 freePageCount;
  };

public:

  enum 
  {
    ALIGN = 4,
    MAX_SMALL_SIZE = APP_HEAP_CHUNK_SIZE,
    DEF_PAGE_SIZE = APP_HEAP_PAGE_SIZE
  };

#ifndef USE_APP_HEAP_SAVING_MODE
  enum BlockType
  {
    BT_SMALL,
    BT_MEDIUM,
    BT_LARGE
  };
#else // USE_APP_HEAP_SAVING_MODE
  enum
  {
    BLOCK_TYPE_MASK = 0x80
  };
#endif // USE_APP_HEAP_SAVING_MODE

protected:

  uint32 pageSize, smallPageSize;

#ifndef USE_APP_HEAP_SAVING_MODE
  uint32 mediumSizeMask;
#endif

  struct SmallBlock
  {
#ifdef DEBUG_APP_HEAP
    SmallBlock * prev, * next;
    
    const char * filename;
    int line;

    void ResetLink();
    void RemoveLink();
    void InsertAfter(SmallBlock * block);
    void InsertBefore(SmallBlock * block);
#endif
    uint8 sizeSlot;
    uint8 pad0;
    uint8 pad1;
    uint8 pad2; // for type

    uint32 Size() const;
    uint32 DataSize() const;
  };

  struct FreeSmallBlock
  {
    FreeSmallBlock * next;
  };

  struct SmallPage
  {
    uint32 size;
    SmallPage * next;
  };

  enum
  {
    SMALL_SLOT_COUNT = (MAX_SMALL_SIZE + sizeof(SmallBlock) + DUMMY_ID_SIZE) / ALIGN + 1
  };

  FreeSmallBlock * freeSmallBlocks[SMALL_SLOT_COUNT];
  SmallPage dummySmallPage;
  SmallPage * smallPage;
  uint32 smallPageOffs;

#ifdef DEBUG_APP_HEAP
  SmallBlock dummySmallBlock;
#endif

  SimpleStats smallStats;

#ifdef DEBUG_APP_HEAP
  void * AllocSmall(uint32 size, const char * filename, int line);
#else
  void * AllocSmall(uint32 size);
#endif

  void FreeSmall(void * p);
  uint32 SizeSmall(void * p);

protected:

  struct Block
  {
    Block * prev, * next;

#ifdef DEBUG_APP_HEAP
    const char * filename;
    int line;
#endif

    uint32 size;
    uint16 page;
    uint8 isFree;
    uint8 pad0; // for type

    uint32 DataSize() const;
    void ResetLink();
    void RemoveLink();
    void InsertAfter(Block * block);
    void InsertBefore(Block * block);
  };

#ifndef USE_APP_HEAP_SAVING_MODE
  struct FreeBlock: public Block
  {
#ifdef DEBUG_APP_HEAP
    int pad0; // for DUMMY_MEDIUM_FREE_ID_PRE
#endif
    FreeBlock * prevFree, * nextFree;

    void ResetFreeLink();
    void RemoveFreeLink();
    void InsertAfterFreeLink(FreeBlock * block);
    void InsertBeforeFreeLink(FreeBlock * block);
  };

  Block dummyBlock;
  FreeBlock dummyFree;
  uint16 nextPage;

  Stats mediumStats;

#ifdef DEBUG_APP_HEAP
  void * AllocMedium(uint32 size, const char * filename, int line);
#else
  void * AllocMedium(uint32 size);
#endif

  void FreeMedium(void * p);
  uint32 SizeMedium(void * p);

  void InsertFreeBlock(FreeBlock * block);

protected:

#endif // USE_APP_HEAP_SAVING_MODE

  Block dummyLargeBlock;
  SimpleStats largeStats;

#ifdef DEBUG_APP_HEAP
  void * AllocLarge(uint32 size, const char * filename, int line);
#else
  void * AllocLarge(uint32 size);
#endif

  void FreeLarge(void * p);
  uint32 SizeLarge(void * p);

  void WriteFile(FILE * f, const char * buf, uint32& freeSize);

  void WriteStats(FILE * f, uint32& freeSize);

#ifdef DEBUG_APP_HEAP
  void WriteSmallBlockHeader(FILE * f, uint32& freeSize);
  void WriteSmallBlock(FILE * f, SmallBlock * block, uint32& freeSize);
  void WriteSmallBlocks(FILE * f, uint32& freeSize);
#endif

  void WriteBlockHeader(FILE * f, int type, uint32& freeSize);
  void WriteBlock(FILE * f, Block * block, uint32& freeSize);
  void WriteBlocks(FILE * f, Block * dummyBlock, uint32& freeSize);

#ifndef USE_APP_HEAP_SAVING_MODE
  void WriteFreeBlockHeader(FILE * f, uint32& freeSize);
  void WriteFreeBlocks(FILE * f, uint32& freeSize);
#endif // USE_APP_HEAP_SAVING_MODE

protected:

  static xHeap * instance;

public:

  xHeap();
  ~xHeap();

  static xHeap * Instance(){ return instance; }
  
  static void Init();
  static void __cdecl Shutdown();

#ifdef DEBUG_APP_HEAP
  void * Alloc(uint32 size, const char * filename, int line);
#else
  void * Alloc(uint32 size);
#endif

  void Free(void * p);

#ifdef DEBUG_APP_HEAP
  void * Realloc(void * p, uint32 size, const char * filename, int line);
#else
  void * Realloc(void * p, uint32 size);
#endif

  uint32 Size(void * p);

  uint32 PageSize() const;
  // void SetPageSize(uint32 value);

  void DumpUsage(const char * filename);

  void GetStats(Stats& smallStats, Stats& mediumStats, Stats& largeStats);

  struct SummaryStats: public Stats
  {
#ifdef DEBUG_APP_HEAP
    uint32 minSmallBlockDataSize;
    uint32 maxSmallBlockDataSize;
    
    uint32 minMediumBlockDataSize;
    uint32 maxMediumBlockDataSize;
    
    uint32 minLargeBlockDataSize;
    uint32 maxLargeBlockDataSize;
#endif
  };

  void GetStats(SummaryStats& stats);
  
  void CheckMemory();
};

#endif // __X_HEAP_H__
