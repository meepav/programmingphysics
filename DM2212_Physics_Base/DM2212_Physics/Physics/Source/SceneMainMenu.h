#ifndef SCENE_MAINMENU_H
#define SCENE_MAINMENU_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

class SceneMainMenu : public SceneBase
{
	static const int MAX_SPEED = 10;
	static const int BULLET_SPEED = 50;
	static const int MISSILE_SPEED = 20;
	static const int MISSILE_POWER = 1;

	static const float ROTATION_SPEED;
	static const float MAX_ROTATION_SPEED;
	static const float GRAVITY_CONSTANT;

public:
	SceneMainMenu();
	~SceneMainMenu();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);
	float CalculateAdditionalForce(GameObject* go1, GameObject* go2);
	//In .h, outside class
	void Wrap(float& val, float bound);

	GameObject* FetchGO();
protected:

	//Physics
	std::vector<GameObject *> m_goList;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	GameObject *m_ship;
	Vector3 m_force; // force for linear displacement
	Vector3 m_torque;
	int m_objectCount;
	int m_lives;
	int m_score;

	//powerup
	bool powerUpActivated;
	bool powerUpHealActivated;
	bool powerUpCanonActivated;
	bool powerUpDamageActivated;
	int powerUpHeal;
	int powerUpCanon;
	int powerUpDamage;
	int powerUpPoints;

	//enemies
	int m_enemy_lives;
	int m_enemy_points;
	int m_enemy_damage;

	double prevElapsed;
	double elapsedTime;
};
static Vector3 RotateVector(const Vector3& vec, float radian);

#endif