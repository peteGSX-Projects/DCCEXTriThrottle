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

#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

#include <Arduino.h>
#include "defines.h"
#include "Keypad.h"
#include "DCCEXProtocol.h"
#include "DisplayFunctions.h"
#include "DCCEXObjects.h"

class MenuSystem;

class MenuItemBase {
public:
  /// @brief Type of item for the object
  enum ItemType {
    Normal,
    Action,
    Entry,
    Menu,
    Throttle,
    Info,
  };

  /// @brief Constructor
  /// @param label 
  /// @param type 
  MenuItemBase(const char* label, MenuItemBase::ItemType type);
  
  /// @brief Get the object's label
  /// @return 
  char* getLabel() const;

  /// @brief Get the item type of the object
  /// @return 
  ItemType getType();

  /// @brief Set the next object in the list
  /// @param next 
  void setNext(MenuItemBase* next);

  /// @brief Get the next object
  /// @return 
  MenuItemBase* getNext() const;
  
  /// @brief Set the objects index
  /// @param index 
  void setIndex(int index);

  /// @brief Get the index value of this object
  /// @return 
  int getIndex() const;
  
  /// @brief What happens when the item is selected from a menu
  virtual void select() {};
  
  /// @brief Display method of this object
  /// @param oled 
  virtual void display() {};
  
  /// @brief Keypad input processing of this object
  /// @param key 
  /// @param keyState 
  virtual void handleKeys(char key, KeyState keyState) {};
  
  /// @brief Sets the static attribute for referencing the menu system
  /// @param menuSystem 
  static void setMenuSystem(MenuSystem* menuSystem);

  /// @brief Set the parent object
  /// @param parent 
  void setParent(MenuItemBase* parent);

protected:
  char* _label;
  ItemType _type;
  MenuItemBase* _next;
  int _index;
  MenuItemBase* _parent;
  static MenuSystem* _menuSystem;

  friend class MenuSystem;

};

class ActionMenuItem : public MenuItemBase {
public:
  typedef void (*Action)();

  /// @brief Constructor for an action item
  /// @param label 
  /// @param action 
  ActionMenuItem(const char* label, Action action, void* objectPointer=nullptr);
  
  /// @brief What happens when the item is selected from a menu
  void select() override;

  /// @brief Get the associated object pointer for this item
  /// @return 
  void* getObjectPointer();

private:
  Action _action;
  void* _objectPointer;

};

class EntryMenuItem : public MenuItemBase {
public:
  typedef void (*Action)(int number);
  
  /// @brief Constructor for a user input entry item
  /// @param label 
  /// @param instruction 
  /// @param action 
  EntryMenuItem(const char* label, const char* instruction, Action action);

  /// @brief What happens when the item is selected from a menu
  void select() override;
  
  /// @brief Display the user entry screen
  void display() override;
  
  /// @brief Handle user input via keypad
  /// @param key 
  /// @param keyState 
  void handleKeys(char key, KeyState keyState) override;

private:
  const char* _instruction;
  Action _action;
  int _inputNumber;
  char _inputBuffer[6];
  int _inputIndex;
  int _inputKeyColumn;

};

class Menu : public MenuItemBase {
public:
  /// @brief Constructor for a menu
  /// @param label 
  Menu(const char* label);
  
  /// @brief What happens when the item is selected from a menu
  void select() override;
  
  /// @brief Display the menu
  void display() override;

  /// @brief Respond to user input
  /// @param key 
  /// @param keyState 
  void handleKeys(char key, KeyState keyState) override;
  
  /// @brief Add a menu item
  /// @param item ActionMenuItem, EntryMenuItem, Menu, ThrottleScreen
  void addMenuItem(MenuItemBase* item);

  /// @brief Get the object in this menu at the specified index
  /// @param index 
  /// @return 
  MenuItemBase* getItemAtIndex(int index);

  /// @brief Get the menu item list
  /// @return 
  MenuItemBase* getItemList();

  int getItemCount();
  
  int getCurrentPage();

  int getItemsPerPage();

private:
  MenuItemBase* _itemList;
  int _itemCount;
  int _currentPage;
  int _itemsPerPage;

};

class ThrottleMenu : public Menu {
public:
  ThrottleMenu(const char* label, int throttleNumber);

  void select() override;

private:
  int _throttleNumber;

};

class ThrottleScreen : public MenuItemBase {
public:
  /// @brief  Constructor for the throttle screen
  ThrottleScreen();
  
  /// @brief Display the throttle screen
  void select() override;

  /// @brief Respond to user input
  /// @param key 
  /// @param keyState 
  void handleKeys(char key, KeyState keyState) override;

  /// @brief Set the menu to display when * pressed
  /// @param menu 
  void setMenu(MenuItemBase* menu);

  /// @brief Get the child menu object
  /// @return 
  MenuItemBase* getMenu();

private:
  MenuItemBase* _menu;

};

class InfoScreen : public MenuItemBase {
public:
  /// @brief Constructor for the info screen
  InfoScreen();

  void select() override;

  void display() override;

  void handleKeys(char key, KeyState keyState) override;

};

// class RosterMenuItem

class RosterMenuItem : public MenuItemBase {
public:
  RosterMenuItem(const char* label, void (*selectFunction)(int)=nullptr);

  void select() override;

  void display() override;

  void handleKeys(char key, KeyState keyState) override;

private:
  int _currentPage;
  int _itemsPerPage;
  void (*_selectFunction)(int);

};

class MenuSystem {
public:
  /// @brief Constructor for the menu system
  MenuSystem();
  
  /// @brief Display the currently selected item
  void display();
  
  /// @brief Pass user input to the currently selected item
  /// @param key 
  /// @param keyState 
  void handleKeys(char key, KeyState keyState);
  
  /// @brief Set the home menu item
  /// @param home 
  void setHome(MenuItemBase* home);
  
  /// @brief Return to the specified home item
  void goHome();

  /// @brief Set the currently selected item
  /// @param currentItem 
  void setCurrentItem(MenuItemBase* currentItem);

  /// @brief Find a menu by its label attribute
  /// @param label 
  /// @return 
  Menu* findMenuByLabel(const char* label);

  /// @brief Set the current action item for callbacks to query
  /// @param item 
  void setCurrentActionItem(ActionMenuItem* item);

  /// @brief Get the currently selected action item
  /// @return 
  ActionMenuItem* getCurrentActionItem();

  /// @brief Set the current throttle being managed
  /// @param throttle 
  void setCurrentThrottle(int throttle);

  /// @brief Get the current throttle being managed
  /// @return 
  int getCurrentThrottle();

  /// @brief Check if the menu system is on the home screen
  /// @return 
  bool isHome();

  /// @brief Update the current track power state
  /// @param state 
  void updatePowerState(TrackPower state);

  /// @brief Get the current item
  /// @return 
  MenuItemBase* getCurrentItem();

  void setDCCEXProtocol(DCCEXProtocol* dccexProtocol);

  DCCEXProtocol* getDCCEXProtocol();

private:
  MenuItemBase* _currentItem;
  MenuItemBase* _home;
  ActionMenuItem* _currentActionItem;
  int _currentThrottle;
  TrackPower _powerState=PowerUnknown;
  DCCEXProtocol* _dccexProtocol;

  Menu* _findMenuByLabelRecursive(const char* label, MenuItemBase* currentMenuItem);
  void _displayPowerState();
  
};

#endif
