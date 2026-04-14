# my-arduino-project — Project Rulebook

## Stack

- PlatformIO, Arduino framework, AVR/ATmega328P (or your target)
- GoogleTest framework for native tests

## Conventions

- Business logic in plain C++ classes — no direct Arduino API calls
- Hardware access behind interfaces that can be mocked
- All array accesses bounds-checked
- No dynamic memory allocation without RAII wrapper
- All local variables initialised at declaration

## Definition of Done

A feature is NOT done until:
1. `pio test -e native` passes — zero sanitizer violations, zero leaks, zero test failures
2. Firmware builds cleanly: `pio run -e uno`
3. @sanitizer-reviewer gate is CLEARED
