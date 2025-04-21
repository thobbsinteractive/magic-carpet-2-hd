#include "Player.h"

#include "../utilities/Maths.h"

//----- (00057FA0) --------------------------------------------------------
void MovePlayer_57FA0(axis_3d* position, uint16_t a2, int16_t a3, int16_t a4)//238fa0 // move player
{
	if (a4)
	{
		a3 &= 0x7ffu;
		a2 &= 0x7ffu;
		if (a3)
		{
			position->z -= (int)(a4 * Maths::sin_DB750[a3]) >> 16;//change z axis
			a4 = (int)(a4 * Maths::sin_DB750[0x200 + a3]) >> 16;
		}
		position->x += (int)(a4 * Maths::sin_DB750[a2]) >> 16;
		position->y -= (int)(a4 * Maths::sin_DB750[0x200 + a2]) >> 16;
	}
}
