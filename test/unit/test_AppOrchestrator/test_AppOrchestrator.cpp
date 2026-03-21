/*
 *  Tests for AppOrchestrator - unit-level coverage using mocks
 *  This file adds a minimal smoke test to exercise construction, begin and basic state accessors.
 *  It is designed to run with GoogleTest and the existing test/mocks in this repo.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "AppOrchestrator.h"

// Mocks used to satisfy the AppOrchestrator dependencies
#include "test/mocks/DCCEXTestHelpers.h"
#include "test/mocks/MockButton.h"
#include "test/mocks/MockDisplay.h"
#include "test/mocks/MockKeypad.h"
#include "test/mocks/MockRotaryEncoder.h"

#include "ConnectionManager.h"
#include "EventManager.h"
#include "MenuManager.h"
#include "Throttle.h"

using namespace testing;

// Helper function to create a lightweight Throttle with mocks
static Throttle *makeThrottle(int index, DCCEXProtocol *csClient, Logger *logger) {
  auto confirmer = new NiceMock<MockButton>();
  auto selector = new NiceMock<MockRotaryEncoder>();
  // The constructor takes: index, confirmer, selector, commandStationClient, logger, and 3 step values
  return new Throttle(index, confirmer, selector, csClient, logger, 1, 2, 5);
}

TEST(AppOrchestratorUnitTest, BeginAndStateAccessors) {
  // Arrange: build minimal test graph using existing mocks
  MockDisplay display;
  NiceMock<MockKeypad> keypad;
  Logger logger;

  // Core collaborators
  EventManager eventManager(&logger);
  MenuManager menuManager(&eventManager, &logger);
  DCCEXProtocol *csClient = new DCCEXProtocol();
  ConnectionManager *connectionManager = new ConnectionManager(csClient, &eventManager, &logger);

  // Throttle instances with mocks
  Throttle *throttles[3];
  throttles[0] = makeThrottle(0, csClient, &logger);
  throttles[1] = makeThrottle(1, csClient, &logger);
  throttles[2] = makeThrottle(2, csClient, &logger);

  // App under test
  AppOrchestrator *app = new AppOrchestrator(&display, &keypad, &logger, 3, throttles, connectionManager, &eventManager,
                                             &menuManager, csClient);

  // Act: initialise via begin()
  app->begin();

  // Assert: basic state is a valid AppState and set/get round-trips
  AppState initial = app->getCurrentAppState();
  // The initial state is expected to be a valid enum value; exact state depends on implementation,
  // but it must be one of the defined AppState values and not APP_STATE_COUNT.
  EXPECT_NE(initial, AppState::APP_STATE_COUNT);

  // Change state and validate the setter/getter behaves
  app->setCurrentAppState(AppState::Menu);
  AppState afterSet = app->getCurrentAppState();
  EXPECT_EQ(afterSet, AppState::Menu);

  // Cleanup
  delete app;
  // Throttles own their internal confirmer/selector mocks; delete after app is torn down
  delete throttles[0];
  delete throttles[1];
  delete throttles[2];
  delete connectionManager;
  delete csClient;
}
