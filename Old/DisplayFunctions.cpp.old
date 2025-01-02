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

#include "DisplayFunctions.h"
#include "StaticMenus.h"
#include "DeviceFunctions.h"
#include "Throttle.h"

/***********************************************************************************
Set up OLED libraries and object
***********************************************************************************/
OLED display(U8G2_R0, CS_PIN, DC_PIN);

void displayStartupInfo() {
  // Set up serial and display basic config
  CONSOLE.print(F("DCC-EX Serial Throttle "));
  CONSOLE.println(VERSION);
  // Set up OLED
  display.begin();
  display.setFont(DEFAULT_FONT);
  display.clear();
  display.drawStr(0, 15, "DCC-EX");
  display.setFont(DEFAULT_FONT);
  display.drawStr(0, 24, "Serial Throttle");
  display.drawStr(0, 33, VERSION);
  display.sendBuffer();
}

void displayKey(char key, KeyState keyState) {
  display.setFont(MENU_ITEM_FONT);
  display.setCursor(55, 63);
  display.print("  ");
  display.setCursor(55, 63);
  display.print(key);
  if (keyState==HOLD) {
    display.print("H");
  } else if (keyState==RELEASED) {
    display.print("R");
  } else {
    display.print("P");
  }
  display.sendBuffer();
}

void displayConnectionError() {
  display.clear();
  display.setFont(DEFAULT_FONT);
  display.drawStr(30, 10, "Connection error");
  display.sendBuffer();
}

void displayHome(TrackPower state) {
  display.clear();
  for (int i=0; i<NUM_THROTTLES; i++) {
    displayThrottleSpeed(i, throttles[i]->getSpeed(), true);
    displayThrottleDirection(i, throttles[i]->getDirection(), true);
    displayThrottleAddress(i, throttles[i]->getLocoAddress(), false, false);
  }
  display.drawHLine(0, 55, 128);
  display.setFont(MENU_ITEM_FONT);
  display.setCursor(0, 63);
  display.print("* Menu");
  display.setCursor(60, 63);
  display.print("Trk Power: ");
  displayPowerState(state);
  display.sendBuffer();
}

void displayThrottleSpeed(int throttle, int speed, bool isHome) {
  if (isHome) {
    display.setFont(SPEED_FONT);
    display.setCursor(
      throttleSetup[throttle].speed.X,
      throttleSetup[throttle].speed.Y
    );
    display.print("   ");
    display.setCursor(
      throttleSetup[throttle].speed.X,
      throttleSetup[throttle].speed.Y
    );
    display.print(speed);
    display.sendBuffer();
  }
}

void displayThrottleDirection(int throttle, Direction direction, bool isHome) {
  if (isHome) {
    display.setFont(DIRECTION_FONT);
    display.setCursor(
      throttleSetup[throttle].direction.X,
      throttleSetup[throttle].direction.Y
    );
    display.print("   ");
    display.setCursor(
      throttleSetup[throttle].direction.X,
      throttleSetup[throttle].direction.Y
    );
    if (direction==Forward) {
      display.print("Fwd");
    } else {
      display.print("Rev");
    }
    display.sendBuffer();
  }
}

void displayThrottleAddress(int throttle, int address, bool isOverridden, bool isConsist) {
  display.setFont(ADDRESS_FONT);
  display.setCursor(
    throttleSetup[throttle].address.X,
    throttleSetup[throttle].address.Y
  );
  display.print("       ");
  display.setCursor(
    throttleSetup[throttle].address.X,
    throttleSetup[throttle].address.Y
  );
  if (isOverridden) {
    display.print("*");
  } else {
    display.print(" ");
  }
  display.print(address);
  if (isConsist) {
    display.print("c");
  }
  display.sendBuffer();
}

void displayThrottleEStop(int throttle) {
  display.setFont(ESTOP_FONT);
  display.setCursor(
    throttleSetup[throttle].speed.X,
    throttleSetup[throttle].speed.Y
  );
  display.print("ESTOP");
  display.sendBuffer();
}

void displayPowerState(TrackPower state) {
  if (menuSystem.isHome()) {
    display.setFont(MENU_ITEM_FONT);
    display.setCursor(113, 63);
    display.print("   ");
    display.setCursor(113, 63);
    if (state==PowerOn) {
      display.print("On");
    } else if (state==PowerOff) {
      display.print("Off");
    } else {
      display.print("?");
    }
    display.sendBuffer();
  }
}

void displayEntryMenuItem(char* label, const char* instruction) {
  display.clear();
  display.setFont(MENU_TITLE_FONT);
  display.setCursor(0, 6);
  display.print(label);
  display.drawHLine(0, 7, 128);
  display.setCursor(0, 17);
  display.print(instruction);
  display.setCursor(0, 26);
  display.print("#####");
  display.drawHLine(0, 54, 128);
  display.setCursor(0, 63);
  display.print("* Back");
  display.setCursor(70, 63);
  display.print("# Confirm");
  display.sendBuffer();
}

void displayEntryError(const char* error) {
  display.setCursor(0, 26);
  display.print("#####");
  display.setCursor(0, 35);
  display.print(error);
  display.sendBuffer();
}

void displayEntryKey(char key, int column) {
  display.setCursor(column, 26);
  display.print(" ");
  display.setCursor(column, 26);
  display.print(key);
  display.sendBuffer();
}

