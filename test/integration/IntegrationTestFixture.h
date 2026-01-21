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

#ifndef INTEGRATIONTESTFIXTURE_H
#define INTEGRATIONTESTFIXTURE_H

#include "AppOrchestrator.h"
#include "CommandStationListener.h"
#include "test/mocks/MockButton.h"
#include "test/mocks/MockDisplay.h"
#include "test/mocks/MockKeypad.h"
#include "test/mocks/MockRotaryEncoder.h"
#include <gtest/gtest.h>

using namespace testing;

/**
 * @brief Test harness for integration tests, complete mock app setup
 */
class IntegrationTestBase : public Test {
protected:
  MockButton *button1;
  MockButton *button2;
  MockButton *button3;
  MockRotaryEncoder *encoder1;
  MockRotaryEncoder *encoder2;
  MockRotaryEncoder *encoder3;
  MockDisplay *display;
  MockKeypad *keypad;
  Stream console;
  Stream csConnection;
  Logger *logger;
  EventManager *eventManager;
  DCCEXProtocol *csClient;
  CommandStationListener *csListener;
  ConnectionManager *connectionManager;
  MenuManager *menuManager;
  Throttle *throttles[NUM_THROTTLES];
  AppOrchestrator *appOrchestrator;

  void SetUp() override {
    // Create objects
    button1 = new NiceMock<MockButton>();
    button2 = new NiceMock<MockButton>();
    button3 = new NiceMock<MockButton>();
    encoder1 = new NiceMock<MockRotaryEncoder>();
    encoder2 = new NiceMock<MockRotaryEncoder>();
    encoder3 = new NiceMock<MockRotaryEncoder>();
    display = new NiceMock<MockDisplay>();
    keypad = new NiceMock<MockKeypad>();
    logger = new Logger;
    eventManager = new EventManager(logger);
    csClient = new DCCEXProtocol;
    csListener = new CommandStationListener(eventManager, logger);
    csClient->setLogStream(&console);
    csClient->setDelegate(csListener);
    csClient->connect(&csConnection);
    connectionManager = new ConnectionManager(csClient, eventManager, logger);
    menuManager = new MenuManager(eventManager, logger);
    throttles[0] = new Throttle(0, button1, encoder1, csClient, logger, 1, 2, 5);
    throttles[1] = new Throttle(1, button2, encoder2, csClient, logger, 1, 2, 5);
    throttles[2] = new Throttle(2, button3, encoder3, csClient, logger, 1, 2, 5);
    appOrchestrator = new AppOrchestrator(display, keypad, logger, NUM_THROTTLES, throttles, connectionManager,
                                          eventManager, menuManager, csClient);

    // Initialise
    logger->setOutput(&console);
    appOrchestrator->begin();
  }

  void TearDown() override {
    delete appOrchestrator;
    for (int i = 0; i < NUM_THROTTLES; i++) {
      delete throttles[i];
    }
    delete menuManager;
    delete connectionManager;
    delete csListener;
    delete csClient;
    delete eventManager;
    delete logger;
    delete keypad;
    delete display;
    delete encoder3;
    delete encoder2;
    delete encoder1;
    delete button3;
    delete button2;
    delete button1;
  }
};

#endif
