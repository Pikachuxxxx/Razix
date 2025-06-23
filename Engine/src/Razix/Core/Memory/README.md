# RazixMemory
Custom CPU memory management library for [Razix Engine](https://github.com/Pikachuxxxx/Razix). Razix Memory provides with custom operators for `new` and `delete` as well as custom implementation of `malloc` and `free`. It also provides efficient heap management and tracking to reduce fragmentation as much as possible and provide extremely fast allocation. It also focuses on improving cache line efficiency reducing cache misses. It also provides detailed tracking that will be later used with Razix Memory Visualization tool in debug mode.

#### **This is being built as a standalone library that can be used anywhere.**

Check the Razix Engine Docs and design notes to know about allocation/API design details.

It will also act as a base for custom rzstl::Allocator and RZSTL.

- Razix uses custom malloc and free + overrides new and delete operators
- We use the kernel functions with a allocation strategy to manage the memory
- First stage of memory pools are seperated by their alignments, hence same aligned data are placed one place ?????? IDK man
- However one needs to consider not only that the data is packed properly but also is together(algiment vs distance of allocation needs to-go hand in hand)
- Hence strategy changes so as to keep aligned/un-aligned data not completely in separate pools but keep them together somehow

## Goals

The following sections will be updated in design notes and in wiki as the project moves forward.

- [ ] **Custom Operators and Memory allocation Functions + Macros**

- [ ] **Heap Strategies + Tracking**

- [ ] **Reducing Fragmentation + tracking**

- [ ] **L1, L2, L3 cache efficiency**

- [ ] **Visualization Tool API**

- [ ] **GPU Interop API to interface with DXMA, VMA etc.**

## References
1. [What Every Programmer Should Know About Memory](https://people.freebsd.org/~lstewart/articles/cpumemory.pdf) - :warning:IMPORTANT!!!!!:warning:
2. [Intel® 64 and IA-32 Architectures Software Developer’s Manual Volume 3A: System Programming Guide, Part 1 // CHAPTER 11 : MEMORY CACHE CONTROL](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-3a-part-1-manual.pdf)
3. [How do `malloc()` and `free()` work?](https://stackoverflow.com/questions/1119134/how-do-malloc-and-free-work)
4. [GNU C library Virtual Memory Allocation And Paging](https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_chapter/libc_3.html)
5. [How to allocate full memory pages](https://stackoverflow.com/questions/43352445/how-to-allocate-full-memory-pages)
6. [CppCon 2014: Mike Acton "Data-Oriented Design and C++"](https://www.youtube.com/watch?v=rX0ItVEVjHc) - Golden reference for Design + Optimizations
7. Jason Gregory Game Engine Architecture - Memory and Fragmentation
8. [malloc using sys calls Implementation example](https://github.com/miguelperes/custom-malloc/blob/master/mymemory.c)
9. [Free-Space Management - University of Wisconsin](https://pages.cs.wisc.edu/~remzi/OSTEP/vm-freespace.pdf)
10. [TLSF - 2 level segregation firt allocator](http://www.gii.upv.es/tlsf/files/ecrts04_tlsf.pdf)
11. [tcmalloc - Google](https://google.github.io/tcmalloc/)
