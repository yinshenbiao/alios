## Contents

```sh
developerkit_sensor
├── developerkit_sensor.c # developerkit_sensor source code
├── Config.in       # kconfig file
├── aos.mk          # aos build system file(for make)
└── k_app_config.h  # aos app config file
```

## Introduction

The **developerkit_sensor** ...

### Dependencies

### Supported Boards

- developerkit

### Build

```sh
# generate developerkit_sensor@developerkit default config
aos make developerkit_sensor@developerkit -c config

# build
aos make
```
