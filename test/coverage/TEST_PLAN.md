# Coverage Test Plan (Local Branch)

- Branch: feature/coverage-apporchestrator-tests
- Objective: Add unit tests to increase branch coverage, focusing on AppOrchestrator and related core components using existing mocks.

- Plan of action (high level)
- Create AppOrchestrator unit tests to cover construction, begin(), get/set app state, and basic lifecycle without requiring full integration.
- Add additional tests for small, self-contained components where possible (e.g., Menu/Logger) leveraging existing tests as anchors.
- Run coverage tooling after each batch to guide further test additions.

- Tests to implement (success scenarios)
- AppOrchestratorUnitTest: BeginAndStateAccessors
  - Build a minimal graph with mocks and THREE throttles using mocked Button/RotaryEncoder
  - Call begin(); verify getCurrentAppState() returns a valid enum value (not APP_STATE_COUNT)
  - Call setCurrentAppState(AppState::Menu) and verify getCurrentAppState() returns Menu
  - Clean up objects
- Optional: Test that multiple begin() calls do not crash (idempotency of init path)
- Extend Menu/Logger tests if coverage gaps remain in those modules (post-AppOrchestrator) by adding 2 targeted scenarios per module.

- Tests to implement (failure modes / edge paths)
- AppOrchestratorUnitTest: BeginWithNullDependencies (sanity check no crash when mocks fail to initialize)
- AppOrchestratorUnitTest: InvalidStateTransition (attempt to switch to APP_STATE_COUNT or an invalid state triggers no crash)
- MenuManager and Menu edge-path tests (additional boundary cases) if uncovered branches remain after AppOrchestrator test batch.

- Testing constraints
- Reuse existing mocks: MockDisplay, MockKeypad, MockButton, MockRotaryEncoder
- Do not modify production code; only add tests
- Ensure each test cleans up allocated resources

- Deliverables
- Added unit tests under test/unit/test_AppOrchestrator/test_AppOrchestrator.cpp
- Documentation of plan in this file for future reviews
