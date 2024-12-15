/*
 *  © 2023 Peter Cole
 *
 *  This file is for a serially connected throttle for a DCC-EX EX-CommandStation.
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

#include <Arduino.h>
#include "defines.h"
#include "DisplayFunctions.h"
#include "DCCEXObjects.h"
#include "StaticMenus.h"
#include "Throttle.h"

DCCEXProtocol dccexProtocol;
DCCEXCallbacks dccexCallbacks;

bool retrievalDisplayed=false;
bool errorDisplayed=false;
bool homeDisplayed=false;
bool requestedVersion=false;
uint8_t connectionRetries=CONNECT_RETRIES;
unsigned long retryDelay=1000;
unsigned long lastRetry=0;
bool gotRoutes=false;
bool gotTurnouts=false;
bool gotTurntables=false;
int progressX=0;
int progressY=30;

void getDCCEXObjects() {
  if (!dccexProtocol.receivedLists() && connectionRetries>0) {
    if (!retrievalDisplayed) {
      display.clear();
      display.setFont(DEFAULT_FONT);
      display.drawStr(0, 10, "Retrieving DCC-EX object lists");
      display.sendBuffer();
      retrievalDisplayed=true;
    } else if (!dccexProtocol.receivedLists() && millis()-lastRetry>retryDelay && connectionRetries > 0) {
      lastRetry = millis();
      connectionRetries--;
      display.setFont(DEFAULT_FONT);
      display.setCursor(progressX, progressY);
      display.print(".");
      display.sendBuffer();
      progressX+=5;
      if (progressX>122) {
        progressY+=8;
        progressX=0;
      }
    }
    dccexProtocol.getLists(true, true, true, true);
    if (!dccexProtocol.receivedVersion() && !requestedVersion) {
      requestedVersion=true;
      dccexProtocol.requestServerVersion();
    }
  }  else if (!dccexProtocol.receivedLists() && connectionRetries==0 && !errorDisplayed) {
    errorDisplayed = true;
    displayConnectionError();
  }  else if (dccexProtocol.receivedLists() && !homeDisplayed) {
    homeDisplayed=true;
    updateRoutes();
    updateTurnouts();
    updateTurntables();
    menuSystem.goHome();
  }
}

// Function to update route entries from the CS
// To trigger after startup, simply set requestedRoutes to false
void updateRoutes() {
  if (dccexProtocol.receivedRouteList() && !gotRoutes) {
    gotRoutes = true;
    Menu* rtMenu=menuSystem.findMenuByLabel("Routes");
    if (!rtMenu) return;
    for (Route* r=dccexProtocol.routes->getFirst(); r; r=r->getNext()) {
      rtMenu->addMenuItem(new ActionMenuItem(r->getName(), nullptr));
    }
  }
}

// Function to update turnout entries from the CS
// To trigger after startup, simply set requestedTurnouts to false
void updateTurnouts() {
  if (dccexProtocol.receivedTurnoutList() && !gotTurnouts) {
    gotTurnouts=true;
    Menu* tMenu=menuSystem.findMenuByLabel("Turnouts");
    for (Turnout* t=dccexProtocol.turnouts->getFirst(); t; t=t->getNext()) {
      tMenu->addMenuItem(new ActionMenuItem(t->getName(), toggleTurnout, t));
    }
  }
}

// Function to update turntable entries from the CS
// To trigger after startup, simply set requestedTurntables to false
void updateTurntables() {
  if (dccexProtocol.receivedTurntableList() && !gotTurntables) {
    gotTurntables=true;
    Menu* ttMenu=menuSystem.findMenuByLabel("Turntables");
    if (!ttMenu) return;
    for (Turntable* tt=dccexProtocol.turntables->getFirst(); tt; tt=tt->getNext()) {
      const char* ttName=tt->getName();
      Menu* newTTMenu=new Menu(ttName);
      ttMenu->addMenuItem(newTTMenu);
      for (TurntableIndex* idx=tt->getFirstIndex(); idx; idx=idx->getNextIndex()) {
        const char* idxName=idx->getName();
        newTTMenu->addMenuItem(new ActionMenuItem(idxName, rotateTurntable, idx));
      }
    }
  }
}

// Function to toggle a turnout
// If closed, will throw, if thrown, will close
void toggleTurnout() {
  if (!menuSystem.getCurrentActionItem()) return;
  void* object=menuSystem.getCurrentActionItem()->getObjectPointer();
  Turnout* turnout=static_cast<Turnout*>(object);
  if (!turnout) return;
  int turnoutId = turnout->getId();
  dccexProtocol.toggleTurnout(turnoutId);
}

void closeTurnout() {
  if (!menuSystem.getCurrentActionItem()) return;
  void* object=menuSystem.getCurrentActionItem()->getObjectPointer();
  Turnout* turnout=static_cast<Turnout*>(object);
  if (!turnout) return;
  int turnoutId = turnout->getId();
  dccexProtocol.closeTurnout(turnoutId);
}

void throwTurnout() {
  if (!menuSystem.getCurrentActionItem()) return;
  void* object=menuSystem.getCurrentActionItem()->getObjectPointer();
  Turnout* turnout=static_cast<Turnout*>(object);
  if (!turnout) return;
  int turnoutId = turnout->getId();
  dccexProtocol.throwTurnout(turnoutId);
}

// Function to rotate a turntable
void rotateTurntable() {
  if (!menuSystem.getCurrentActionItem()) return;
  void* object=menuSystem.getCurrentActionItem()->getObjectPointer();
  TurntableIndex* index=static_cast<TurntableIndex*>(object);
  if (!index) return;
  int ttId=index->getTTId();
  int indexId=index->getId();
  dccexProtocol.rotateTurntable(ttId, indexId, 0);
}

// Function to turn track power on or off
void trackPowerOn() {
  dccexProtocol.powerOn();
  menuSystem.goHome();
}

void trackPowerOff() {
  dccexProtocol.powerOff();
  menuSystem.goHome();
}

// Function to join or unjoin tracks
void setJoinTracks() {
  // if (currentMenuPtr != nullptr) {
  //   int join = currentMenuPtr->getItem(currentMenuPtr->getSelectedItem()).objectId;
  //   if (join == 1) {
  //     CONSOLE.println("Would join but not implemented yet");
  //   } else {
  //     CONSOLE.println("Would unjoin but not implemented yet");
  //   }
  // }
}

void setRosterLoco(int selectedLoco) {
  int throttle=menuSystem.getCurrentThrottle();
  auto th=throttles[throttle];
  if (th->isConsist()) return;
  if (th->isLoco()) {
    th->forgetLoco();
  }
  int i=0;
  for (Loco* loco=dccexProtocol.roster->getFirst(); loco; loco=loco->getNext()) {
    if (i==selectedLoco) {
      if (!Throttle::addressInUse(throttles, NUM_THROTTLES, loco->getAddress())) {
        th->setLoco(loco);
      }
      break;
    }
    i++;
  }
  menuSystem.goHome();
}

void addConsistLoco(int selectedLoco) {
  int throttle=menuSystem.getCurrentThrottle();
  auto th=throttles[throttle];
  if (th->isLoco()) return;
  Consist* consist=th->getConsist();
  if (!consist) {
    CONSOLE.println("Create new consist");
    consist=new Consist();
    // th->setConsist(consist);
  } else {
    CONSOLE.println("Already a consist");
  }
  int i=0;
  for (Loco* loco=dccexProtocol.roster->getFirst(); loco; loco=loco->getNext()) {
    if (i==selectedLoco) {
      if (!Throttle::addressInUse(throttles, NUM_THROTTLES, loco->getAddress())) {
        consist->addLoco(loco, Facing::FacingForward);
        CONSOLE.print("Adding loco ");
        CONSOLE.print(loco->getAddress());
        CONSOLE.println(" to consist");
      }
      break;
    }
    i++;
  }
  if (consist) {
    th->setConsist(consist);
  }
  menuSystem.goHome();
}

bool setLocoAddress(int throttle, int address) {
  if (Throttle::addressInUse(throttles, NUM_THROTTLES, address)) return false;
  Loco* newLoco=new Loco(address, LocoSource::LocoSourceEntry);
  throttles[throttle]->setLoco(newLoco);
  return true;
}

void readLocoAddress() {
  dccexProtocol.readLoco();
}

void forgetLoco() {
  int throttle=menuSystem.getCurrentThrottle();
  if (throttles[throttle]->getSpeed()>0) return;
  throttles[throttle]->forgetLoco();
  menuSystem.goHome();
}

void showConsist() {
  int throttle=menuSystem.getCurrentThrottle();
  auto th=throttles[throttle];
  Consist* consist=th->getConsist();
  if (consist) {
    for (ConsistLoco* cl=consist->getFirst(); cl; cl=cl->getNext()) {
      CONSOLE.print(cl->getLoco()->getName());
      CONSOLE.print(" - ");
      if (cl->getFacing()==Facing::FacingReversed) {
        CONSOLE.println("Rev");
      } else {
        CONSOLE.println("Fwd");
      }
    }
  }
}
