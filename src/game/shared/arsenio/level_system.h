#ifndef LEVEL_SYSTEM_H
#define LEVEL_SYSTEM_H
#ifdef _WIN32
#pragma once
#endif

// Function prototypes
void SaveLevelData(int level, int xp);
bool LoadLevelData(int& level, int& xp);

#endif // LEVEL_SYSTEM_H
