#include "SceneAsteroid.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>
#include <iomanip>

const float SceneAsteroid::ROTATION_SPEED = 4.0f;
const float SceneAsteroid::MAX_ROTATION_SPEED = 5.0f;
const float SceneAsteroid::GRAVITY_CONSTANT = 3.0f;

SceneAsteroid::SceneAsteroid()
{
}

SceneAsteroid::~SceneAsteroid()
{
}

void SceneAsteroid::Init()
{
	SceneBase::Init();

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;
	
	Math::InitRNG();


	//Exercise 2a: Construct 100 GameObject with type GO_ASTEROID and add into m_goList
	for (int i = 0; i < 100; ++i) {
		m_goList.push_back(new GameObject(GameObject::GO_ASTEROID));
	}

	//Exercise 2b: Initialize m_lives and m_score
	m_lives = 100;
	m_score = 0;

	//initialise powerups
	powerUpActivated = false;
	powerUpHealActivated = false;
	powerUpDamageActivated = false;
	powerUpCanonActivated = false;

	powerUpHeal = 50;
	powerUpDamage = 20;
	powerUpPoints = 100;

	//initialise enemies
	m_enemy_lives = 100;
	m_enemy_points = 50;
	m_enemy_damage = 5;

	m_player_damage = 10;

	//initialise asteroids
	m_asteroid_lives = 50;
	m_asteroid_small_lives = 25;
	m_asteroid_points = 20;

	//Exercise 2c: Construct m_ship, set active, type, scale and pos
	m_ship = new GameObject(GameObject::GO_SHIP);
	m_ship->active = true;
	m_ship->scale.Set(2.5, 2.5, 1);
	m_ship->pos.Set(m_worldWidth / 2, m_worldHeight / 2, 1);
	m_ship->vel.Set(0, 0, 0);
	m_ship->direction.Set(-1, 0, 0); // direction of ship thats facing

	m_ship->momentOfInertia = m_ship->mass * m_ship->scale.x * m_ship->scale.x;
	m_ship->angularVelocity = 0;
	angularvel = 0;

	m_torque.SetZero();

	prevElapsed = elapsedTime = 0.0;
	timer = 0;
	poweruptimer = Math::RandIntMinMax(1, 10);
	activatehealtimer = 0;
	activatecanontimer = 0;
	activatedamagetimer = 0;
	enemytimer = 0;
	survive = 0;
}

GameObject* SceneAsteroid::FetchGO()
{
	//Exercise 3a: Fetch a game object from m_goList and return it
	//Exercise 3a: Fetch a game object from m_goList and return it
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active) {
			continue;
		}
		go->active = true;
		return go;
	}
	//Get Size before adding 10
	int prevSize = m_goList.size();
	for (int i = 0; i < 10; ++i) {
		m_goList.push_back(new GameObject(GameObject::GO_ASTEROID));
	}
	m_goList.at(prevSize)->active = true;
	return m_goList.at(prevSize);

	//Get Previous Size
	//Return go at the previous size
	//b. Handle the situation whenever m_goList runs out of objects (hint: create in batch of 10s)
}

