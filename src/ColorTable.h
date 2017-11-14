#ifndef COLOR_TABLE_H
#define COLOR_TABLE_H

// color table with predefined colors
static int colorTableSize = 9;
static double colorTable[9][3] = {
    { 0.3, 0.0, 0.0 }, // red
    { 0.0, 0.3, 0.0 }, // green
    { 0.0, 0.0, 0.3 }, // blue
    { 0.3, 0.3, 0.0 }, // yellow
    { 0.0, 0.3, 0.3 }, // cyan
    { 0.3, 0.0, 0.3 }, // violett
    { 0.3, 0.1, 0.1 }, // brighter red
    { 0.1, 0.3, 0.1 }, // brighter green
    { 0.1, 0.1, 0.3 }  // brighter blue
};

#endif // COLOR_TABLE_H
