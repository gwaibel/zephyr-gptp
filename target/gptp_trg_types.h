/*
 * Copyright (c) 2020, Sensor-Technik Wiedemann GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GPTP_TRG_TYPES_H
#define GPTP_TRG_TYPES_H

/*
 * This header provides the standard types for the gptp stack.
 * This file shall be deployed with the gptp API headers.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* And here some common declarations */

#ifndef BIT
 #define BIT(n)                (1UL << (n))
#endif

#ifndef CONTAINER_OF
 #define CONTAINER_OF(ptr, type, field) ((type *)(((char *)(ptr)) - offsetof(type, field)))
#endif

#ifndef __packed
 #define __packed             __attribute__((__packed__))
#endif

#endif