void SceneAsteroid::Update(double dt)
{
	SceneBase::Update(dt);
	elapsedTime += dt;
	timer += dt;
	poweruptimer -= dt;
	enemyspawn -= dt;
	enemytimer -= dt;
	survive += dt;
	if (powerUpHealActivated == true)
	{
		activatehealtimer += dt;
		if (activatehealtimer >= 5)
		{
			powerUpHealActivated = false;
			activatehealtimer = 0;
		}
	}	
	if (powerUpCanonActivated == true)
	{
		activatecanontimer += dt;
		if (activatecanontimer >= 5)
		{
			powerUpCanonActivated = false;
			activatecanontimer = 0;
		}
	}
	if (powerUpDamageActivated == true)
	{
		activatedamagetimer += dt;
		if (activatedamagetimer >= 5)
		{
			powerUpDamageActivated = false;
			activatedamagetimer = 0;
		}
	}

	if (m_ship->active == false)
	{
		Application::SceneManager(4);
	}
	if (survive >= 100 && m_ship->active == true)
	{
		Application::SceneManager(3);
	}

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	
	if(Application::IsKeyPressed('9'))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if(Application::IsKeyPressed('0'))
	{
		m_speed += 0.1f;
	}
	m_force.SetZero();
	m_torque.SetZero();
	//Exercise 6: set m_force values based on WASD
	if (Application::IsKeyPressed('W'))
	{
		m_force += m_ship->direction * 100.0f;
	}
	if (Application::IsKeyPressed('A'))
	{
		m_force += m_ship->direction * ROTATION_SPEED;
		m_torque += Vector3(-m_ship->scale.x, -m_ship->scale.y, 0).Cross(Vector3(ROTATION_SPEED, 0, 0));
		if (angularvel == 2)
		{
			m_ship->angularVelocity = 0;
		}
		angularvel = 1;
		
	}
	if (Application::IsKeyPressed('S'))
	{
		m_force -= m_ship->direction * 100.0f;
	}
	if (Application::IsKeyPressed('D'))
	{
		m_force += m_ship->direction * ROTATION_SPEED;
		m_torque += Vector3(-m_ship->scale.x, m_ship->scale.y, 0).Cross(Vector3(ROTATION_SPEED, 0, 0));
		if (angularvel == 1)
		{
			m_ship->angularVelocity = 0;
		}
		angularvel = 2;
		
	}
	//Exercise 8: use 2 keys to increase and decrease mass of ship
	if (Application::IsKeyPressed('O'))
	{
		m_ship->mass += 1.0f * dt;
		m_ship->momentOfInertia = m_ship->mass * m_ship->scale.x * m_ship->scale.x;
	}
	if (Application::IsKeyPressed('P'))
	{
		m_ship->mass -= 1.0f * dt;
		if (m_ship->mass >= 0.1f)
		{
			m_ship->mass = 0.1f;
		}
		m_ship->momentOfInertia = m_ship->mass * m_ship->scale.x * m_ship->scale.x;
	}

	if (timer >= 2)
	{
		//Spawn random asteroids
		for (int i = 0; i < 3; ++i)
		{
			//Create Asteroid
			GameObject* go = FetchGO();
			go->type = GameObject::GO_ASTEROID;
			go->pos.Set(0, Math::RandFloatMinMax(0, m_worldHeight), 0);
			go->vel.Set(Math::RandFloatMinMax(10, 20), 0, 0);
			go->scale.Set(6.0f, 6.0f, 1);
			go->hp = 50;
			go->points = 20;
		}
		for (int i = 0; i < 3; ++i)
		{
			//Create Asteroid
			GameObject* go = FetchGO();
			go->type = GameObject::GO_ASTEROID_SMALL;
			go->pos.Set(0, Math::RandFloatMinMax(0, m_worldHeight), 0);
			go->vel.Set(Math::RandFloatMinMax(10, 20), 0, 0);
			go->scale.Set(3.0f, 3.0f, 1);
			go->hp = 25;
			go->points = 20;
		}
		timer = 0;
	}

	// spawning powerups at random timings
	if (poweruptimer <= 0)
	{
		powerup = Math::RandIntMinMax(1, 3);
		if (powerup == 1)
		{
			//heal
			GameObject* heal = FetchGO();
			heal->type = GameObject::GO_PH;
			heal->pos.Set(0, Math::RandFloatMinMax(0, m_worldHeight), 0);
			heal->vel.Set(Math::RandFloatMinMax(10, 20), 0, 0);
			heal->scale.Set(5.0f, 5.0f, 1.0f);
			heal->points = 100;
			heal->active = true;
		}
		else if (powerup == 2)
		{
			//canon
			GameObject* canon = FetchGO();
			canon->type = GameObject::GO_PC;
			canon->pos.Set(0, Math::RandFloatMinMax(0, m_worldHeight), 0);
			canon->vel.Set(Math::RandFloatMinMax(10, 20), 0, 0);
			canon->scale.Set(5.0f, 5.0f, 1.0f);
			canon->points = 100;
			canon->active = true;
		}
		else if (powerup == 3)
		{
			//damage
			GameObject* damage = FetchGO();
			damage->type = GameObject::GO_PD;
			damage->pos.Set(0, Math::RandFloatMinMax(0, m_worldHeight), 0);
			damage->vel.Set(Math::RandFloatMinMax(10, 20), 0, 0);
			damage->scale.Set(5.0f, 5.0f, 1.0f);
			damage->points = 100;
			damage->active = true;
		}
		poweruptimer = Math::RandIntMinMax(1, 10);
		//std::cout << powerup << std::endl;
	}

	//spawning enemy at random timings 
	if (enemytimer <= 0)
	{
		enemyspawn = Math::RandIntMinMax(1, 3);
		if (enemyspawn == 1)
		{
			//std::cout << "spawn" << std::endl;
			//diagonally up
			GameObject* enemy = FetchGO();
			enemy->type = GameObject::GO_ENEMY;
			enemy->pos.Set(0, 0, 0);
			Vector3 d;
			d.Set(m_worldWidth, m_worldHeight, 0);
			//go->vel = m_ship->direction * BULLET_SPEED;
			enemy->scale.Set(5.0f, 5.0f, 1.0f);
			Vector3 dir;
			enemy->vel.Set(0);
			dir = d - enemy->pos;
			//std::cout << dir << std::endl;
			dir = dir.Normalize();
			enemy->direction.Set(dir.x, dir.y, dir.z);
			enemy->momentOfInertia = enemy->mass * enemy->scale.x * enemy->scale.x;
			enemy->angularVelocity = 0;
			enemy->force.SetZero();
			enemy->torque.SetZero();

			enemy->firerate = 1;
			enemy->points = 50;
			enemy->hp = 100;
		}
		else if (enemyspawn == 2)
		{
			//std::cout << "spawn" << std::endl;
			//centre
			GameObject* enemy = FetchGO();
			enemy->type = GameObject::GO_ENEMY;
			enemy->pos.Set(0, m_worldHeight / 2, 0);
			Vector3 d;
			d.Set(m_worldWidth, m_worldHeight / 2, 0);
			//go->vel = m_ship->direction * BULLET_SPEED;
			enemy->scale.Set(5.0f, 5.0f, 1.0f);
			Vector3 dir;
			enemy->vel.Set(0);
			dir = d - enemy->pos;
			//std::cout << dir << std::endl;
			dir = dir.Normalize();
			enemy->direction.Set(dir.x, dir.y, dir.z);
			enemy->momentOfInertia = enemy->mass * enemy->scale.x * enemy->scale.x;
			enemy->angularVelocity = 0;
			enemy->force.SetZero();
			enemy->torque.SetZero();

			enemy->firerate = 1;
			enemy->points = 50;
			enemy->hp = 100;
		}
		else if (enemyspawn == 3)
		{
			//std::cout << "spawn" << std::endl;
			//diagonally down
			GameObject* enemy = FetchGO();
			enemy->type = GameObject::GO_ENEMY;
			enemy->pos.Set(0, m_worldHeight, 0);
			Vector3 d;
			d.Set(m_worldWidth, 0, 0);
			//go->vel = m_ship->direction * BULLET_SPEED;
			enemy->scale.Set(5.0f, 5.0f, 1.0f);
			Vector3 dir;
			enemy->vel.Set(0);
			dir = d - enemy->pos;
			//std::cout << dir << std::endl;
			dir = dir.Normalize();
			enemy->direction.Set(dir.x, dir.y, dir.z);
			enemy->momentOfInertia = enemy->mass * enemy->scale.x * enemy->scale.x;
			enemy->angularVelocity = 0;
			enemy->force.SetZero();
			enemy->torque.SetZero();
			
			enemy->firerate = 1;
			enemy->points = 50;
			enemy->hp = 100;
		}
		enemytimer = Math::RandFloatMinMax(1, 5);
		//std::cout << enemyspawn << std::endl;
	}

	//Exercise 14: use a key to spawn a bullet
	if (Application::IsKeyPressed(VK_SPACE))
	{
		//Exercise 15: limit the spawn rate of bullets
		double diff = elapsedTime - prevElapsed;
		if (diff > 0.1) {
			GameObject* go = FetchGO();
			go->type = GameObject::GO_BULLET;
			go->pos = m_ship->pos;
			go->vel = m_ship->direction * BULLET_SPEED;
			go->scale.Set(0.6f, 0.7f, 1.0f);
			prevElapsed = elapsedTime;
		}
	}
	
	double x, y;
	Application::GetCursorPos(&x, &y);
	int w = Application::GetWindowWidth();
	int h = Application::GetWindowHeight();

	//Mouse Section
	static bool bLButtonState = false;
	if(!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		//std::cout << "LBUTTON DOWN" << std::endl;

		//spawn black hole
		GameObject* hole = FetchGO();
		hole->type = GameObject::GO_BLACKHOLE;
		hole->scale.Set(5, 5, 1);
		hole->mass = 1000;
		hole->pos.Set((x / w) * m_worldWidth, (h - y) / h * m_worldHeight, 0);
		hole->vel.SetZero();
	}
	else if(bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		//std::cout << "LBUTTON UP" << std::endl;

	}
	static bool bRButtonState = false;
	if(!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		//std::cout << "RBUTTON DOWN" << std::endl;
		
		//spawn white hole
		GameObject* hole = FetchGO();
		hole->type = GameObject::GO_WHITEHOLE;
		hole->scale.Set(5, 5, 1);
		hole->mass = 1000;
		hole->pos.Set((x / w)* m_worldWidth, (h - y) / h * m_worldHeight, 0);
		hole->vel.SetZero();
	}
	else if(bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		//std::cout << "RBUTTON UP" << std::endl;
	}

	//Physics Simulation Section

	//Exercise 7: Update ship's velocity based on m_force
	//F = MA
	//A = F * 1/M

	Vector3 acceleration = m_force * (1.0f / m_ship->mass);
	//Velocity
	m_ship->vel += acceleration * dt * m_speed;

	//Exercise 10: Cap Velocity magnitude (FLOAT) using MAX_SPEED (FLOAT)
	if (m_ship->vel.LengthSquared() > MAX_SPEED * MAX_SPEED)
	{
		m_ship->vel.Normalize() *= MAX_SPEED;
	}
		//Position
	m_ship->pos += m_ship->vel * dt * m_speed;

	float angularAcceleration = m_torque.z / m_ship->momentOfInertia;
	m_ship->angularVelocity += angularAcceleration * dt * m_speed;
	m_ship->angularVelocity = Math::Clamp(m_ship->angularVelocity, -MAX_ROTATION_SPEED, MAX_ROTATION_SPEED);
	m_ship->direction = RotateVector(m_ship->direction, m_ship->angularVelocity * dt * m_speed);
	m_ship->angle = Math::RadianToDegree(atan2(m_ship->direction.y, m_ship->direction.x));

	//Exercise 9: wrap ship position if it leaves screen
	//Wrap(m_ship->pos.x, m_worldWidth);
	//Wrap(m_ship->pos.y, m_worldHeight);

	Bound(m_ship->pos.x, m_worldWidth);
	Bound(m_ship->pos.y, m_worldHeight);
	static int Bcount;
	static std::vector<GameObject*> Shoot;

	Bcount = 0;
	Shoot.clear();

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			//Move the active game object
			go->pos += go->vel * dt * m_speed;
			//Exercise 12: handle collision between GO_SHIP and GO_ASTEROID using simple distance-based check
			//Move the active game object
			go->pos += go->vel * dt * m_speed;
			if (go->type == GameObject::GO_ASTEROID)
			{
				float dis = go->pos.DistanceSquared(m_ship->pos);
				float cRad = (m_ship->scale.x + go->scale.x) * (m_ship->scale.x + go->scale.x);
				if (dis < cRad)
				{
					//go->active = false;
					//m_lives -= 0.01;
				}

				//Exercise 13: asteroids should wrap around the screen like the ship
				/*Wrap(go->pos.x, m_worldWidth);
				Wrap(go->pos.y, m_worldHeight);*/
			}

			//Exercise 16: unspawn bullets when they leave screen
			else if (go->type == GameObject::GO_BULLET)
			{
				if (go->pos.x > m_worldWidth
					|| go->pos.x <0
					|| go->pos.y > m_worldHeight
					|| go->pos.y < 0)
				{
					go->active = false;
					continue;
				}
				//Exercise 18: collision check between GO_BULLET and GO_ASTEROID
				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject* go2 = (GameObject*)*it2;
					if (go2->type == GameObject::GO_ASTEROID || go2->type == GameObject::GO_ASTEROID_SMALL || go2->type == GameObject::GO_ENEMY && go2->active)
					{
						float dis = go->pos.DistanceSquared(go2->pos);
						float rad = (go->scale.x + go2->scale.x) * (go->scale.x + go2->scale.x);
						if (dis < rad)
						{
							go->active = false;
							go2->hp -= m_player_damage;
							
							if (go2->hp <= 0) // checking for selected asteroid hp
							{
								go2->active = false;
								m_score += go2->points;
							}
						}
					}
				}
			}

			//else if (go->type == GameObject::GO_BLACKHOLE || go->type == GameObject::GO_WHITEHOLE)
			//{
			//	// Code that affect the ship
			//	// Affect everything other than the ship
			//	for (int j = 0; j < m_goList.size(); j++)
			//	{
			//		if (m_goList[j]->active &&
			//			m_goList[j]->type != GameObject::GO_BLACKHOLE &&
			//			m_goList[j]->type != GameObject::GO_WHITEHOLE)
			//		{
			//			if (m_goList[j]->pos.DistanceSquared(go->pos) < 3600.0f)
			//			{
			//				//1 Case Destroy the Object
			//				if (m_goList[j]->pos.DistanceSquared(go->pos) < 4.0f)
			//				{
			//					go->mass += m_goList[j]->mass;
			//					m_goList[j]->active = false;
			//				}
			//				//2 Not close enough... affect the object through force
			//				else
			//				{
			//					float sign = (go->type == GameObject::GO_WHITEHOLE) ? -1 : 1;
			//					Vector3	dir = sign * (go->pos - m_goList[j]->pos).Normalized();
			//					float force = CalculateAdditionalForce(m_goList[j], go);
			//					m_goList[j]->vel += 1.f / m_goList[j]->mass * dir * force * dt * m_speed;
			//				}
			//			}
			//		}
			//	}
			//}
			else if (go->type == GameObject::GO_PH || go->type == GameObject::GO_PC ||go->type == GameObject::GO_PD)
			{
				if (go->pos.x > m_worldWidth
					|| go->pos.x <0
					|| go->pos.y > m_worldHeight
					|| go->pos.y < 0)
				{
					go->active = false;
					continue;
				}
				float dis = go->pos.DistanceSquared(m_ship->pos);
				float rad = (go->scale.x + m_ship->scale.x) * (go->scale.x + m_ship->scale.x);
				if (dis < rad)
				{
					go->active = false;
					powerUpActivated;
					m_score += go->points;
					if (go->type == GameObject::GO_PH) // heal
					{
						m_lives += 50;
						if (m_lives >= 100)
						{
							m_lives = 100;
						}
						powerUpHealActivated = true;
					}
					else if (go->type == GameObject::GO_PC) // canon
					{
						// add canon feature 
						powerUpCanonActivated = true;
						GameObject* go = FetchGO();
						go->type = GameObject::GO_CANON;
						go->pos.Set(m_worldWidth / 2, m_ship->pos.y, 0);
						go->vel.Set(0);
						go->scale.Set(m_worldWidth / 2, 10.0f, 1.0f);
						go->timer = 5;
					}
					else if (go->type == GameObject::GO_PD)
					{
						m_player_damage += 20;
						powerUpDamageActivated = true;
					}
				}
			}
			else if (go->type == GameObject::GO_CANON)
			{
				go->timer -= dt;
				if (go->timer <= 0)
				{
					go->active = false;
				}
				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject* go2 = (GameObject*)*it2;
					if (go2->type == GameObject::GO_ASTEROID || go2->type == GameObject::GO_ASTEROID_SMALL || go2->type == GameObject::GO_ENEMY && go2->active)
					{
						float dis = go->pos.DistanceSquared(go2->pos);
						float rad = (go->scale.x + go2->scale.x) * (go->scale.x + go2->scale.x);

						if (go->pos.x < go2->pos.x + go2->scale.x * 2 && 
							go->pos.x + go->scale.x * 2 > go2->pos.x &&
							go->pos.y < go2->pos.y + go2->scale.y * 2 &&
							go->pos.y + go->scale.y * 2 > go2->pos.y)
						{
							go2->hp -= 100;

							if (go2->hp <= 0) // checking for selected asteroid hp
							{
								go2->active = false;
								m_score += go2->points;
							}
						}
					}
				}
			}
			else if (go->type == GameObject::GO_ENEMYBULLET)
			{
				if (go->pos.x > m_worldWidth
					|| go->pos.x <0
					|| go->pos.y > m_worldHeight
					|| go->pos.y < 0)
				{
					go->active = false;
					continue;
				}
				//Exercise 18: collision check between GO_ENEMYBULLET and GO_SHIP
				
				float dis = go->pos.DistanceSquared(m_ship->pos);
				float rad = (go->scale.x + m_ship->scale.x) * (go->scale.x + m_ship->scale.x);
				if (dis < rad)
				{
					go->active = false;
					m_lives -= m_enemy_damage;
					//std::cout << m_ship->hp << std::endl;
					if (m_lives <= 0) // checking for player hp
					{
						m_ship->active = false;
					}
				}
			}
			else if (go->type == GameObject::GO_ENEMY)
			{
				if (go->pos.x > m_worldWidth
					|| go->pos.x <0
					|| go->pos.y > m_worldHeight
					|| go->pos.y < 0)
				{
					go->active = false;
					continue;
				}
				go->torque.SetZero();
				go->force.SetZero();
				go->force += go->direction * 100.0f;
				Vector3 acceleration = go->force * (1.0f / go->mass);
				//Velocity
				go->vel += acceleration * dt * m_speed;

				//Exercise 10: Cap Velocity magnitude (FLOAT) using MAX_SPEED (FLOAT)
				if (go->vel.LengthSquared() > MAX_SPEED * MAX_SPEED)
				{
					go->vel.Normalize() *= MAX_SPEED;
				}
				//Position
				go->pos += go->vel * dt * m_speed;

				float angularAcceleration = go->torque.z / go->momentOfInertia;
				go->angularVelocity += angularAcceleration * dt * m_speed;
				go->angularVelocity = Math::Clamp(go->angularVelocity, -MAX_ROTATION_SPEED, MAX_ROTATION_SPEED);
				go->direction = RotateVector(go->direction, go->angularVelocity * dt * m_speed);
				go->angle = Math::RadianToDegree(atan2(go->direction.y, go->direction.x));
				go->firerate -= dt;
				if (go->firerate <= 0)
				{
					Bcount++;
					Shoot.push_back(go);
					go->firerate = 1;
				}
			}
		}
	}

	//loop to cout bullet spawn
	for (int i = 0; i < Bcount; i++)
	{
		GameObject* go2 = FetchGO();
		go2->type = GameObject::GO_ENEMYBULLET;
		go2->pos = Shoot[i]->pos;
		go2->vel = Shoot[i]->direction * BULLET_SPEED;
		go2->scale.Set(0.6f, 0.7f, 1.0f);
	}
}


