#include "AppClass.h"
#include <iostream>
#include <fstream>
using namespace Simplex; //10.0.17763.0 - lab sdk version
						 //10.0.16299.0 - sdk version it was on when i opened it today for some reason




void Application::InitVariables(void)
{
	m_sProgrammer = "TEAM BACKROW";

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 100.0f, 0.0f), //Position
		vector3(0.0f, 0.0f, 0.5f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

															 
	m_pEntityMngr = MyEntityManager::GetInstance(); //Initialize the entity manager
	m_pMeshMngr = MeshManager::GetInstance();
	//m_pEntityMngr->UsePhysicsSolver();
	//m_pEntityMngr->AddEntity("cubeMesh.fbx", "Cube");
	//m_pEntityMngr->SetModelMatrix(IDENTITY_M4 * glm::translate(vector3(0.f, 10.f, 0.f)));

	//grab the .dat folder
	std::ifstream m_fTheFile;
	m_fTheFile.open("labData.txt", std::ios_base::in | std::ios_base::binary);
	std::string line;
	data = new char[100000];

	if (m_fTheFile.is_open())
	{
		while (std::getline(m_fTheFile, line))
		{
			for (uint i = 0; i < line.length(); i++)
			{
				std::cout << "Current index value: " << line[i] << std::endl;
				data[i] = line[i];
			}
		}
	}
	m_fTheFile.close();

	//base floor thing
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			m_pEntityMngr->AddEntity("baseCube.fbx", "base_" + i + j);
			vector3 v3Position = vector3(-37.5 + (i * 5), 10, -37.5 + (j * 5));
			matrix4 m4Pos = glm::translate(v3Position);
			m_pEntityMngr->SetModelMatrix(m4Pos * glm::scale(vector3(5.f)));
		}
	}

	//adds squares on top
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			uint num = (15 * i) + j;
			std::cout << data[num] << std::endl;
			if (data[num] == '0')
			{
				m_pEntityMngr->AddEntity("cubeMesh.fbx");
				vector3 v3Position = vector3(-37.5 + (i * 5), 15, -37.5 + (j * 5));
				matrix4 m4Pos = glm::translate(v3Position);
				m_pEntityMngr->SetModelMatrix(m4Pos * glm::scale(vector3(5.f)));
			}

			if (data[num] == '2')
			{
				m_pBall = new MyEntity("sphere.fbx");
				m_pBall->UsePhysicsSolver(true);
				vector3 v3Position = vector3(-37.5 + (i * 5), 15, -37.5 + (j * 5));
				matrix4 m4Pos = glm::translate(v3Position);
				m_pBall->SetModelMatrix(m4Pos * glm::scale(vector3(0.4f)));
			}
			if (data[num] == '3')
			{
				winCondition = vector3(-37.5 + (i * 5), 15, -37.5 + (j * 5));
			}
		}
	}
	
	std::cout << winCondition.x << ", " << winCondition.y << ", " << winCondition.z << std::endl;
	m_uOctantLevels = 1;
	m_pRoot = new MyOctant(m_uOctantLevels, 5);

	m_pEntityMngr->Update();
	m_pBall->Update();
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();
	
	//check for input
	ProcessInput(m_pBall);

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();
	m_pBall->Update();

	uint numCubes = m_pEntityMngr->GetEntityCount();

	//if the ball is within the bounds of the win condition
	if (m_pBall->GetPosition().x > winCondition.x - 2.5 && m_pBall->GetPosition().x < winCondition.x + 2.5 &&
		m_pBall->GetPosition().z > winCondition.z - 2.5 && m_pBall->GetPosition().z < winCondition.z + 2.5 &&
		m_pBall->GetPosition().y > 12.5 && m_pBall->GetPosition().y < 17.5)
	{
		std::cout << "YOU WIN" << std::endl;
	}

	for (uint i = 0; i < numCubes; i++)
	{
		MyEntity* temp = m_pEntityMngr->GetEntity(i);
		bool isColliding = true;

		if (m_pBall->GetRigidBody()->GetMaxGlobal().x < temp->GetRigidBody()->GetMinGlobal().x) //this to the right of other
			isColliding = false;
		if (m_pBall->GetRigidBody()->GetMinGlobal().x > temp->GetRigidBody()->GetMaxGlobal().x) //this to the left of other
			isColliding = false;

		if (m_pBall->GetRigidBody()->GetMaxGlobal().y < temp->GetRigidBody()->GetMinGlobal().y) //this below of other
			isColliding = false;
		if (m_pBall->GetRigidBody()->GetMinGlobal().y > temp->GetRigidBody()->GetMaxGlobal().y) //this above of other
			isColliding = false;

		if (m_pBall->GetRigidBody()->GetMaxGlobal().z < temp->GetRigidBody()->GetMinGlobal().z) //this behind of other
			isColliding = false;
		if (m_pBall->GetRigidBody()->GetMinGlobal().z > temp->GetRigidBody()->GetMaxGlobal().z) //this in front of other
			isColliding = false;

		//if (m_pBall->GetPosition().x + 0.2f < temp->GetRigidBody()->GetMaxGlobal().x) //this to the right of other
		//	isColliding = false;
		//if (m_pBall->GetPosition().x - 0.2f> temp->GetRigidBody()->GetMaxGlobal().x) //this to the left of other
		//	isColliding = false;

		//if (m_pBall->GetPosition().y + 0.2f < temp->GetRigidBody()->GetMinGlobal().y) //this below of other
		//	isColliding = false;
		//if (m_pBall->GetPosition().y - 0.2f > temp->GetRigidBody()->GetMaxGlobal().y) //this above of other
		//	isColliding = false;

		//if (m_pBall->GetPosition().z + 0.2f < temp->GetRigidBody()->GetMinGlobal().z) //this behind of other
		//	isColliding = false;
		//if (m_pBall->GetPosition().z - 0.2f > temp->GetRigidBody()->GetMaxGlobal().z) //this in front of other
		//	isColliding = false;

		if (isColliding)
		{
			//m_pBall->UsePhysicsSolver(false);
			if (m_pBall->GetRigidBody()->GetMaxGlobal().y > temp->GetRigidBody()->GetMaxGlobal().y)
			{
				m_pBall->GetSolver()->ApplyForce(vector3(0.0f, -(m_pBall->GetSolver()->GetVelocity().y + -0.001f), 0.0f));
			}
			else 
			{
				std::cout << "collision" << std::endl;
				if (m_pBall->GetRigidBody()->GetMinGlobal().x < temp->GetRigidBody()->GetMinGlobal().x && m_pBall->GetSolver()->GetVelocity().x > 0.0f)
				{
					m_pBall->GetSolver()->ApplyForce(vector3(-(m_pBall->GetSolver()->GetVelocity().x + 0.0001f), 0.0f, 0.0f));
				}
				if (m_pBall->GetRigidBody()->GetMaxGlobal().x > temp->GetRigidBody()->GetMaxGlobal().x && m_pBall->GetSolver()->GetVelocity().x < 0.0f)
				{
					m_pBall->GetSolver()->ApplyForce(vector3(-(m_pBall->GetSolver()->GetVelocity().x + -0.0001f), 0.0f, 0.0f));
				}
				if (m_pBall->GetRigidBody()->GetMinGlobal().z < temp->GetRigidBody()->GetMinGlobal().z && m_pBall->GetSolver()->GetVelocity().z > 0.0f)
				{
					m_pBall->GetSolver()->ApplyForce(vector3(0.0f, 0.0f, -(m_pBall->GetSolver()->GetVelocity().z + 0.0001f)));
				}
				if (m_pBall->GetRigidBody()->GetMaxGlobal().z > temp->GetRigidBody()->GetMaxGlobal().z && m_pBall->GetSolver()->GetVelocity().z < 0.0f)
				{
					m_pBall->GetSolver()->ApplyForce(vector3(0.0f, 0.0f, -(m_pBall->GetSolver()->GetVelocity().z + -0.0001f)));
				}
			}
		}
	}

	//m_pBall->UsePhysicsSolver(false);

	//std::cout << (m_pBall->GetPosition().y);

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
	m_pBall->AddToRenderList(true);
	  
	//m_pCameraMngr->SetPositionTargetAndUpward(
	//	vector3(0.0f, 30.0f, 0.0f), //Position
	//	vector3(0.0f, 0.0f, 0.5f),	//Target
	//	AXIS_Y);					//Up

}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//draw octree
	m_pRoot->Display();
		
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();
	
	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}

void Application::Release(void)
{
	//release GUI
	delete[] data;
	ShutdownGUI();
}

void Application::ProcessInput(MyEntity* ball) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		ball->PushBall(vector3(0.0f, 0.0f, 0.00005f));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		ball->PushBall(vector3(0.0f, 0.0f, -0.00005f));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		ball->PushBall(vector3(0.00005f, 0.0f, 0.0f));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		ball->PushBall(vector3(-0.00005f, 0.0f, 0.0f));
	}
}