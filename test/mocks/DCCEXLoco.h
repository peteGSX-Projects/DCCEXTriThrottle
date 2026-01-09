/* -*- c++ -*-
 *
 * DCCEXProtocol
 *
 * This package implements a DCCEX native protocol connection,
 * allow a device to communicate with a DCC-EX EX-CommandStation.
 *
 * Copyright © 2024 Peter Cole
 * Copyright © 2023 Peter Akers
 * Copyright © 2023 Peter Cole
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike
 * 4.0 International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-sa/4.0/ or send a letter to
 * Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
 *
 * Attribution — You must give appropriate credit, provide a link to the
 * license, and indicate if changes were made. You may do so in any
 * reasonable manner, but not in any way that suggests the licensor
 * endorses you or your use.
 *
 * ShareAlike — If you remix, transform, or build upon the material, you
 * must distribute your contributions under the same license as the
 * original.
 *
 * All other rights reserved.
 *
 */

/**
 * @brief Consolidation of DCCEXLoco.h/cpp to save mocking the whole thing
 */

#ifndef DCCEXLOCO_H
#define DCCEXLOCO_H

#include <Arduino.h>

static const int MAX_FUNCTIONS = 32;
const int MAX_OBJECT_NAME_LENGTH = 30;      // including Loco name, Turnout/Point names, Route names, etc. names
#define MAX_SINGLE_COMMAND_PARAM_LENGTH 500 // Unfortunately includes the function list for an individual loco

enum Direction {
  Reverse = 0,
  Forward = 1,
};

enum LocoSource {
  LocoSourceRoster = 0,
  LocoSourceEntry = 1,
};

enum Facing {
  FacingForward = 0,
  FacingReversed = 1,
};

/// @brief Class for a Loco object representing a DCC addressed locomotive
class Loco {
public:
  /// @brief Constructor
  /// @param address DCC address of loco
  /// @param source LocoSourceRoster (from roster) or LocoSourceEntry (from user
  /// input)
  Loco(int address, LocoSource source) : _address(address), _source(source) {
    for (int i = 0; i < MAX_FUNCTIONS; i++) {
      _functionNames[i] = nullptr;
    }
    _direction = Forward;
    _speed = 0;
    _name = nullptr;
    _functionStates = 0;
    _momentaryFlags = 0;
    _next = nullptr;
    if (_source == LocoSource::LocoSourceRoster) {
      if (!_first) {
        _first = this;
      } else {
        Loco *current = _first;
        while (current->_next != nullptr) {
          current = current->_next;
        }
        current->_next = this;
      }
    }
  }

  /// @brief Get loco address
  /// @return DCC address of loco
  int getAddress() { return _address; }

  /// @brief Set loco name
  /// @param name Name of the loco
  void setName(const char *name) {
    if (_name) {
      delete[] _name;
      _name = nullptr;
    }
    int nameLength = strlen(name);
    _name = new char[nameLength + 1];
    strcpy(_name, name);
  }

  /// @brief Get loco name
  /// @return Name of the loco
  const char *getName() { return _name; }

  /// @brief Set loco speed
  /// @param speed Valid speed (0 - 126)
  void setSpeed(int speed) { _speed = speed; }

  /// @brief Get loco speed
  /// @return Speed (0 - 126)
  int getSpeed() { return _speed; }

  /// @brief Set loco direction (enums Forward, Reverse)
  /// @param direction Direction to set (Forward|Reverse)
  void setDirection(Direction direction) { _direction = direction; }

  /// @brief Get loco direction (enums Forward, Reverse)
  /// @return Current direction (Forward|Reverse)
  Direction getDirection() { return (Direction)_direction; }

  /// @brief Get loco source (enums LocoSourceRoster, LocoSourceEntry)
  /// @return Source of loco (LocoSourceRoster|LocoSourceEntry)
  LocoSource getSource() { return (LocoSource)_source; }