void SceneAsteroid::RenderGO(GameObject *go)
{
	//Exe 4
	switch (go->type)
	{
	case GameObject::GO_SHIP:
		//Exercise 4a: render a sphere with radius 1
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_SHIP], true);
		modelStack.PopMatrix();
		break;
		//Exercise 17a: render a ship texture or 3D ship model
		//Exercise 17b:	re-orientate the ship with velocity
		//Exercise 4b: render a cube with length 2
		break;
	case GameObject::GO_BULLET:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BULLET], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_BLACKHOLE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BLACKHOLE], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_WHITEHOLE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_WHITEHOLE], false);
		modelStack.PopMatrix();
		break;

	//enemies
	case GameObject::GO_ENEMY:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ENEMY], true);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMYBULLET:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ENEMYBULLET], false);
		modelStack.PopMatrix();
		break;

	//powerups
	case GameObject::GO_PH:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_POWERUPHEAL], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_PC:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_POWERUPCANON], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_PD:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_POWERUPDAMAGE], false);
		modelStack.PopMatrix();
		break;

	//asteroids
	case GameObject::GO_ASTEROID_SMALL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ASTEROID_SMALL], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ASTEROID:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ASTEROID], false);
		modelStack.PopMatrix();
	case GameObject::GO_CANON:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CANON], false);
		modelStack.PopMatrix();
	}
}

