#include "DisplayBuffers.h"
#include <vector>

Segment gAllSegments[36] = {
    // ===================
    // OCTA SEGMENTS
    // all segments direction = coming forward or clockwise

    // back legs, clockwise from lower left
    Segment(36, true),
    Segment(28, false),
    Segment(25, false),
    Segment(27, true),

    // square, clockwise from lower left
    Segment(32, true),
    Segment(29, false),
    Segment(26, false),
    Segment(35, false),

    // front legs, clockwise from lower left
    Segment(33, false),
    Segment(31, true),
    Segment(30, false),
    Segment(34, true),

    // ===================
    // MINI TETRA SEGMENTS
    // all segments direction = outside coming in.

    // down tetra
    // top front left (j)
    Segment(4, false), Segment(6, false), Segment(3, true),
    // top back (g)
    Segment(5, true), Segment(12, false), Segment(11, true),
    // top front right (h)
    Segment(10, false), Segment(7, true), Segment(8, false),
    // bottom (i)
    Segment(2, false), Segment(1, true), Segment(9, true),

    // up tetra
    // bottom back left (l)
    Segment(23, true), Segment(24, false), Segment(19, true),
    // bottom back right (n)
    Segment(14, false), Segment(13, true), Segment(16, false),
    // bottom front (k)
    Segment(17, true), Segment(20, false), Segment(18, false),
    // top (m)
    Segment(22, false), Segment(21, true), Segment(15, true),
};

Segment* const gOctaSegments = gAllSegments;

Segment* const gMiniTetraSegments = gAllSegments + 12;

std::vector<std::vector<Segment>> gOrbits = {
    // ===================
    // OCTA SQUARES

    // square right tilting
    {
        Segment(30, false),
        Segment(33, true),
        Segment(36, false),
        Segment(25, false),
    },

    // square left tilting
    {
        Segment(31, true),
        Segment(34, false),
        Segment(27, false),
        Segment(28, false),
    },

    // square back tilting
    {
        Segment(29, false),
        Segment(26, false),
        Segment(35, false),
        Segment(32, true),
    },

    // ===
    // BIG TETRA UP FACES
    // left face
    {
        Segment(22, false),
        Segment(23, false),
        Segment(19, true),
        Segment(18, true),
        Segment(20, false),
        Segment(21, false),
    },

    // right face
    {
        Segment(21, true),
        Segment(20, true),
        Segment(17, true),
        Segment(16, true),
        Segment(14, false),
        Segment(15, false),
    },

    // back face
    {
        Segment(22, false),
        Segment(23, false),
        Segment(24, false),
        Segment(13, false),
        Segment(14, false),
        Segment(15, false),
    },

    // bottom face
    {
        Segment(19, true),
        Segment(18, true),
        Segment(17, true),
        Segment(16, true),
        Segment(13, true),
        Segment(24, true),
    },


    // ===
    // BIG TETRA DOWN FACES
    // front face
    {
        Segment(6, false),
        Segment(7, false),
        Segment(8, false),
        Segment(9, false),
        Segment(2, false),
        Segment(3, false),
    },

    // top face
    {
        Segment(6, false),
        Segment(7, false),
        Segment(10, false),
        Segment(11, false),
        Segment(5, true),
        Segment(4, true),
    },

    // left face
    {
        Segment(5, true),
        Segment(4, true),
        Segment(3, true),
        Segment(2, true),
        Segment(1, true),
        Segment(12, true),
    },

    // right face
    {
        Segment(12, false),
        Segment(1, false),
        Segment(9, true),
        Segment(8, true),
        Segment(10, false),
        Segment(11, false),
    },
};
