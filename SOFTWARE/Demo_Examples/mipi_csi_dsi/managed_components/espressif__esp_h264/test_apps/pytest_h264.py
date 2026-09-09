# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded_idf import IdfDut

@pytest.mark.esp32s3
@pytest.mark.esp32p4
def test_h264_hw(dut: IdfDut) -> None:
    dut.run_all_single_board_cases()
