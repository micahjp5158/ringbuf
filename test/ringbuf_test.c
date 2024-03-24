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