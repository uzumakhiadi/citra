// Copyright 2015 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

namespace Service {

class Interface;

namespace CECD {

enum class CecStateAbbreviated {
    CEC_STATE_ABBREV_IDLE = 1,      ///< Corresponds to CEC_STATE_IDLE
    CEC_STATE_ABBREV_NOT_LOCAL = 2, ///< Corresponds to CEC_STATEs *FINISH*, *POST, and OVER_BOSS
    CEC_STATE_ABBREV_SCANNING = 3,  ///< Corresponds to CEC_STATE_SCANNING
    CEC_STATE_ABBREV_WLREADY =
        4, ///< Corresponds to CEC_STATE_WIRELESS_READY when some unknown bool is true
    CEC_STATE_ABBREV_OTHER = 5, ///< Corresponds to CEC_STATEs besides *FINISH*, *POST, and
                                /// OVER_BOSS and those listed here
};

/**
 * GetCecStateAbbreviated service function
 *  Inputs:
 *      0: 0x000E0000
 *  Outputs:
 *      1: Result code
 *      2: CecStateAbbreviated
 */
void GetCecStateAbbreviated(Service::Interface* self);

/**
 * GetCecInfoEventHandle service function
 *  Inputs:
 *      0: 0x000F0000
 *  Outputs:
 *      1: Result code
 *      3: Event Handle
 */
void GetCecInfoEventHandle(Service::Interface* self);

/**
 * GetChangeStateEventHandle service function
 *  Inputs:
 *      0: 0x00100000
 *  Outputs:
 *      1: Result code
 *      3: Event Handle
 */
void GetChangeStateEventHandle(Service::Interface* self);

/**
 * OpenAndRead service function
 *  Inputs:
 *      0: 0x00120104
 *      1: Buffer size
 *      2: NCCH Program ID
 *      3: Path type
 *      4: File open flag
 *      5: Descriptor for process ID
 *      6: Placeholder for process ID
 *      7: Descriptor for mapping a write-only buffer in the target process
 *      8: Buffer address
 *  Outputs:
 *      1: Result code
 *      2: Total bytes read
 *      3: Descriptor for mapping a write-only buffer in the target process
 *      4: Buffer address
 */
void OpenAndRead(Service::Interface* self);

/// Initialize CECD service(s)
void Init();

/// Shutdown CECD service(s)
void Shutdown();

} // namespace CECD
} // namespace Service
