#include <stddef.h>

class MemoryManager
{
private:
    struct FreeStore
    {
        FreeStore *next;
    };
    void expandPoolSize();
    void cleanUp();
    FreeStore* freeStoreHead;

public:
    MemoryManager()
    {
        freeStoreHead = nullptr;
        expandPoolSize();
    };
    virtual ~MemoryManager()
    {
        cleanUp();
    }
    virtual void* allocate(size_t size);
    virtual void  free(void* deleted);

};

MemoryManager gMemoryManager;

class Complex
{
public:
    Complex(double a, double b): r(a), c(b){}
    inline void* operator new (size_t size);
    inline void  operator delete (void* pointerToDekete);
private:
    double r;
    double c;
};


void* Complex::operator new (size_t size)
{
    return gMemoryManager.allocate(size);
}

void  Complex::operator delete (void* pointerToDelete)
{
    //we need to explicitly call delete or there will be memory leaks as with the usual new
    gMemoryManager.free(pointerToDelete);
}

void MemoryManager::expandPoolSize()
{
    // 1. check the minimum size needed for a memory chunk to be both a FreeStore* or Complex
    // 2. make an array of char (1 byte) of the right size and change to a FreeStore pointer
    // 3. Store FreeStoreHead pointer as the pointer to the char array
    // NOTE: reinterpret_cast takes the bits that make up is argument and uses the exact array as the bits for the cast, unlike static cast, see https://stackoverflow.com/questions/573294/when-to-use-reinterpret-cast
    // 4. set the member variable next of FreeStore to another reinterpret_cast char array for a POOLSIZE amount
    // 5. set the last next pointer to a null ptr

    size_t size = (sizeof(Complex) > sizeof(FreeStore*)) ? sizeof(Complex) : sizeof(FreeStore*);
    FreeStore* head = reinterpret_cast <FreeStore*> (new char[size]);
    freeStoreHead = head;

    for (int i =0; i < POOLSIZE; i++)
    {
        head->next = reinterpret_cast <FreeStore*> (new char[size]);
        head = head->next;
    }
    
    head->next = nullptr;

}


void MemoryManager::cleanUp()
{
    // 1. Store the head pointer as the nextPtr to delete
    // 2. incriment the head pointer
    // 3. delete the next pointer
    // 4. stop when the next pointer is a null ptr

    FreeStore* nextPtr = freeStoreHead;
    for(; nextPtr; nextPtr = freeStoreHead)
    {
        freeStoreHead = freeStoreHead->next;
        delete [] nextPtr; //remeber this was a char array
    }
}

inline void* MemoryManager::allocate(size_t size)
{
    // 1. if the free store hasnt been created, create the free-store and go to 3
    // 2. if the free store has been exhausted, create a new free-store
    // 3. Return the first element of the free-store and mark the next element of the free-store as the free-store head

    if (nullptr == freeStoreHead)
        expandPoolSize();

    FreeStore* head = freeStoreHead;
    freeStoreHead = head->next;
    return head;

}

inline void MemoryManager::free(void* deleted)
{
    // 1. Make the next field in the deleted pointer point to the current free-store head
    // 2. Mark the deleted pointer as the free store head
    // NOTE: we have a singly linked list, so if we free an element, we add the element to the pool by adding the freed element to the front of the pool and having the next pointer of the FreeStore struct point to the old front, reusing memory space

    FreeStore* head = static_cast <FreeStore*> (deleted);
    head->next = freeStoreHead;
    freeStoreHead = head;
}
