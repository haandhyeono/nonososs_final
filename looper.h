/******************************************************************************
 *
 * COPYRIGHT 2017 ASAHI KASEI MICRODEVICES CORPORATION ("AKM")
 * All Rights Reserved.
 *
 * This software is licensed to you under the Apache License, Version 2.0
 * (http://www.apache.org/licenses/LICENSE-2.0) except for using, copying,
 * modifying, merging, publishing and/or distributing in combination with
 * AKM's Proprietary Software defined below. 
 *
 * "Proprietary Software" means the software and its related documentations
 * which AKM will provide only to those who have entered into the commercial
 * license agreement with AKM separately. If you wish to use, copy, modify,
 * merge, publish and/or distribute this software in combination with AKM's
 * Proprietary Software, you need to request AKM to enter into such agreement
 * and grant commercial license to you.
 *
 ******************************************************************************/
#ifndef INCLUDE_LOOPER_H
#define INCLUDE_LOOPER_H

#include "AKL_APIs.h"

/*!
 * \brief A main function of measurement sequence.
 *
 * \param prm A pointer to #AKL_SCL_PRMS struct.
 *
 * \return
 */
int16_t measurement_loop(
    struct AKL_SCL_PRMS *prm
);

void interrupt(
    void
);

#endif /* INCLUDE_LOOPER_H */
