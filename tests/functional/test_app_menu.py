from ragger.firmware import Firmware
from ragger.navigator import NavInsID


# Verify the behaviour of the device's main menu.
def test_app_menu_main(firmware, navigator, test_name, default_screenshot_path):
    instructions = []

    # Navigate in the main menu
    if firmware.device.startswith("nano"):
        instructions = [
            NavInsID.RIGHT_CLICK,
            NavInsID.RIGHT_CLICK,
            NavInsID.RIGHT_CLICK,
            NavInsID.RIGHT_CLICK,
        ]
    # STAX and FLEX will use the default empty list

    navigator.navigate_and_compare(
        default_screenshot_path,
        test_name,
        instructions,
        screen_change_before_first_instruction=False,
    )


# Verify the behaviour of the device's info menu.
def test_app_menu_info(firmware, navigator, test_name, default_screenshot_path):
    instructions = []

    # Navigate in the main menu
    if firmware.device.startswith("nano"):
        instructions = [
            NavInsID.RIGHT_CLICK,
            NavInsID.RIGHT_CLICK,
            NavInsID.BOTH_CLICK,
            NavInsID.RIGHT_CLICK,
            NavInsID.BOTH_CLICK,
        ]
    elif firmware is Firmware.STAX or firmware is Firmware.FLEX:
        instructions = [
            NavInsID.USE_CASE_HOME_INFO,
            NavInsID.USE_CASE_SETTINGS_MULTI_PAGE_EXIT,
        ]

    navigator.navigate_and_compare(
        default_screenshot_path,
        test_name,
        instructions,
        screen_change_before_first_instruction=False,
    )
