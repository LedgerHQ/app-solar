################################################################################
# Moldule Description                                                          #
################################################################################

project(UiUtils
  VERSION 1.1.3
  DESCRIPTION "UI utils of the Solar app"
  LANGUAGES C)

################################################################################
# Moldule Source Files                                                         #
################################################################################

set(LEDGER_LIB_STANDARD_SRC "$ENV{BOLOS_SDK}/lib_standard_app/format.c")

set(SOLAR_UI_UTILS_SRC "${SOLAR_SOURCE_DIR}/ui/ui_utils.c")

add_library(ui_utils "${LEDGER_LIB_STANDARD_SRC}" "${SOLAR_UI_UTILS_SRC}")

################################################################################
# Module Include Directories                                                   #
################################################################################

target_include_directories(ui_utils PUBLIC
  "$ENV{BOLOS_SDK}/include"
  "$ENV{BOLOS_SDK}/lib_standard_app"
  "${SOLAR_SOURCE_DIR}")
