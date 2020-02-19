/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __APP_ENTRY_H__
#define __APP_ENTRY_H__

#include "aos/kernel.h"

int wifi_start(const char *ssid, const char *passwd);

void relay_gpio_init(void);
void relay_gpio_set(uint8_t status);

void linkkit_connect(const char *pk, const char *dn, const char *ds, const char *ps, int (*callback)(const int, const char *, const int), int (*callback2)(void));
void linkkit_start(void);
int linkkit_set_props(unsigned char *payload, int payload_len);

#endif