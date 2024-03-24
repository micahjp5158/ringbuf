/**
 ********************************************************************************
 * @file    ringbuf.h
 * @author  micahjp5158
 * @date    3-23-2024
 * @brief   Ring buffer implementation in C
 ********************************************************************************
 */

#ifndef __RINGBUF_H__
#define __RINGBUF_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/
#include <stdbool.h>
#include <stddef.h>

/************************************
 * MACROS AND DEFINES
 ************************************/

/************************************
 * TYPEDEFS
 ************************************/
typedef enum ringbuf_status {
  RINGBUF_STATUS_OK,
  RINGBUF_STATUS_NULL_PTR,
  RINGBUF_STATUS_BUF_SIZE_NOT_PWR_OF_2,
  RINGBUF_STATUS_EMPTY,
  RINGBUF_STATUS_FULL,
  RINGBUF_STATUS_NUM
} RingBuf_Status_t;

/* Struct members should not be access directly - only use init, get, and put handlers*/
typedef struct {
  void *buf;                    // Ringbuffer data buffer
  size_t elem_size;             // Size of one element in the buffer
  size_t buf_size;              // Number of elements in the buffer
  volatile size_t head;         // First element of the ringbuffer
  volatile size_t tail;         // Last element of the ringbuffer
  volatile size_t num_elements; // Number of elements in the ringbuffer
} RingBuf_Handle_t;

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/

/**
 * @brief Initialize a RingBuf handler with the specified buffer parameters.
 * @param ringbuf: Ringbuf handler.
 * @param buf: Pre-allocated buffer for the ringbuffer.
 * @param elem_size: Size of each element in the buffer.
 * @param buf_size: Number of elements in the buffer.
 * @return: RINGBUF_STATUS_OK if successfully created, error code otherwise.
 ********************************************************************************/
RingBuf_Status_t ringbuf_init(RingBuf_Handle_t *ringbuf, void *buf, size_t elem_size, size_t buf_size);

/**
 * @brief Get (and remove) the oldest element in the ring buffer.
 * @param ringbuf: Ringbuf handler.
 * @param data: If get is successful, the element's data will be written here.
 * @return: RINGBUF_STATUS_OK if successfully read, error code otherwise.
 * @note: Contains a critical section for the ring buffer. Assumes the caller
 *        holds the appropriate mutex or otherwise prevents race conditions.
 ********************************************************************************/
RingBuf_Status_t ringbuf_get(RingBuf_Handle_t *ringbuf, void *data);

/**
 * @brief Insert a new element to the end of a ring buffer.
 * @param ringbuf: Ringbuf handler.
 * @param data: Element to insert into the buffer.
 * @return: RINGBUF_STATUS_OK if successfully inserted, error code otherwise.
 * @note: Contains a critical section for the ring buffer. Assumes the caller
 *        holds the appropriate mutex or otherwise prevents race conditions.
 ********************************************************************************/
RingBuf_Status_t ringbuf_put(RingBuf_Handle_t *ringbuf, const void *data);

/**
 * @brief Clears all data in the ringbuffer.
 * @param ringbuf: Ringbuf handler.
 * @note: Does not delete data from the buffer, only resets the head and
 *        tail position and sets number of elements to 0.
 * @note: Contains a critical section for the ring buffer. Assumes the caller
 *        holds the appropriate mutex or otherwise prevents race conditions.
 ********************************************************************************/
RingBuf_Status_t ringbuf_clear(RingBuf_Handle_t *ringbuf);

#ifdef __cplusplus
}
#endif

#endif // __RINGBUF_H__