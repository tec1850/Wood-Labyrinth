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
	char data[1000];

	uint index = 0;
	uint fullFileLength = 0;

	if (m_fTheFile.is_open())
	{
		while (std::getline(m_fTheFile, line))
		{
			for (uint i = 0; i < line.length(); i++)
			{
				if (isspace(line[i]))
				{

				}
				else
				{
					data[i * index] = line[i];
					fullFileLength += 1;
				}
			}
			index++;
		}
	}
	m_fTheFile.close();

	//adds squares on bottom
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			std::cout << data[((15 * i) + j)] << std::endl;
			if (data[((15 * i) + j)] == '0')
			{
				m_pEntityMngr->AddEntity("cubeMesh.fbx");
				vector3 v3Position = vector3(-7.5 + i, 10, -7.5 + j);
				matrix4 m4Pos = glm::translate(v3Position);
				m_pEntityMngr->SetModelMatrix(m4Pos);
			}
		}
	}

	//addes sphere
	m_pBall = new MyEntity("sphere.fbx");
	
	//m_pBall->SetMass(1.0f);
	m_pBall->UsePhysicsSolver(true);
	vector3 v3Position = vector3(-.5, 25, -.5);
	matrix4 m4Pos = glm::translate(v3Position);
	m_pBall->SetModelMatrix(m4Pos);

	m_pEntityMngr->Update();
	m_pBall->Update();
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	//ArcBall();

	//Is the first person camera active?
	//CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();
	m_pBall->Update();

	//std::cout << (m_pBall->GetPosition().y);

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
	m_pBall->AddToRenderList(true);
	
	
	//m_pMeshMngr->AddCubeToRenderList(glm::rotate(IDENTITY_M4, static_cast<float>(PI / 2.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(vector3(10.f)) , C_BROWN, RENDER_SOLID);
	m_pMeshMngr->AddCubeToRenderList(IDENTITY_M4 * glm::scale(vector3(10.f)), C_BROWN, RENDER_SOLID); //is this necessary? commented it out and nothing changed
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 30.0f, 0.0f), //Position
		vector3(0.0f, 0.0f, 0.5f),	//Target
		AXIS_Y);					//Up

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