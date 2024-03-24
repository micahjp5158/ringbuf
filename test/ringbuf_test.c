/* =========================================================================
    Unity - A Test Framework for C
    ThrowTheSwitch.org
    Copyright (c) 2007-24 Mike Karlesky, Mark VanderVoord, & Greg Williams
    SPDX-License-Identifier: MIT
========================================================================= */


#include "ringbuf.h"
#include "unity.h"

#include <string.h>

#define BUF_SIZE 8
uint8_t buf[BUF_SIZE];
RingBuf_Handle_t ringbuf_handle;

void setUp(void)
{
  /* This is run before EACH TEST */
  memset(&ringbuf_handle, 0, sizeof(RingBuf_Handle_t));
  memset(buf, 0, BUF_SIZE);
}

void tearDown(void)
{
}

/**
 * @brief Verifies the ring buffer initialization behaves as expected.
 ********************************************************************************/
void test_ringbuf_init(void)
{
  RingBuf_Status_t ringbuf_status;

  /* Initialize the ring buffer */
  ringbuf_status = ringbuf_init(&ringbuf_handle, buf, sizeof(uint8_t), BUF_SIZE);
  TEST_ASSERT_EQUAL(RINGBUF_STATUS_OK, ringbuf_status);

  /* Verify all ringbuf struct members are initialized as expected */
  TEST_ASSERT_EQUAL(&buf, ringbuf_handle.buf);
  TEST_ASSERT_EQUAL(sizeof(uint8_t), ringbuf_handle.elem_size);
  TEST_ASSERT_EQUAL(BUF_SIZE, ringbuf_handle.buf_size);
  TEST_ASSERT_EQUAL(0, ringbuf_handle.head);
  TEST_ASSERT_EQUAL(0, ringbuf_handle.tail);
  TEST_ASSERT_EQUAL(0, ringbuf_handle.num_elements);
}

/**
 * @brief Verifies that writing an element to the ring buffer behaves as expected.
 ********************************************************************************/
void test_ringbuf_put(void)
{
  RingBuf_Status_t ringbuf_status;

  /* Initialize the ring buffer */
  ringbuf_status = ringbuf_init(&ringbuf_handle, buf, sizeof(uint8_t), BUF_SIZE);
  TEST_ASSERT_EQUAL(RINGBUF_STATUS_OK, ringbuf_status);

  /* Put a character in the ring buffer */
  uint8_t data = 0x01;
  ringbuf_status = ringbuf_put(&ringbuf_handle, &data);

  /* Verify the tail and number of elements increase */
  TEST_ASSERT_EQUAL(1, ringbuf_handle.tail);
  TEST_ASSERT_EQUAL(1, ringbuf_handle.num_elements);
}

/**
 * @brief Verifies that reading an element from the ring buffer behaves as
 *        expected.
 ********************************************************************************/
void test_ringbuf_get(void)
{
  RingBuf_Status_t ringbuf_status;

  /* Initialize the ring buffer */
  ringbuf_status = ringbuf_init(&ringbuf_handle, buf, sizeof(uint8_t), BUF_SIZE);
  TEST_ASSERT_EQUAL(RINGBUF_STATUS_OK, ringbuf_status);

  /* Put a character in the ring buffer */
  uint8_t put_data = 0x01;
  ringbuf_status = ringbuf_put(&ringbuf_handle, &put_data);
  TEST_ASSERT_EQUAL(RINGBUF_STATUS_OK, ringbuf_status);

  /* Verify the tail and number of elements increase */
  TEST_ASSERT_EQUAL(1, ringbuf_handle.tail);
  TEST_ASSERT_EQUAL(1, ringbuf_handle.num_elements);

  /* Read the character back from the buffer */
  uint8_t get_data = 0x00;
  ringbuf_status = ringbuf_get(&ringbuf_handle, &get_data);
  TEST_ASSERT_EQUAL(RINGBUF_STATUS_OK, ringbuf_status);

  /* Verify the read data matches what was inserted */
  TEST_ASSERT_EQUAL(put_data, get_data);

  /* Verify the head increased and number of elements decreased */
  TEST_ASSERT_EQUAL(1, ringbuf_handle.head);
  TEST_ASSERT_EQUAL(0, ringbuf_handle.num_elements);
}

/**
 * @brief Verifies that writing to the ring buffer while it is full returns the
 *        appropriate error code.
 ********************************************************************************/
void test_ringbuf_put_while_full(void)
{
  RingBuf_Status_t ringbuf_status;
  uint8_t data;

  /* Initialize the ring buffer */
  ringbuf_status = ringbuf_init(&ringbuf_handle, buf, sizeof(uint8_t), BUF_SIZE);
  TEST_ASSERT_EQUAL(RINGBUF_STATUS_OK, ringbuf_status);

  /* Fill the ring buffer */
  for (int i = 0; i < BUF_SIZE; i++)
  {
    data = i;
    ringbuf_status = ringbuf_put(&ringbuf_handle, &data);
    TEST_ASSERT_EQUAL(RINGBUF_STATUS_OK, ringbuf_status);
  }

  /* Verify the buffer is full and tail has wrapped around back to the start */
  TEST_ASSERT_EQUAL(0, ringbuf_handle.tail);
  TEST_ASSERT_EQUAL(BUF_SIZE, ringbuf_handle.num_elements);

  /* Verify that writing to the ringbuf while full returns an error */
  data = BUF_SIZE;
  ringbuf_status = ringbuf_put(&ringbuf_handle, &data);
  TEST_ASSERT_EQUAL(RINGBUF_STATUS_FULL, ringbuf_status);
}

