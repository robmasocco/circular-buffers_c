/* Roberto Masocco
 * Creation Date: 28/7/2019
 * Latest Version: 30/7/2019
 * ----------------------------------------------------------------------------
 * This file contains functions to manage the Circular Buffer data structure.
 * See the header file for a general description of the structure.
 * See each function's comments for a description of their behaviour.
 */
/* This code is released under the MIT license.
 * See the attached LICENSE file.
 */

#include <stdlib.h>
#include <string.h>

#include "CircularBuffer.h"

/* Creates a new Circular Buffer of the specified size. */
CircBuffer *createCBuffer(ulong cbSize) {
    // Sanity check.
    if (cbSize == 0) return NULL;
    // Allocate memory for the new structure's metadata and data area.
    CircBuffer *buffer = calloc(1, sizeof(CircBuffer));
    if (buffer == NULL) return NULL;  // calloc failed.
    void **dataArea = calloc(cbSize, sizeof(void *));
    if (dataArea == NULL) {
        // calloc failed.
        free(buffer);
        return NULL;
    }
    // Set up the new structure.
    buffer->cbSize = cbSize;
    buffer->_dataPtr = dataArea;
    buffer->_readPtr = dataArea;
    buffer->_writePtr = dataArea;
    buffer->dataCount = 0;
    return buffer;
}

/* Deletes a Circular Buffer. */
void deleteCBuffer(CircBuffer *cBuff, int toFree) {
    if (cBuff == NULL) return;
    if (toFree)
        // If requested, free all the entries before destroying the structure.
        for (ulong i = 0; i < cBuff->cbSize; i++)
            free((cBuff->_dataPtr)[i]);
    free(cBuff->_dataPtr);
    free(cBuff);
}

/* Reads an entry from the given buffer. Also makes such entry unavailable.
 * Returns the entry or NULL.
 */
void *cbRead(CircBuffer *cBuff) {
    if (cBuff == NULL) return NULL;  // Sanity check.
    if (cBuff->dataCount == 0) return NULL;  // Empty buffer.
    // Now, the read pointer points to the next available data.
    void *newData = *(cBuff->_readPtr);
    *(cBuff->_readPtr) = NULL;
    cBuff->dataCount--;
    // Now, advance and eventually wrap around the pointer.
    cBuff->_readPtr++;
    if (cBuff->_readPtr == (cBuff->_dataPtr + cBuff->cbSize))
        cBuff->_readPtr = cBuff->_dataPtr;
    return newData;
}

/* Writes an entry in the given buffer.
 * Returns 1 on success, 0 if the buffer was full.
 */
int cbWrite(CircBuffer *cBuff, void *data) {
    if ((cBuff == NULL) || (data == NULL)) return 0;  // Sanity check.
    if (cBuff->dataCount == cBuff->cbSize) return 0;  // Full buffer.
    // Now, the write pointer points to the next available location.
    *(cBuff->_writePtr) = data;
    cBuff->dataCount++;
    // Now, advance and eventually wrap around the pointer.
    cBuff->_writePtr++;
    if (cBuff->_writePtr == (cBuff->_dataPtr + cBuff->cbSize))
        cBuff->_writePtr = cBuff->_dataPtr;
    return 1;
}

/* Reads a portion of the buffer, placing it in the provided area.
 * Can be instructed to only perform the operation if there's that amount of
 * data to read if "upTo=0".
 * Returns the number of read operations performed.
 */
ulong cbCopy(CircBuffer *cBuff, void **dataBuf, ulong bufSize, int upTo) {
    // Sanity checks.
    if ((dataBuf == NULL) || (bufSize == 0) || (cBuff == NULL)) return 0;
    if (!upTo && (bufSize > cBuff->cbSize)) return 0;
    // Check operation requirements.
    if (!cBuff->dataCount || (!upTo && (cBuff->dataCount < bufSize))) return 0;
    // Set the number of operations to do.
    ulong ops;
    if (!upTo) ops = bufSize;
    else ops = cBuff->dataCount >= bufSize ? bufSize : cBuff->dataCount;
    // Read data from the buffer.
    if (((cBuff->_dataPtr + cBuff->cbSize) - cBuff->_readPtr) < ops) {
        // Two separate reads must be done to correctly wrap the pointer
        // around the buffer.
        ulong toEnd = (cBuff->_dataPtr + cBuff->cbSize) - cBuff->_readPtr;
        memcpy(dataBuf, cBuff->_readPtr, toEnd * sizeof(void *));
        memset(cBuff->_readPtr, 0, toEnd * sizeof(void *));
        cBuff->_readPtr = cBuff->_dataPtr;
        memcpy(dataBuf + toEnd, cBuff->_readPtr,
                (ops - toEnd) * sizeof(void *));
        memset(cBuff->_readPtr, 0, (ops - toEnd) * sizeof(void *));
        cBuff->_readPtr += (ops - toEnd);
    } else {
        // All reads can be done in one go.
        memcpy(dataBuf, cBuff->_readPtr, ops * sizeof(void *));
        memset(cBuff->_readPtr, 0, ops * sizeof(void *));
        cBuff->_readPtr += ops;
        if (cBuff->_readPtr == (cBuff->_dataPtr + cBuff->cbSize))
            cBuff->_readPtr = cBuff->_dataPtr;
    }
    cBuff->dataCount -= ops;
    return ops;
}

/* Writes a block of data into the buffer.
 * Can be instructed to only write to the buffer if there's enough room for
 * all the data or up to the given amount if "upTo=1".
 * Returns the number of write operations performed.
 */
ulong cbPaste(CircBuffer *cBuff, void **dataBuf, ulong bufSize, int upTo) {
    // Sanity checks.
    if ((dataBuf == NULL) || (bufSize == 0) || (cBuff == NULL)) return 0;
    if (!upTo && (bufSize > cBuff->cbSize)) return 0;
    ulong freeCells = cBuff->cbSize - cBuff->dataCount;
    // Check operation requirements.
    if (!freeCells || (!upTo && (freeCells < bufSize))) return 0;
    // Set the number of operations to do.
    ulong ops;
    if (!upTo) ops = bufSize;
    else ops = freeCells >= bufSize ? bufSize : freeCells;
    // Write data to the buffer.
    if (((cBuff->_dataPtr + cBuff->cbSize) - cBuff->_writePtr) < ops) {
        // Two separate writes must be done to correctly wrap the pointer
        // around the buffer.
        ulong toEnd = (cBuff->_dataPtr + cBuff->cbSize) - cBuff->_writePtr;
        memcpy(cBuff->_writePtr, dataBuf, toEnd * sizeof(void *));
        cBuff->_writePtr = cBuff->_dataPtr;
        memcpy(cBuff->_writePtr, dataBuf + toEnd,
               (ops - toEnd) * sizeof(void *));
        cBuff->_writePtr += (ops - toEnd);
    } else {
        // All writes can be done in one go.
        memcpy(cBuff->_writePtr, dataBuf, ops * sizeof(void *));
        cBuff->_writePtr += ops;
        if (cBuff->_writePtr == (cBuff->_dataPtr + cBuff->cbSize))
            cBuff->_writePtr = cBuff->_dataPtr;
    }
    cBuff->dataCount += ops;
    return ops;
}
