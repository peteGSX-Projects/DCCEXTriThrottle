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
#include "MenuSystem.h"

MenuSystem* MenuItemBase::_menuSystem=nullptr;

// class MenuItem
MenuItemBase::MenuItemBase(const char* label, MenuItemBase::ItemType type)
  : _type(type) {
  _parent=nullptr;
  _next=nullptr;
  _index=-1;
  _label=strdup(label);
}

char* MenuItemBase::getLabel() const {
  return _label;
}

MenuItemBase::ItemType MenuItemBase::getType() {
  return _type;
}

void MenuItemBase::setNext(MenuItemBase* next) {
  _next=next;
}

MenuItemBase* MenuItemBase::getNext() const {
  return _next;
}

void MenuItemBase::setIndex(int index) {
  _index=index;
}

int MenuItemBase::getIndex() const {
  return _index;
}

void MenuItemBase::setMenuSystem(MenuSystem* menuSystem) {
  _menuSystem=menuSystem;
}

void MenuItemBase::setParent(MenuItemBase* parent) {
  _parent=parent;
}

// class ActionMenuItem
ActionMenuItem::ActionMenuItem(const char* label, Action action, void* objectPointer)
  : MenuItemBase(label, MenuItemBase::Action) {
    _action=action;
    _objectPointer=objectPointer;
  }

void ActionMenuItem::select() {
  if (!_menuSystem || !_action) return;
  _menuSystem->setCurrentActionItem(this);
  _action();
}

void* ActionMenuItem::getObjectPointer() {
  return _objectPointer;
}

// class EntryMenuItem
EntryMenuItem::EntryMenuItem(const char* label, const char* instruction, Action action)
  : MenuItemBase(label, ItemType::Entry), _instruction(instruction), _action(action), _inputNumber(0) {
  _inputIndex=0;
  _inputKeyColumn=0;
}

void EntryMenuItem::select() {
  _menuSystem->setCurrentItem(this);
  _inputIndex=0;
  _inputKeyColumn=0;
  this->display();
}

void EntryMenuItem::display() {
  displayEntryMenuItem(_label, _instruction);
}

void EntryMenuItem::handleKeys(char key, KeyState keyState) {
  if (keyState==PRESSED) {
    switch(key) {
      case '*':
        if (_parent) {
          _menuSystem->setCurrentItem(_parent);
          _parent->display();
        }
        break;
      
      case '#':
        if (_inputIndex>0) {
          int number=atoi(_inputBuffer);
          _inputIndex=0;
          memset(_inputBuffer, 0, sizeof(_inputBuffer));
          _inputKeyColumn=0;
          if (number < 1 || number > 10239) {
            displayEntryError("INVALID ADDRESS");
          } else {
            if (setLocoAddress(_menuSystem->getCurrentThrottle(), number)) {
              _menuSystem->goHome();
            } else {
              displayEntryError("ADDRESS IN USE");
            }
          }
        }
        break;
      
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if (_inputIndex<5) {
          _inputBuffer[_inputIndex++]=key;
          displayEntryKey(key, _inputKeyColumn);
          _inputKeyColumn+=5;
        }
        break;
      
      default:
        break;
    }
  }
}

// class Menu
Menu::Menu(const char* label)
  : MenuItemBase(label, MenuItemBase::Menu) {
  _itemList=nullptr;
  _itemCount=0;
  _currentPage=0;
  _itemsPerPage=10;
}

void Menu::select() {
  _menuSystem->setCurrentItem(this);
  this->display();
}

void Menu::display() {
  char* itemList[_itemsPerPage];

  for (int i=0; i<_itemsPerPage; i++) {
    int index=_currentPage*_itemsPerPage+i;
    MenuItemBase* item=getItemAtIndex(index);
    if (item) {
      itemList[i]=item->getLabel();
    } else {
      itemList[i]=nullptr;
    }
  }

  displayMenu(_label, _currentPage, _itemsPerPage, _itemCount, itemList);
}

void Menu::handleKeys(char key, KeyState keyState) {
  if (keyState==PRESSED) {
    switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': {
        int itemIndex=key-'0';
        itemIndex+=_currentPage*_itemsPerPage;
        MenuItemBase* item=getItemAtIndex(itemIndex);
        if (item) {
          item->select();
        }
        break;
      }

      case '*':
        if (this->_parent) {
          _menuSystem->setCurrentItem(this->_parent);
          _parent->select();
        }
        break;
      
      case '#':
        _currentPage=(_currentPage+1) % ((int)ceil(_itemCount/(float)_itemsPerPage));
        display();
        break;
      
      default:
        break;
    }
  }
}

void Menu::addMenuItem(MenuItemBase* item) {
  if (this->_itemList==nullptr) {
    this->_itemList=item;
  } else {
    MenuItemBase* current=this->_itemList;
    while (current->getNext()!=nullptr) {
      current=current->getNext();
    }
    current->setNext(item);
  }
  item->setIndex(_itemCount);
  item->setParent(this);
  _itemCount++;
}

MenuItemBase* Menu::getItemAtIndex(int index) {
  for (MenuItemBase* item=_itemList; item; item=item->getNext()) {
    if (item->getIndex()==index) {
      return item;
    }
  }
  return nullptr;
}

MenuItemBase* Menu::getItemList() {
  return _itemList;
}

int Menu::getItemCount() {
  return _itemCount;
}
  
int Menu::getCurrentPage() {
  return _currentPage;
}

int Menu::getItemsPerPage() {
  return _itemsPerPage;
}

// class ThrottleMenu

ThrottleMenu::ThrottleMenu(const char* label, int throttleNumber)
  : Menu(label) {
  _throttleNumber=throttleNumber;
}

void ThrottleMenu::select() {
  _menuSystem->setCurrentThrottle(_throttleNumber);
  Menu::select();
}

