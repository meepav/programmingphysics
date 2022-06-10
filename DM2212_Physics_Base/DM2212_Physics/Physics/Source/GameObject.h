#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"

struct GameObject
{
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		GO_BALL,
		GO_CUBE,
		GO_SHIP,
		GO_BULLET,
		GO_BLACKHOLE,
		GO_WHITEHOLE,

		// enemy
		GO_ENEMY,
		GO_ENEMYBULLET,

		// powerups
		GO_POWERUPHEAL,
		GO_POWERUPCANON,
		GO_POWERUPDAMAGE,

		// asteroids
		GO_ASTEROID,
		GO_ASTEROID_SMALL,

		GO_TEST,
		GO_PH, //heal
		GO_PC, //canon
		GO_PD, //damage
		GO_CANON,

		GO_TOTAL, //must be last
	};
	GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	bool active;
	float mass;
	float angle;
	float hp;
	float points;
	float timer;
	float firerate;
	Vector3 force;
	Vector3 torque;

	Vector3 direction;
	float momentOfInertia;
	float angularVelocity;

	GameObject(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~GameObject();
};

#endif