  /// @brief Setup functions for the loco
  /// @param functionNames Char array of function names
  void setupFunctions(const char *functionNames) {
    if (functionNames == nullptr) {
      return;
    }
    // Copy functionNames so we can clean up later
    char *fNames = new char[strlen(functionNames) + 1];
    if (fNames == nullptr) {
      return; // Bail out if malloc failed
    }
    strcpy(fNames, functionNames); // Copy names

    // Remove any existing names first
    for (int nameIndex = 0; nameIndex < MAX_FUNCTIONS; nameIndex++) {
      if (_functionNames[nameIndex] != nullptr) {
        delete[] _functionNames[nameIndex];
        _functionNames[nameIndex] = nullptr;
      }
    }

    int fNameIndex = 0;                // Index for each function name
    int fNamesLength = strlen(fNames); // Length of all names for sizing later
    int fNameStartChar = 0;            // Position of the first char in the name

    // Iterate through the fNames char array to look for names
    for (int charIndex = 0; charIndex <= fNamesLength; charIndex++) {
      // End of name is either / or null terminator
      // Start of name will be in char array index fNameStart
      if (fNames[charIndex] == '/' || fNames[charIndex] == '\0') {
        // Make sure we're at a sane index
        if (fNameIndex < MAX_FUNCTIONS) {
          bool momentary = false;
          // If start is *, it's momentary, name starts at following index
          if (fNames[fNameStartChar] == '*') {
            momentary = true;
            fNameStartChar++;
          }
          int nameLength = charIndex - fNameStartChar;           // Calculate length of name
          _functionNames[fNameIndex] = new char[nameLength + 1]; // Allocate mem + null terminator
          if (_functionNames[fNameIndex] != nullptr) {
            // Copy the name to the array index and null terminate it
            strncpy(_functionNames[fNameIndex], &fNames[fNameStartChar], nameLength);
            _functionNames[fNameIndex][nameLength] = '\0';
          }
          // Set the momentary flag
          if (momentary) {
            _momentaryFlags |= 1 << fNameIndex;
          } else {
            _momentaryFlags &= ~(1 << fNameIndex);
          }
          // Move to the next index
          fNameIndex++;
        } else {
          break;
        }
        fNameStartChar = charIndex + 1; // Calculate the start index of the next name
      }
    }
    delete[] fNames; // Clean up fNames
  }

  /// @brief Test if function is on
  /// @param function Number of the function to test
  /// @return true|false
  bool isFunctionOn(int function) { return _functionStates & 1 << function; }

  /// @brief Set function states
  /// @param functionStates Integer representing all function states
  void setFunctionStates(int functionStates) { _functionStates = functionStates; }

  /// @brief Get function states
  /// @return Integer representing current function states
  int getFunctionStates() { return _functionStates; }

  /// @brief Get the name/label for a function
  /// @param function Number of the function to return the name/label of
  /// @return char* representing the function name/label
  const char *getFunctionName(int function) { return _functionNames[function]; }

  /// @brief Get the name/label for a function
  /// @param function Number of the function to return the name/label of
  /// @return char* representing the function name/label
  bool isFunctionMomentary(int function) { return _momentaryFlags & 1 << function; }

  /// @brief Get first Loco object
  /// @return Pointer to the first Loco object
  static Loco *getFirst() { return _first; }

  /// @brief Set the next loco in the roster list
  /// @param loco Pointer to the next Loco object
  void setNext(Loco *loco) { _next = loco; }

  /// @brief Get next Loco object
  /// @return Pointer to the next Loco object
  Loco *getNext() { return _next; }

  /// @brief Get Loco object by its DCC address
  /// @param address DCC address of the loco to get
  /// @return Loco object or nullptr if it doesn't exist
  static Loco *getByAddress(int address) {
    for (Loco *l = getFirst(); l; l = l->getNext()) {
      if (l->getAddress() == address) {
        return l;
      }
    }
    return nullptr;
  }

  /// @brief Clear all Locos from the roster
  static void clearRoster() {
    // Count Locos in roster
    int locoCount = 0;
    Loco *currentLoco = Loco::getFirst();
    while (currentLoco != nullptr) {
      locoCount++;
      currentLoco = currentLoco->getNext();
    }

    // Store Loco pointers in an array for clean up
    Loco **deleteLocos = new Loco *[locoCount];
    currentLoco = Loco::getFirst();
    for (int i = 0; i < locoCount; i++) {
      deleteLocos[i] = currentLoco;
      currentLoco = currentLoco->getNext();
    }

    // Delete each Loco
    for (int i = 0; i < locoCount; i++) {
      delete deleteLocos[i];
    }

    // Clean up the array of pointers
    delete[] deleteLocos;

    // Reset first pointer
    Loco::_first = nullptr;
  }

  /// @brief Destructor for the Loco object
  ~Loco() {
    _removeFromList(this);

    if (_name) {
      delete[] _name;
      _name = nullptr;
    }

    for (int i = 0; i < MAX_FUNCTIONS; i++) {
      if (_functionNames[i]) {
        delete[] _functionNames[i];
        _functionNames[i] = nullptr;
      }
    }

    _next = nullptr;
  }

private:
  int _address;
  char *_name;
  int _speed;
  Direction _direction;
  LocoSource _source;
  char *_functionNames[MAX_FUNCTIONS];
  int32_t _functionStates;
  int32_t _momentaryFlags;
  inline static Loco *_first = nullptr;
  Loco *_next;

