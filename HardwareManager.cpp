/*
 *  © 2026 Peter Cole
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this code.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "HardwareManager.h"
#include <Arduino.h>

#if defined(ARDUINO_ARCH_STM32)
extern "C" char *sbrk(int incr);
#endif

void HardwareManager::initialise() {
#if defined(ARDUINO_BLUEPILL_F103C8)
  // Disable JTAG and enable SWD by clearing the SWJ_CFG bits
  // Assuming the register is named AFIO_MAPR or AFIO_MAPR2
  AFIO->MAPR &= ~(AFIO_MAPR_SWJ_CFG);
  // or
  // AFIO->MAPR2 &= ~(AFIO_MAPR2_SWJ_CFG);
#endif // BLUEPILL

#ifndef NATIVE_TESTING
  CONSOLE_STREAM.begin(115200);
  COMMANDSTATION_STREAM.begin(115200);
#endif // NATIVE_TESTING
}

int HardwareManager::getFreeMemory() {
#if defined(ARDUINO_ARCH_STM32)
  char top;
  return &top - reinterpret_cast<char *>(sbrk(0));
#else
  return 65536; // Fallback for native testing
#endif
}

bool HardwareManager::isMemorySafe() {
  if (HardwareManager::getFreeMemory() < _stackHeapBuffer) {
    return false;
  } else {
    return true;
  }
}
