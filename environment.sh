#!/usr/bin/env bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export FREERTOS_KERNEL_PATH=${SCRIPT_DIR}/external/FreeRTOS-Kernel
export PICO_SDK_PATH=${SCRIPT_DIR}/external/pico-sdk
export CORE_MQTT_PATH=${SCRIPT_DIR}/external/coreMQTT
export CORE_MQTT_AGENT_PATH=${SCRIPT_DIR}/external/coreMQTT-Agent
export JSON_MAKER_PATH=${SCRIPT_DIR}/external/json-maker
export TINY_JSON_PATH=${SCRIPT_DIR}/external/tiny-json