/**
 * @brief Verifies that reading from the ring buffer while it is empty returns the
 *        appropriate error code.
 ********************************************************************************/
void test_ringbuf_get_while_empty(void)
{
  RingBuf_Status_t ringbuf_status;
  uint8_t data;

  /* Initialize the ring buffer */
  ringbuf_status = ringbuf_init(&ringbuf_handle, buf, sizeof(uint8_t), BUF_SIZE);
  TEST_ASSERT_EQUAL(RINGBUF_STATUS_OK, ringbuf_status);

  /* Verify the buffer is empty */
  TEST_ASSERT_EQUAL(0, ringbuf_handle.num_elements);

  /* Verify that reading from an empty buffer returns an error */
  ringbuf_status = ringbuf_get(&ringbuf_handle, &data);
  TEST_ASSERT_EQUAL(RINGBUF_STATUS_EMPTY, ringbuf_status);
}

/**
 * @brief Verifies that writing to and reading from the ring buffer handles
 *        looping the head / tail to the beginning of the buffer once the end
 *        is reached as expected.
 ********************************************************************************/
void test_ringbuf_loop_when_needed(void)
{
  RingBuf_Status_t ringbuf_status;
  uint8_t data;

  /* Initialize the ring buffer */
  ringbuf_status = ringbuf_init(&ringbuf_handle, buf, sizeof(uint8_t), BUF_SIZE);
  TEST_ASSERT_EQUAL(RINGBUF_STATUS_OK, ringbuf_status);

  /* Fill the ring buffer half way*/
  for (int i = 0; i < BUF_SIZE/2; i++)
  {
    data = i;
    ringbuf_status = ringbuf_put(&ringbuf_handle, &data);
    TEST_ASSERT_EQUAL(RINGBUF_STATUS_OK, ringbuf_status);
  }

  /* Verify the buffer is halfway filled */
  TEST_ASSERT_EQUAL(BUF_SIZE/2, ringbuf_handle.tail);
  TEST_ASSERT_EQUAL(BUF_SIZE/2, ringbuf_handle.num_elements);

  /* Read all characters in the buffer */
  for (int i = 0; i < BUF_SIZE/2; i++)
  {
    ringbuf_status = ringbuf_get(&ringbuf_handle, &data);
    TEST_ASSERT_EQUAL(RINGBUF_STATUS_OK, ringbuf_status);
    TEST_ASSERT_EQUAL(i, data);
  }

  /* Verify the buffer is empty */
  ringbuf_status = ringbuf_get(&ringbuf_handle, &data);
  TEST_ASSERT_EQUAL(RINGBUF_STATUS_EMPTY, ringbuf_status);
  TEST_ASSERT_EQUAL(0, ringbuf_handle.num_elements);

  /* Verify the head / tail are in the center of the buffer */
  TEST_ASSERT_EQUAL(BUF_SIZE/2, ringbuf_handle.head);

  /* Verify the head and tail ended at the same index */
  TEST_ASSERT_EQUAL(ringbuf_handle.head, ringbuf_handle.tail);

  /* Fill the ring buffer */
  for (int i = 0; i < BUF_SIZE; i++)
  {
    data = i;
    ringbuf_status = ringbuf_put(&ringbuf_handle, &data);
    TEST_ASSERT_EQUAL(RINGBUF_STATUS_OK, ringbuf_status);
  }

  /* Verify the buffer is full */
  data = BUF_SIZE;
  ringbuf_status = ringbuf_put(&ringbuf_handle, &data);
  TEST_ASSERT_EQUAL(RINGBUF_STATUS_FULL, ringbuf_status);
  TEST_ASSERT_EQUAL(BUF_SIZE, ringbuf_handle.num_elements);

  /* Verify the tail pointer correctly circled back to the start of the buffer when needed */
  TEST_ASSERT_EQUAL(BUF_SIZE/2, ringbuf_handle.tail);

  /* Read all characters in the buffer */
  for (int i = 0; i < BUF_SIZE; i++)
  {
    ringbuf_status = ringbuf_get(&ringbuf_handle, &data);
    TEST_ASSERT_EQUAL(RINGBUF_STATUS_OK, ringbuf_status);
    TEST_ASSERT_EQUAL(i, data);
  }

  /* Verify the buffer is empty */
  ringbuf_status = ringbuf_get(&ringbuf_handle, &data);
  TEST_ASSERT_EQUAL(RINGBUF_STATUS_EMPTY, ringbuf_status);
  TEST_ASSERT_EQUAL(0, ringbuf_handle.num_elements);

  /* Verify the head pointer correctly circled back to the start of the buffer when needed */
  TEST_ASSERT_EQUAL(BUF_SIZE/2, ringbuf_handle.head);

  /* Verify the head and tail ended at the same index */
  TEST_ASSERT_EQUAL(ringbuf_handle.head, ringbuf_handle.tail);
}