void displayMenu(char* label, int currentPage, int itemsPerPage, int itemCount, char * itemList[]) {
  display.clear();
  display.setFont(MENU_TITLE_FONT);
  display.setCursor(0, 6);
  display.print(label);
  display.drawHLine(0, 7, 128);
  int X=0;
  int Y=17;
  display.setFont(MENU_ITEM_FONT);
  for (int i=0; i<itemsPerPage; i++) {
    if (itemList[i]) {
      display.setCursor(X, Y);
      display.print(i);
      display.print(" ");
      if (strlen(itemList[i])>10 && itemCount>5) {
        itemList[i][10]='\0';
      }
      display.print(itemList[i]);
      Y+=8;
      if (i==4) {
        X=64;
        Y=17;
      }
    }
  }
  display.drawHLine(0, 54, 128);
  display.setCursor(0, 63);
  display.print("* Back");
  if (itemCount>itemsPerPage) {
    display.setCursor(70, 63);
    display.print("# Page ");
    int nextPage=(currentPage+1)%((int)ceil(itemCount/(float)itemsPerPage))+1;
    display.print(nextPage);
  }
  display.sendBuffer();
  displayTurnoutStates();
  displayTurntableIndex();
}

void displayRoster(char* label, int currentPage, int itemsPerPage) {
  int count=dccexProtocol.getRosterCount();
  auto r=dccexProtocol.roster;
  display.clear();
  display.setFont(MENU_TITLE_FONT);
  display.setCursor(0, 6);
  display.print(label);
  display.drawHLine(0, 7, 128);
  int X=0;
  int Y=17;
  display.setFont(MENU_ITEM_FONT);
  int i=0;
  for (Loco* loco=r->getFirst(); loco; loco=loco->getNext()) {
    int index=currentPage*itemsPerPage+i;
    if (index>=currentPage*itemsPerPage && index<(currentPage&itemsPerPage)+itemsPerPage) {
      display.setCursor(X, Y);
      display.print(i);
      if (Throttle::addressInUse(throttles, NUM_THROTTLES, loco->getAddress())) {
        display.print("-");
      } else {
        display.print(" ");
      }
      display.print(loco->getName());
      Y+=8;
      if (i==4) {
        X=64;
        Y=17;
      }
    }
    i++;
  }
  display.drawHLine(0, 54, 128);
  display.setCursor(0, 63);
  display.print("* Back");
  if (count>itemsPerPage) {
    display.setCursor(70, 63);
    display.print("# Page ");
    int nextPage=(currentPage+1)%((int)ceil(count/(float)itemsPerPage))+1;
    display.print(nextPage);
  }
  display.sendBuffer();
}

void displayTurnoutStates() {
  Menu* tMenu=static_cast<Menu*>(menuSystem.getCurrentItem());
  if (tMenu && strcmp(tMenu->getLabel(), "Turnouts")==0) {
    int page=tMenu->getCurrentPage();
    int ppage=tMenu->getItemsPerPage();
    int X=6;
    int Y=16;
    display.setFont(STATUS_FONT);
    for (int i=0; i<ppage; i++) {
      int index=page*ppage+i;
      char tThrown=' ';
      ActionMenuItem* item=static_cast<ActionMenuItem*>(tMenu->getItemAtIndex(index));
      if (item) {
        Turnout* t=static_cast<Turnout*>(item->getObjectPointer());
        tThrown=(t->getThrown()) ? 'T' : 'C';
      }
      display.setCursor(X, Y);
      display.print(" ");
      display.setCursor(X, Y);
      display.print(tThrown);
      Y+=8;
      if (i==4) {
        X=70;
        Y=16;
      }
    }
    display.sendBuffer();
  }
}

void displayTurntableIndex() {
  Menu* ttMenu=static_cast<Menu*>(menuSystem.getCurrentItem());
  if (ttMenu) {
    TurntableIndex* tti=static_cast<TurntableIndex*>((static_cast<ActionMenuItem*>(ttMenu->getItemList()))->getObjectPointer());
    if (tti) {
      int ttId=tti->getTTId();
      Turntable* tt=dccexProtocol.turntables->getById(ttId);
      if (tt) {
        int page=ttMenu->getCurrentPage();
        int ppage=ttMenu->getItemsPerPage();
        int currentIndex=tt->getIndex();
        int X=6;
        int Y=16;
        display.setFont(STATUS_FONT);
        for (int i=0; i<ppage; i++) {
          int index=page*ppage+i;
          display.setCursor(X, Y);
          display.print(" ");
          display.setCursor(X, Y);
          if (index==currentIndex) {
            display.print("*");
          }
          Y+=8;
          if (i==4) {
            X=70;
            Y=16;
          }
        }
        display.sendBuffer();
      }
    }
  }
}

void displayInfoScreen(char* label) {
  display.clear();
  display.setFont(MENU_TITLE_FONT);
  display.setCursor(0, 6);
  display.print(label);
  display.drawHLine(0, 7, 128);
  display.setCursor(0, 17);
  display.print("DCC-EX Serial Throttle");
  display.setCursor(0, 26);
  display.print("Version: ");
  display.print(VERSION);
  display.setCursor(0, 35);
  display.print("Free RAM: ");
  display.print(getFreeMemory());
  display.print(" bytes");
  display.setCursor(0, 44);
  display.print("EX-CommandStation");
  display.setCursor(0, 53);
  display.print("Version: ");
  display.print(dccexProtocol.getMajorVersion());
  display.print(".");
  display.print(dccexProtocol.getMinorVersion());
  display.print(".");
  display.print(dccexProtocol.getPatchVersion());
  display.drawHLine(0, 54, 128);
  display.setCursor(0, 63);
  display.print("* Back");
  display.sendBuffer();
}
