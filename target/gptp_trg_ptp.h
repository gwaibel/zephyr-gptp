/*
 * Copyright (c) 2020, Sensor-Technik Wiedemann GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GPTP_TRG_PTP_H
#define GPTP_TRG_PTP_H

#include "gptp_trg_types.h"
#include "net/ptp_time.h"



struct device {
	const char *name;
	const void *config_info;
	const void *driver_api;
	void * const driver_data;
};


/**
 * @brief Get the time of the PTP clock.
 *
 * @param dev PTP clock device
 * @param tm Where to store the current time.
 *
 * @return 0 if ok, <0 if error
 */
int ptp_clock_get(struct device *dev, struct net_ptp_time *tm);

/**
 * @brief Set the time of the PTP clock.
 *
 * @param dev PTP clock device
 * @param tm Time to set
 *
 * @return 0 if ok, <0 if error
 */
int ptp_clock_set(struct device *dev, struct net_ptp_time *tm);

/**
 * @brief Adjust the PTP clock time change rate when compared to its neighbor.
 *
 * @param dev PTP clock device
 * @param rate Rate of the clock time change
 *
 * @return 0 if ok, <0 if error
 */
int ptp_clock_rate_adjust(struct device *dev, float rate);

/**
 * @brief Adjust the PTP clock time.
 *
 * @param dev PTP clock device
 * @param increment Increment of the clock in nanoseconds
 *
 * @return 0 if ok, <0 if error
 */
int ptp_clock_adjust(struct device *dev, int increment);


#endif