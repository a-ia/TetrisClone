#include "Piece.h"

Piece::Piece() {}

Piece::Piece(byte ax, byte ay, byte bx, byte by, byte cx, byte cy, byte dx, byte dy, byte col)
    : a(Vec2(ax, ay)), b(Vec2(bx, by)), c(Vec2(cx, cy)), d(Vec2(dx, dy)), color(col) {}

Piece::Piece(Vec2 da, Vec2 db, Vec2 dc, Vec2 dd, byte col)
    : a(da), b(db), c(dc), d(dd), color(col) {}

Piece CreatePiece(PIECE_TYPE type)
{
    switch (type)
    {
    case S: return Piece(Vec2(1, 8), Vec2(1, 7), Vec2(0, 7), Vec2(2, 8), 0);
    case Z: return Piece(Vec2(1, 7), Vec2(1, 8), Vec2(0, 8), Vec2(2, 7), 1);
    case L: return Piece(Vec2(1, 8), Vec2(1, 7), Vec2(1, 9), Vec2(0, 9), 2);
    case J: return Piece(Vec2(1, 8), Vec2(1, 7), Vec2(1, 9), Vec2(0, 7), 3);
    case SQR: return Piece(Vec2(1, 8), Vec2(1, 7), Vec2(0, 8), Vec2(0, 7), 4);
    case I: return Piece(Vec2(1, 8), Vec2(1, 7), Vec2(1, 9), Vec2(1, 10), 5);
    case T: return Piece(Vec2(1, 8), Vec2(1, 7), Vec2(1, 9), Vec2(0, 8), 6);
    }
    return Piece();
}

sf::Color getPieceColor(byte colorIndex) {
    switch (colorIndex) {
        case 0: return sf::Color(0, 255, 0);    // S - Green
        case 1: return sf::Color(255, 0, 0);    // Z - Red
        case 2: return sf::Color(255, 165, 0);  // L - Orange
        case 3: return sf::Color(0, 0, 255);    // J - Blue
        case 4: return sf::Color(255, 255, 0);  // SQR - Yellow
        case 5: return sf::Color(0, 255, 255);  // I - Cyan
        case 6: return sf::Color(255, 0, 255);  // T - Magenta
        default: return sf::Color(100, 100, 100); // Default gray
    }
}

bool rotatePiece(Piece& p, const byte colliders[][16])
{
    // Store original position in case rotation is invalid
    Piece original = p;
    
    // Pivot point is p.a
    int px = p.a.x;
    int py = p.a.y;
    
    // Rotate each point except the pivot
    // For clockwise rotation: (x,y) -> (px + (y-py), py - (x-px))
    
    // Pivot point (p.a) stays the same
    
    // Rotate p.b
    byte bx_new = px + (p.b.y - py);
    byte by_new = py - (p.b.x - px);
    p.b.x = bx_new;
    p.b.y = by_new;
    
    // Rotate p.c
    byte cx_new = px + (p.c.y - py);
    byte cy_new = py - (p.c.x - px);
    p.c.x = cx_new;
    p.c.y = cy_new;
    
    // Rotate p.d
    byte dx_new = px + (p.d.y - py);
    byte dy_new = py - (p.d.x - px);
    p.d.x = dx_new;
    p.d.y = dy_new;
    
    // Check if rotation is valid (within bounds and not colliding)
    if (p.a.x >= 16 || p.b.x >= 16 || p.c.x >= 16 || p.d.x >= 16 ||
        p.a.y >= 20 || p.b.y >= 20 || p.c.y >= 20 || p.d.y >= 20 ||
        p.a.x < 0 || p.b.x < 0 || p.c.x < 0 || p.d.x < 0 ||
        colliders[p.a.y][p.a.x] == 2 || colliders[p.b.y][p.b.x] == 2 ||
        colliders[p.c.y][p.c.x] == 2 || colliders[p.d.y][p.d.x] == 2)
    {
        // Rotation is invalid, revert to original position
        p = original;
        return false;
    }
    
    return true;
}
