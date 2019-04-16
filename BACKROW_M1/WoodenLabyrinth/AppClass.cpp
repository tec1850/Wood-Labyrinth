#include "AppClass.h"
using namespace Simplex;
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

	//adds squares on bottom
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			m_pEntityMngr->AddEntity("cubeMesh.fbx");
			vector3 v3Position = vector3(-7.5 + i, 10, -7.5 + j);
			matrix4 m4Pos = glm::translate(v3Position);
			m_pEntityMngr->SetModelMatrix(m4Pos);
		}
	}

	//addes sphere
	m_pEntityMngr->AddEntity("sphere.fbx");
	vector3 v3Position = vector3(-.5, 12, -.5);
	matrix4 m4Pos = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Pos);

	m_pEntityMngr->Update();
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

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);

	m_pMeshMngr->AddCubeToRenderList(IDENTITY_M4 * glm::scale(vector3(10.f)) , C_BROWN, RENDER_SOLID);
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 30.0f, 0.0f), //Position
		vector3(0.0f, 0.0f, 0.5f),	//Target
		AXIS_Y);					//Up

	//might have to add another ^^ for the sphere when added in


	//gravity goes here?
	//could use ApplyForce in MyEntity
	//->ApplyForce(vector3(0.0f,-3.0f,0.0f));

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