  /// @brief Method to remove this loco from the roster list
  /// @param loco Pointer to the Loco to remove
  static void _removeFromList(Loco *loco) {
    if (!loco) {
      return;
    }

    if (getFirst() == loco) {
      _first = loco->getNext();
    } else {
      Loco *currentLoco = _first;
      while (currentLoco && currentLoco->getNext() != loco) {
        currentLoco = currentLoco->getNext();
      }
      if (currentLoco) {
        currentLoco->setNext(loco->getNext());
      }
    }
  }

  friend class Consist;
};

/// @brief Class to add an additional attribute to a Loco object to specify the direction it is facing in a consist
class ConsistLoco {
public:
  /// @brief Constructor
  /// @param loco Pointer to the Loco object to add
  /// @param facing Direction loco is facing in the consist
  /// (FacingForward|FacingReversed)
  ConsistLoco(Loco *loco, Facing facing);

  /// @brief Get the associated Loco object for this consist entry
  /// @return Pointer to the Loco object
  Loco *getLoco() { return nullptr; }

  /// @brief Set which way the loco is facing in the consist (FacingForward,
  /// FacingReversed)
  /// @param facing FacingForward|FacingReversed
  void setFacing(Facing facing);

  /// @brief Get which way the loco is facing in the consist (FacingForward,
  /// FacingReversed)
  /// @return FacingForward|FacingReversed
  Facing getFacing();

  /// @brief Get the next consist loco object
  /// @return Pointer to the next ConsistLoco object
  ConsistLoco *getNext();

  /// @brief Set the next consist loco object
  /// @param consistLoco Pointer to the ConsistLoco object
  void setNext(ConsistLoco *consistLoco);

  /// @brief Destructor for a ConsistLoco
  ~ConsistLoco();

private:
  Loco *_loco;
  Facing _facing;
  ConsistLoco *_next;

  friend class Consist;
};

/// @brief Class to create a software consist of one or more ConsistLoco objects
class Consist {
public:
  /// @brief Constructor
  Consist();

  /// @brief Set consist name
  /// @param name Name to set for the consist
  void setName(const char *name);

  /// @brief Get consist name
  /// @return Current name of the consist
  const char *getName();

  /// @brief Add a loco to the consist using a Loco object
  /// @param loco Pointer to a loco object
  /// @param facing Direction the loco is facing (FacingForward | FacingReversed)
  void addLoco(Loco *loco, Facing facing);

  /// @brief Add a loco to the consist using a DCC address
  /// @param address DCC address of the loco to add
  /// @param facing Direction the loco is facing (FacingForward | FacingReversed)
  void addLoco(int address, Facing facing);

  /// @brief Remove a loco from the consist - Loco objects with LocoSourceEntry will also be deleted
  /// @param loco Pointer to a loco object to remove
  void removeLoco(Loco *loco);

  /// @brief Remove all locos from a consist - Loco objects with LocoSourceEntry
  /// will also be deleted
  void removeAllLocos();

  /// @brief Update the direction of a loco in the consist
  /// @param loco Pointer to the loco object to update
  /// @param facing Direction to set it to (FacingForward|FacingReversed)
  void setLocoFacing(Loco *loco, Facing facing);

  /// @brief Get the count of locos in the consist
  /// @return Count of locos
  int getLocoCount();

  /// @brief Check if the provided loco is in the consist
  /// @param loco Pointer to the loco object to check
  /// @return true|false
  bool inConsist(Loco *loco);

  /// @brief Check if the loco with the provided address is in the consist
  /// @param address DCC address of loco to check
  /// @return true|false
  bool inConsist(int address);

  /// @brief Get consist speed - obtained from first linked loco
  /// @return Current speed (0 - 126)
  int getSpeed() {
    ConsistLoco *cl = _first;
    if (!cl)
      return 0;
    return cl->getLoco()->getSpeed();
  }

  /// @brief Get consist direction - obtained from first linked loco
  /// @return Current direction (Forward|Reverse)
  Direction getDirection();

  /// @brief Get the first loco in the consist
  /// @return Pointer to the first ConsistLoco object
  ConsistLoco *getFirst();

  /// @brief Get the loco in the consist with the specified address
  /// @param address DCC address of loco to retrieve
  /// @return Pointer to the first ConsistLoco object
  ConsistLoco *getByAddress(int address);

  /// @brief Destructor for a Consist
  ~Consist();

private:
  char *_name;
  int _locoCount;
  ConsistLoco *_first;

  void _addLocoToConsist(ConsistLoco *consistLoco);
};

#endif