float SceneAsteroid::CalculateAdditionalForce(GameObject* go1, GameObject* go2)
{
	float radiusSquared = go1->pos.DistanceSquared(go2->pos);
	return (GRAVITY_CONSTANT * go1->mass * go2->mass) / radiusSquared;
}

//void SceneAsteroid::Wrap(float& val, float bound)
//{
//	if (val < 0)
//	{
//		val += bound;
//	}
//	else if (val > bound)
//	{
//		val -= bound;
//	}
//}

void SceneAsteroid::Bound(float& val, float bound)
{
	if (val < 0)
	{
		val = 0;
	}
	else if (val > bound)
	{
		val = bound;
	}
}

void SceneAsteroid::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.LoadMatrix(projection);
	
	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
						camera.position.x, camera.position.y, camera.position.z,
						camera.target.x, camera.target.y, camera.target.z,
						camera.up.x, camera.up.y, camera.up.z
					);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();
	
	RenderMesh(meshList[GEO_AXES], false);

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -1);
	modelStack.Scale(300, 200, 1);
	RenderMesh(meshList[GEO_BACKGROUND], false);
	modelStack.PopMatrix();

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			RenderGO(go);
			if (go->type == GameObject::GO_TEST)
			{
				std::cout << "hi" << std::endl;
			}
		}
	}

	//On screen text

