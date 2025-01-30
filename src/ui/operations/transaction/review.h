#pragma once

#include <stdint.h>  // uint*_t

#include <nbgl_content.h>  // nbgl_content_t

/* -------------------------------------------------------------------------- */

#define REVIEW_PAGE_COUNT 3  // review intent page, tx info page, sign page

/* -------------------------------------------------------------------------- */

void review_content_provider(uint8_t page, nbgl_content_t *content);
