#ifndef __GRAD_DESC_LOCALIZATION_H__
#define __GRAD_DESC_LOCALIZATION_H__

#include "shell.h"
#include "contiki.h"
#include "dev/leds.h"
#include "dev/serial-line.h"
#include <stdio.h>
#include <stdlib.h>
#include "node-id.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "sky-transmission.h"
#include "sky-math.h"
#include "global-sensor.h"
#include "net/rime.h"
#include <string.h>

void shell_grad_desc_localization_init();

struct history_entry
{
	struct history_entry *next;
	rimeaddr_t addr;
	uint8_t seq;
};

#endif /* __GRAD_DESC_LOCALIZATION_H__ */