//Exe 5
	std::ostringstream ss;
	//Exercise 5a: Render m_lives, m_score
	ss.str("");
	ss << "Health: " << m_lives << "/100HP";
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 57);

	//Render score
	ss.str("");
	ss << std::setfill('0') << std::setw(5) << m_score;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 65, 57);

	//Render powerup
	if (powerUpHealActivated == true)
	{
		ss.str("");
		ss << "Heal Activated";
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 54);
	}
	else if (powerUpCanonActivated == true)
	{
		ss.str("");
		ss << "Canon Activated";
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 54);
	}
	else if (powerUpDamageActivated == true)
	{
		ss.str("");
		ss << "2x Damage Activated";
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 54);
	}

	/*ss.str("");
	ss << "Pos: " << m_ship->pos;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 12);*/

	/*ss.str("");
	ss << "Vel: " << m_ship->vel;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 15);*/

	/*ss.str("");
	ss << "Mass: " << m_ship->mass;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 18);*/

	//Remember to add to Render
	//below RenderMesh(meshList[GEO_AXES], false);
	RenderGO(m_ship);


	//ss.precision(3);
	//ss << "Speed: " << m_speed;
	//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 6);
	
	ss.str("");
	ss.precision(5);
	ss << "Timer: " << survive << "/100 seconds";
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 3);

	ss.str("");
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 1);
}

void SceneAsteroid::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while(m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if(m_ship)
	{
		delete m_ship;
		m_ship = NULL;
	}
}

static Vector3 RotateVector(const Vector3& vec, float radian)
{
	return Vector3(vec.x * cos(radian) + vec.y * -sin(radian),
					vec.x * sin(radian) + vec.y * cos(radian), 0.f);
}
