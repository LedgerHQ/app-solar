from pathlib import Path

from ragger.backend import BackendInterface
from ragger.navigator import Navigator, NavInsID


# Verify the behaviour of the device's main menu.
def test_app_menu_main(
    backend: BackendInterface,
    navigator: Navigator,
    test_name: str,
    default_screenshot_path: Path,
):
    instructions = []

    device = backend.device
    # Navigate in the main menu
    if device.is_nano:
        instructions = [
            NavInsID.RIGHT_CLICK,
            NavInsID.RIGHT_CLICK,
            NavInsID.RIGHT_CLICK,
            NavInsID.RIGHT_CLICK,
        ]
    # STAX, FLEX and APEX_P will use the default empty list

    navigator.navigate_and_compare(
        default_screenshot_path,
        test_name,
        instructions,
        screen_change_before_first_instruction=False,
    )


# Verify the behaviour of the device's info menu.
def test_app_menu_info(
    backend: BackendInterface,
    navigator: Navigator,
    test_name: str,
    default_screenshot_path: Path,
):
    instructions = []

    device = backend.device
    # Navigate in the main menu
    if device.is_nano:
        instructions = [
            NavInsID.RIGHT_CLICK,
            NavInsID.RIGHT_CLICK,
            NavInsID.BOTH_CLICK,
            NavInsID.RIGHT_CLICK,
            NavInsID.BOTH_CLICK,
        ]
    else:
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
