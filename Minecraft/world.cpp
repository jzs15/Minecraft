#include "world.h"

World::World() {
	seed = time(NULL);
	for (int x = 0; x < SCX; x++)
	{
		for (int y = 0; y < SCY; y++)
		{
			for (int z = 0; z < SCZ; z++)
			{
				c[x][y][z] = new Chunk(x - SCX / 2, y - SCY / 2, z - SCZ / 2);
			}
		}
	}

	for (int x = 0; x < SCX; x++)
	{
		for (int y = 0; y < SCY; y++)
		{
			for (int z = 0; z < SCZ; z++) {
				if (x > 0)
					c[x][y][z]->left = c[x - 1][y][z];
				if (x < SCX - 1)
					c[x][y][z]->right = c[x + 1][y][z];
				if (y > 0)
					c[x][y][z]->below = c[x][y - 1][z];
				if (y < SCY - 1)
					c[x][y][z]->above = c[x][y + 1][z];
				if (z > 0)
					c[x][y][z]->front = c[x][y][z - 1];
				if (z < SCZ - 1)
					c[x][y][z]->back = c[x][y][z + 1];
			}
		}
	}
}


World::~World()
{
}

uint8_t World::get(int x, int y, int z) const {
	int cx = (x + CX * (SCX / 2));
	int cy = (y + CY * (SCY / 2));
	int cz = (z + CZ * (SCZ / 2));
	cx = cx < 0 ? cx / CX - 1 : cx / CX;
	cy = cy < 0 ? cy / CY - 1 : cy / CY;
	cz = cz < 0 ? cz / CZ - 1 : cz / CZ;

	if (cx < 0 || cx >= SCX || cy < 0 || cy >= SCY || cz < 0 || cz >= SCZ) {
		return 0;
	}


	return c[cx][cy][cz]->get(x & (CX - 1), y & (CY - 1), z & (CZ - 1));
}

bool World::isBlock(int x, int y, int z)
{
	uint8_t block = get(x, y, z);
	return (block && transparent[block] != 5);
}

void World::set(int x, int y, int z, uint8_t type) {
	int cx = (x + CX * (SCX / 2));
	int cy = (y + CY * (SCY / 2));
	int cz = (z + CZ * (SCZ / 2));
	cx = cx < 0 ? (cx - 1) / CX : cx / CX;
	cy = cy < 0 ? (cy - 1) / CY : cy / CY;
	cz = cz < 0 ? (cz - 1) / CX : cz / CZ;

	if (cx < 0 || cx >= SCX || cy < 0 || cy >= SCY || cz < 0 || cz >= SCZ)
		return;

	c[cx][cy][cz]->set(x & (CX - 1), y & (CY - 1), z & (CZ - 1), type);
}

bool World::canSetBlock(int x, int y, int z, uint8_t blk)
{
	if (get(x, y, z))
		return false;
	if (transparent[blk] == 5)
		return isBlock(x, y - 1, z);
	return isBlock(x, y, z + 1) || isBlock(x, y, z - 1) || isBlock(x, y + 1, z) || isBlock(x, y - 1, z) ||
		isBlock(x + 1, y, z) || isBlock(x - 1, y, z);

}

void World::render(const glm::mat4 &pv) {
	float ud = FLT_MAX;
	int ux = -1;
	int uy = -1;
	int uz = -1;

	for (int x = 0; x < SCX; x++) {
		for (int y = 0; y < SCY; y++) {
			for (int z = 0; z < SCZ; z++) {
				glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(c[x][y][z]->ax * CX, c[x][y][z]->ay * CY, c[x][y][z]->az * CZ));
				glm::mat4 mvp = pv * model;
				glm::vec4 center = mvp * glm::vec4(CX / 2, CY / 2, CZ / 2, 1);

				float d = glm::length(center);
				center.x /= center.w;
				center.y /= center.w;
				center.z /= center.w;

				if (!is_zoom && (fabsf(center.x) > 1 + fabsf(CX * 2 / center.w) || fabsf(center.y) > 1 + fabsf(CY * 2 / center.w) || fabsf(center.z) > 1 + fabsf(CZ * 2 / center.w)))
				{
					continue;
				}

				if (!c[x][y][z]->initialized) {
					if (ux < 0 || d < ud) {
						ud = d;
						ux = x;
						uy = y;
						uz = z;
					}
					continue;
				}

				glUniformMatrix4fv(glGetUniformLocation(cur_program, "pv"), 1, GL_FALSE, glm::value_ptr(pv));
				glUniformMatrix4fv(glGetUniformLocation(cur_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
				c[x][y][z]->render();
			}
		}
	}

	if (ux >= 0) {
		c[ux][uy][uz]->noise(seed);
		if (c[ux][uy][uz]->left)
			c[ux][uy][uz]->left->noise(seed);
		if (c[ux][uy][uz]->right)
			c[ux][uy][uz]->right->noise(seed);
		if (c[ux][uy][uz]->below)
			c[ux][uy][uz]->below->noise(seed);
		if (c[ux][uy][uz]->above)
			c[ux][uy][uz]->above->noise(seed);
		if (c[ux][uy][uz]->front)
			c[ux][uy][uz]->front->noise(seed);
		if (c[ux][uy][uz]->back)
			c[ux][uy][uz]->back->noise(seed);
		c[ux][uy][uz]->initialized = true;
	}
}