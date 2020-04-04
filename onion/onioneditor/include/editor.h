#pragma once

#include <map>
#include "gui.h"


// Contains all data about the game.
class Data
{
private:
	// A mapping from a path to the sprite sheet loaded from it.
	static std::unordered_map<std::string, SpriteSheet*> m_SpriteSheets;

	// A set of chunk names.
	static std::map<CHUNK_KEY, std::string> m_Chunks;

public:
	/// <summary>Saves everything the game needs to know to the data files.</summary>
	static void save();

	/// <summary>Loads all data from the data files.</summary>
	static void load();
};


// Used to edit stuff.
class Editor
{
public:
	/// <summary>Displays the editor.</summary>
	virtual void display() = 0;

	/// <summary>Freezes input for the editor.</summary>
	virtual void freeze() = 0;

	/// <summary>Unfreezes input for the editor.</summary>
	virtual void unfreeze() = 0;
};