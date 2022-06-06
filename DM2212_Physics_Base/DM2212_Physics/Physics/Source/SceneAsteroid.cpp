#include "SceneAsteroid.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

const float SceneAsteroid::ROTATION_SPEED = 1.0f;
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
	m_lives = 3;
	m_score = 0;

	//Exercise 2c: Construct m_ship, set active, type, scale and pos
	m_ship = new GameObject(GameObject::GO_SHIP);
	m_ship->active = true;
	m_ship->scale.Set(1, 1, 1);
	m_ship->pos.Set(m_worldWidth / 2, m_worldHeight / 2);
	m_ship->vel.Set(0, 0, 0);
	m_ship->direction.Set(0, 1, 0); // direction of ship thats facing

	m_ship->momentOfInertia = m_ship->mass * m_ship->scale.x * m_ship->scale.x;
	m_ship->angularVelocity = 0;

	m_torque.SetZero();

	prevElapsed = elapsedTime = 0.0;
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
	}
	if (Application::IsKeyPressed('S'))
	{
		m_force -= m_ship->direction * 100.0f;
	}
	if (Application::IsKeyPressed('D'))
	{
		m_force += m_ship->direction * ROTATION_SPEED;
		m_torque += Vector3(-m_ship->scale.x, m_ship->scale.y, 0).Cross(Vector3(ROTATION_SPEED, 0, 0));
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
	//Exercise 11: use a key to spawn some asteroids
	if (Application::IsKeyPressed('V'))
	{
		//Spawn 25 asteroids
		for (int i = 0; i < 25; ++i)
		{
			//Create Asteroid
			GameObject* go = FetchGO();
			go->type = GameObject::GO_ASTEROID;
			go->pos.Set(Math::RandFloatMinMax(0, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight), 0);
			go->vel.Set(Math::RandFloatMinMax(-20, 20), Math::RandFloatMinMax(-20, 20), 0);
			go->scale.Set(1, 1, 1);
		}
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
			go->scale.Set(0.2f, 0.2f, 1.0f);
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
		std::cout << "LBUTTON DOWN" << std::endl;

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
		std::cout << "LBUTTON UP" << std::endl;

	}
	static bool bRButtonState = false;
	if(!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;
		
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
		std::cout << "RBUTTON UP" << std::endl;
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
	Wrap(m_ship->pos.x, m_worldWidth);
	Wrap(m_ship->pos.y, m_worldHeight);

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
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
					go->active = false;
					m_lives -= 1;
				}

				//Exercise 13: asteroids should wrap around the screen like the ship
				Wrap(go->pos.x, m_worldWidth);
				Wrap(go->pos.y, m_worldHeight);
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
					if (go2->type == GameObject::GO_ASTEROID && go2->active)
					{
						float dis = go->pos.DistanceSquared(go2->pos);
						float rad = (go->scale.x + go2->scale.x) * (go->scale.x + go2->scale.x);
						if (dis < rad)
						{
							go->active = false;
							go2->active = false;
							m_score += 2;
						}
					}
				}
			}

			else if (go->type == GameObject::GO_BLACKHOLE 
				|| go->type == GameObject::GO_WHITEHOLE)
			{
				// Code that affect the ship
				// Affect everything other than the ship
				for (int j = 0; j < m_goList.size(); j++)
				{
					if (m_goList[j]->active && 
						m_goList[j]->type != GameObject::GO_BLACKHOLE &&
						m_goList[j]->type != GameObject::GO_WHITEHOLE)
					{
						if (m_goList[j]->pos.DistanceSquared(go->pos) < 3600.0f)
						{
							//1 Case Destroy the Object
							if (m_goList[j]->pos.DistanceSquared(go->pos) < 4.0f)
							{
								go->mass += m_goList[j]->mass;
								m_goList[j]->active = false;
							}
							//2 Not close enough... affect the object through force
							else
							{
								float sign = (go->type == GameObject::GO_WHITEHOLE) ? -1 : 1;
								Vector3	dir = sign * (go->pos - m_goList[j]->pos).Normalized();
								float force = CalculateAdditionalForce(m_goList[j], go);
								m_goList[j]->vel += 1.f / m_goList[j]->mass * dir * force * dt * m_speed;
							}
						}
					}
				}
			}
		}
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
	case GameObject::GO_ASTEROID:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ASTEROID], false);
		modelStack.PopMatrix();
		//Exercise 4b: render a cube with length 2
		break;
	case GameObject::GO_BULLET:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
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
	}
}

float SceneAsteroid::CalculateAdditionalForce(GameObject* go1, GameObject* go2)
{
	float radiusSquared = go1->pos.DistanceSquared(go2->pos);
	return (GRAVITY_CONSTANT * go1->mass * go2->mass) / radiusSquared;
}

void SceneAsteroid::Wrap(float& val, float bound)
{
	if (val < 0)
	{
		val += bound;
	}
	else if (val > bound)
	{
		val -= bound;
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

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			RenderGO(go);
		}
	}

	//On screen text

//Exe 5
	std::ostringstream ss;
	//Exercise 5a: Render m_lives, m_score
	ss.str("");
	ss << "lives: " << m_lives << " Score:" << m_score;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 9);

	ss.str("");
	ss << "Pos: " << m_ship->pos;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 12);

	ss.str("");
	ss << "Vel: " << m_ship->vel;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 15);

	ss.str("");
	ss << "Mass: " << m_ship->mass;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 18);

	//Remember to add to Render
	//below RenderMesh(meshList[GEO_AXES], false);
	RenderGO(m_ship);


	ss.precision(3);
	ss << "Speed: " << m_speed;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 6);
	
	ss.str("");
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 0, 3);
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
