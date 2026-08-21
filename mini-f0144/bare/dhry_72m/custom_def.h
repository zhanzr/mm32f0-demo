#ifndef __CUSTOM_DEF_H__
#define __CUSTOM_DEF_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "hal_conf.h"

#ifndef configTICK_RATE_HZ
#define configTICK_RATE_HZ  1000
#endif

#define SERIAL_BAUDRATE 115200

#endif /* __CUSTOM_DEF_H__ */