// class ThrottleScreen
ThrottleScreen::ThrottleScreen()
  : MenuItemBase(nullptr, MenuItemBase::Throttle) {}

void ThrottleScreen::select() {
  _menuSystem->goHome();
}

void ThrottleScreen::handleKeys(char key, KeyState keyState) {
  if (_menu) {
    if (keyState==PRESSED) {
      switch(key) {
        case '*':
          _menuSystem->setCurrentItem(_menu);
          _menu->display();
          break;
        
        default:
          break;
      }
    }
  }
}

void ThrottleScreen::setMenu(MenuItemBase* menu) {
  _menu=menu;
  _menu->setParent(this);
}

MenuItemBase* ThrottleScreen::getMenu() {
  return _menu;
}

// class InfoScreen

InfoScreen::InfoScreen()
  : MenuItemBase("Sys Info", ItemType::Info) {}

void InfoScreen::select() {
  _menuSystem->setCurrentItem(this);
  this->display();
}

void InfoScreen::display() {
  displayInfoScreen(_label);
}

void InfoScreen::handleKeys(char key, KeyState keyState) {
  if (keyState==PRESSED) {
    switch(key) {
      case '*':
      if (_parent) {
        _menuSystem->setCurrentItem(_parent);
        _parent->display();
      }
      break;
      
      default:
        break;
    }
  }
}

// class RosterMenuItem

RosterMenuItem::RosterMenuItem(const char* label, void (*selectFunction)(int))
  : MenuItemBase(label, ItemType::Normal) {
    _selectFunction=selectFunction;
    _itemsPerPage=10;
    _currentPage=0;
  }

void RosterMenuItem::select() {
  _menuSystem->setCurrentItem(this);
  this->display();
}

void RosterMenuItem::display() {
  displayRoster(_label, _currentPage, _itemsPerPage);
}

void RosterMenuItem::handleKeys(char key, KeyState keyState) {
  auto dcc=_menuSystem->getDCCEXProtocol();
  if (keyState==PRESSED) {
    switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': {
        int itemIndex=key-'0';
        itemIndex+=_currentPage*_itemsPerPage;       
        if (!dcc) break;
        int i=0;
        for (Loco* loco=dcc->roster->getFirst(); loco; loco=loco->getNext()) {
          if (i==itemIndex) {
            if (!_selectFunction) break;
            _selectFunction(itemIndex);
          }
          i++;
        }
        break;
      }

      case '*':
        if (this->_parent) {
          _menuSystem->setCurrentItem(this->_parent);
          _parent->select();
        }
        break;
      
      case '#':
        if (!dcc) break;
        _currentPage=(_currentPage+1) % ((int)ceil(dcc->getRosterCount()/(float)_itemsPerPage));
        display();
        break;
      
      default:
        break;
    }
  }
}

// class MenuSystem
MenuSystem::MenuSystem() {
    _currentItem=nullptr;
    _home=nullptr;
    _currentActionItem=nullptr;
    _currentThrottle=-1;
    _dccexProtocol=nullptr;
  }

void MenuSystem::display() {
  if (_currentItem) {
    _currentItem->display();
  }
}

void MenuSystem::handleKeys(char key, KeyState keyState) {
  if (_currentItem) {
    _currentItem->handleKeys(key, keyState);
  }
}

void MenuSystem::setHome(MenuItemBase* home) {
  _home=home;
  _currentItem=home;
  MenuItemBase::setMenuSystem(this);
}

void MenuSystem::goHome() {
  _currentItem=_home;
  displayHome(_powerState);
}

void MenuSystem::setCurrentItem(MenuItemBase* currentItem) {
  _currentItem=currentItem;
}

Menu* MenuSystem::findMenuByLabel(const char* label) {
  ThrottleScreen* home=static_cast<ThrottleScreen*>(_home);
  return _findMenuByLabelRecursive(label, home->getMenu());
}

void MenuSystem::setCurrentActionItem(ActionMenuItem *item) {
  _currentActionItem=item;
}

ActionMenuItem* MenuSystem::getCurrentActionItem() {
  return _currentActionItem;
}

void MenuSystem::setCurrentThrottle(int throttle) {
  _currentThrottle=throttle;
}

int MenuSystem::getCurrentThrottle() {
  return _currentThrottle;
}

bool MenuSystem::isHome() {
  if (_currentItem==_home) return true;
  return false;
}

void MenuSystem::updatePowerState(TrackPower state) {
  _powerState=state;
  _displayPowerState();
}

MenuItemBase* MenuSystem::getCurrentItem() {
  return _currentItem;
}

void MenuSystem::setDCCEXProtocol(DCCEXProtocol* dccexProtocol) {
  _dccexProtocol=dccexProtocol;
}

DCCEXProtocol* MenuSystem::getDCCEXProtocol() {
  return _dccexProtocol;
}

Menu* MenuSystem::_findMenuByLabelRecursive(const char* label, MenuItemBase* currentMenuItem) {
  if (!currentMenuItem) {
    return nullptr; // No items matched label
  }
  if (currentMenuItem->getType()==MenuItemBase::Menu) {
    Menu* currentMenu=static_cast<Menu*>(currentMenuItem);
    if (strcmp(currentMenuItem->getLabel(), label)==0) {
      return currentMenu;
    }
    MenuItemBase* item=currentMenu->getItemList();
    while (item) {
      Menu* foundMenu=_findMenuByLabelRecursive(label, item);
      if (foundMenu) {
        return foundMenu;
      }
      item=item->getNext();
    }
  }
  return _findMenuByLabelRecursive(label, currentMenuItem->getNext());
}

void MenuSystem::_displayPowerState() {
  if (_currentItem==_home) {
    displayPowerState(_powerState);
  }
}
