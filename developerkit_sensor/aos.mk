NAME := developerkit_sensor

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := developerkit_sensor

$(NAME)_SOURCES += app_main.c linkkit_client.c

$(NAME)_COMPONENTS += linkkit_sdk_c netmgr cjson cli

GLOBAL_INCLUDES += ./
$(NAME)_COMPONENTS_CUSTOMIZED := ulog cli cjson libiot_wrappers netmgr rhino libiot_infra libiot_devmodel yloop uagent sensor newlib_stub
$(NAME)_COMPONENTS += $($(NAME)_COMPONENTS_CUSTOMIZED)

