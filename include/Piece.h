#pragma once
#include "Utils.h"
#include <SFML/Graphics.hpp>

struct Piece
{
    Vec2 a, b, c, d;
    byte color;
    
    Piece();
    Piece(byte ax, byte ay, byte bx, byte by, byte cx, byte cy, byte dx, byte dy, byte col = 0);
    Piece(Vec2 da, Vec2 db, Vec2 dc, Vec2 dd, byte col = 0);
};

// Create a piece based on type
Piece CreatePiece(PIECE_TYPE type);

// Get color for a piece
sf::Color getPieceColor(byte colorIndex);

// Rotate a piece
bool rotatePiece(Piece& p, const byte colliders[][16]);
