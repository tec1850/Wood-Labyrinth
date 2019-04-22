#include "AppClass.h"
#include <iostream>
#include <fstream>
using namespace Simplex; //10.0.17763.0 - lab sdk version




void Application::InitVariables(void)
{
	m_sProgrammer = "TEAM BACKROW";

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 30.0f, 0.0f), //Position
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
	char* data = new char[1000];

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
			m_pEntityMngr->AddEntity("cubeMesh.fbx", "base_" + i + j);
			vector3 v3Position = vector3(-7.5 + i, 10, -7.5 + j);
			matrix4 m4Pos = glm::translate(v3Position);
			m_pEntityMngr->SetModelMatrix(m4Pos);
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
				vector3 v3Position = vector3(-7.5 + i, 11, -7.5 + j);
				matrix4 m4Pos = glm::translate(v3Position);
				m_pEntityMngr->SetModelMatrix(m4Pos);
			}
		}
	}

	//adds sphere
	m_pBall = new MyEntity("sphere.fbx");
	
	//m_pBall->SetMass(1.0f);
	m_pBall->UsePhysicsSolver(true);
	vector3 v3Position = vector3(-.5, 25, -.5);
	matrix4 m4Pos = glm::translate(v3Position);
	m_pBall->SetModelMatrix(m4Pos * glm::scale(vector3(0.2f)));

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

	for (uint i = 0; i < numCubes; i++)
	{
		MyEntity* temp = m_pEntityMngr->GetEntity(i);
		if (m_pBall->IsColliding(temp))
		{
			m_pBall->GetSolver()->ApplyForce(vector3(0.0f, -0.12f, 0.0f));
		}
	}

	//std::cout << (m_pBall->GetPosition().y);

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
	m_pBall->AddToRenderList(true);

	//m_pBall.
	  
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
	ShutdownGUI();
}

void Application::ProcessInput(MyEntity* ball) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		ball->PushBall(vector3(0.0f, 0.0f, 0.01f));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		ball->PushBall(vector3(0.0f, 0.0f, -0.01f));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		ball->PushBall(vector3(0.01f, 0.0f, 0.0f));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		ball->PushBall(vector3(-0.01f, 0.0f, 0.0f));
	}
}