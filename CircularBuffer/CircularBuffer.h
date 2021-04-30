/* Roberto Masocco
 * Creation Date: 28/7/2019
 * Latest Version: 30/7/2019
 * ----------------------------------------------------------------------------
 * This file contains type definitions and declarations for the Circular Buffer
 * data structure. See the source file for a brief description of what each
 * function does.
 * This data structure is meant to be used, in both reading and writing, as a
 * "circle", meaning that even if it is a single array in memory, it keeps
 * track (with internal pointers) of the cells which correspond to the start
 * of the new data ("read" pointer), and of the old data ("write" pointer).
 * Such pointers are incremented with the possibility to "wrap around" the
 * ends of the array, thus making it a circular structure.
 * Methods provided allow for structure creation, indexing, writing and
 * deletion.
 * Data entered in the cells can be of any type that fits into a "void *"
 * (usually 64 bits on x86_64).
 * This structure is intended as FIFO, so it doesn't allow old data to be
 * overwritten; routines always return the amount of data that they were able
 * to write, or read.
 * This library uses dynamic memory allocation in the heap, since the whole
 * structure with its metadata is created there when requested, and freed as
 * such. Options are provided to free elements too upon structure deletion.
 */
/* This code is released under the MIT license.
 * See the attached LICENSE file.
 */

#ifndef CIRCBUF_H
#define CIRCBUF_H

/* WARNING: In order to guarantee efficiency in memory usage and speed, this
 * library reads and writes blocks of bytes as long as a "void *" (8 bytes on
 * x86_64). Thus, make sure you can handle eventual casts for "read" and "write"
 * operations, and provide actual "void *" arrays to "copy" and "paste".
 * If in doubt while using types of different length, additional handling of
 * the single bytes prior to writing and after reading is suggested.
 * If you'd like to account for the eventual overhead caused by unused bytes
 * in each "void *", you'll have to modify the library to suit your needs.
 */

#include <sys/types.h>

/* A circular buffer is made of a pointer to a data area, its length, and a
 * couple more pointers to the start of the new and old data respectively.
 * Such pointers are generated and managed by the various methods.
 * A counter of the valid entries is also made available.
 */
typedef struct {
    void **_dataPtr;
    ulong cbSize;
    void **_readPtr;
    void **_writePtr;
    ulong dataCount;
} CircBuffer;

CircBuffer *createCBuffer(ulong cbSize);
void deleteCBuffer(CircBuffer *cBuff, int toFree);
void *cbRead(CircBuffer *cBuff);
int cbWrite(CircBuffer *cBuff, void *data);
ulong cbCopy(CircBuffer *cBuff, void **dataBuf, ulong bufSize, int upTo);
ulong cbPaste(CircBuffer *cBuff, void **dataBuf, ulong bufSize, int upTo);

#endif
