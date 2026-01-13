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

#include "MenuManager.h"

MenuManager::MenuManager(Logger *logger) : _logger(logger), _currentMenu(nullptr) {}

void MenuManager::handleUserInput(UserInputInterface::UserInputEvent inputEvent) {}

Menu *MenuManager::getCurrentMenu() { return _currentMenu; }

bool MenuManager::isAtRootMenu() { return (_currentMenu == nullptr || _currentMenu->getParent() == nullptr); }

MenuManager::~MenuManager() {}
