/**
 ********************************************************************************
 * @file    ringbuf->c
 * @author  micahjp5158
 * @date    3-23-2024
 * @brief   Ring buffer implementation in C
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "ringbuf.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/

/************************************
 * PRIVATE TYPEDEFS
 ************************************/

/************************************
 * STATIC VARIABLES
 ************************************/

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/

/************************************
 * STATIC FUNCTIONS
 ************************************/

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
RingBuf_Status_t ringbuf_init(RingBuf_Handle_t *ringbuf, void *buf, size_t elem_size, size_t buf_size)
{
  /* Verify the buffer exists */
  if (buf == NULL)
  {
    return RINGBUF_STATUS_NULL_PTR;
  }

  /* Clear the buffer */
  memset(buf, 0, ringbuf->elem_size * ringbuf->buf_size);

  /* Set / initialize ringbuffer struct members */
  ringbuf->buf = buf;
  ringbuf->elem_size = elem_size;
  ringbuf->buf_size = buf_size;
  ringbuf->head = 0;
  ringbuf->tail = 0;
  ringbuf->num_elements = 0;

  return RINGBUF_STATUS_OK;
}

RingBuf_Status_t ringbuf_get(RingBuf_Handle_t *ringbuf, void *data){
  /* Make sure buffer is not empty */
  if (ringbuf->num_elements == 0)
  {
    return RINGBUF_STATUS_EMPTY;
  }

  /* Copy data from current head to the destination */
  memcpy(data, ((uint8_t *)ringbuf->buf + (ringbuf->head * ringbuf->elem_size)), ringbuf->elem_size);

  /* If head is at final element, go to beginning */
  /* Othersize, wrap back to the beginning */
  if(ringbuf->head == ringbuf->buf_size)
  {
    ringbuf->head = 0;
  } else {
    ringbuf->head++;
  }

  /* Decrease number of elements */
  ringbuf->num_elements--;

  return RINGBUF_STATUS_OK;
}

RingBuf_Status_t ringbuf_put(RingBuf_Handle_t *ringbuf, const void *data)
{
  /* Make sure buffer is not full */
  if (ringbuf->num_elements >= ringbuf->buf_size)
  {
    return RINGBUF_STATUS_FULL;
  }

  /* Copy the data into the buffer at the new tail position */
  memcpy(((uint8_t *)ringbuf->buf + (ringbuf->tail * ringbuf->elem_size)), data, ringbuf->elem_size);

  /* If tail is at final element, go to beginning */
  /* Othersize, wrap back to the beginning */
  if(ringbuf->tail >= ringbuf->buf_size - 1)
  {
    ringbuf->tail = 0;
  } else {
    ringbuf->tail++;
  }

  /* Increase number of elements */
  ringbuf->num_elements++;

  return RINGBUF_STATUS_OK;
}

RingBuf_Status_t ringbuf_clear(RingBuf_Handle_t *ringbuf)
{
  /* Reset head and tail position */
  ringbuf->head = ringbuf->tail;

  /* Set number of elements to 0 */
  ringbuf->num_elements = 0;

  return RINGBUF_STATUS_OK;
}
