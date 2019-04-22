#include "MyOctant.h"
using namespace Simplex;
// initialize static variables
uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 3;
uint MyOctant::m_uIdealEntityCount = 5;
uint MyOctant::GetOctantCount(void) {
	return m_uOctantCount;
}
void MyOctant::Init(void) {
	m_uChildren = 0;
	m_fSize = 0;
	m_uID = m_uOctantCount;
	m_uLevel = 0;

	m_v3Center = vector3(0);
	m_v3Min = vector3(0);
	m_v3Max = vector3(0);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_pRoot = nullptr;
	m_pParent = nullptr;

	for (uint i = 0; i < 8; i++) {
		m_pChild[i] = nullptr;
	}
}
void MyOctant::Swap(MyOctant& other) {
	//swaps variables
	std::swap(m_uChildren, other.m_uChildren);
	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_pParent, other.m_pParent);

	//swaps children
	for(uint i = 0; i < 8; i++){
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}
MyOctant* MyOctant::GetParent(void) {
	return m_pParent;
}
void MyOctant::Release(void) {
	//should only happen for the root
	if (m_uLevel == 0) {
		KillBranches();
	}
	m_uChildren = 0;
	m_fSize = 0;
	m_EntityList.clear();
	m_lChild.clear();
}
MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount){
	Init();

	//vars
	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;

	//sets root
	m_pRoot = this;
	m_lChild.clear();

	std::vector<vector3> lMinMax;
	uint nObjects = m_pEntityMngr->GetEntityCount();

	//inits lMinMax
	for(uint i = 0; i < nObjects; ++i){
		MyEntity* pEntity = m_pEntityMngr->GetEntity(i);
		MyRigidBody* pRigidBody = pEntity->GetRigidBody();
		lMinMax.push_back(pRigidBody->GetMinGlobal());
		lMinMax.push_back(pRigidBody->GetMaxGlobal());
	}

	//makes rigidbody out of lMinMax
	MyRigidBody* pRigidBody = new MyRigidBody(lMinMax);
	vector3 vHalfWidth = pRigidBody->GetHalfWidth();

	float fmax = vHalfWidth.x;
	for(size_t i = 0; i < 3; i++){
		if(fmax < vHalfWidth[i]){
			fmax = vHalfWidth[i];
		}
	}

	lMinMax.clear();

	m_fSize = fmax * 2;
	m_v3Center = pRigidBody->GetCenterLocal();
	m_v3Min = m_v3Center - vector3(fmax);
	m_v3Max = m_v3Center + vector3(fmax);

	m_uOctantCount++;

	SafeDelete(pRigidBody);

	ConstructTree(m_uMaxLevel);
}
MyOctant::MyOctant(vector3 a_v3Center, float a_fSize){
	Init();

	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);
	m_uOctantCount++;
}
MyOctant::MyOctant(MyOctant const& other){
	m_uChildren = other.m_uChildren;
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_fSize = other.m_fSize;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_pParent = other.m_pParent;

	m_pRoot = other.m_pRoot;
	m_lChild = other.m_lChild;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for(size_t i = 0; i < 8; i++){
		m_pChild[i] = other.m_pChild[i];
	}
}
MyOctant& MyOctant::operator=(MyOctant const& other){
	if(this != &other){
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}
MyOctant::~MyOctant(){
	Release();
}
float MyOctant::GetSize(void){
	return m_fSize;
}
vector3 MyOctant::GetCenterGlobal(void){
	return m_v3Center;
}
vector3 MyOctant::GetMinGlobal(void){
	return m_v3Min;
}
vector3 MyOctant::GetMaxGlobal(void){
	return m_v3Max;
}
void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color){
	//displays wire cube
	if (m_uID == a_nIndex) {
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
	}
	else{
		for (uint i = 0; i < m_uChildren; i++) {
			m_pChild[i]->Display(a_nIndex);
		}
	}
}

void MyOctant::Display(vector3 a_v3Color){
	//same as other display but recursive
	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->Display(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color,RENDER_WIRE);
}
void MyOctant::Subdivide() {
	//if node's reached max depth, return w/ no changes
	if (m_uLevel >= m_uMaxLevel) {
		return;
	}
	//already been subdivided
	if (m_uChildren != 0) {
		return;
	}

	m_uChildren = 8;

	float fHalfSize = m_fSize / 2;
	float fQuartSize = m_fSize / 4;

	//one for each child
	m_pChild[0] = new MyOctant(m_v3Center + vector3(fQuartSize, fQuartSize, fQuartSize), fHalfSize);
	m_pChild[1] = new MyOctant(m_v3Center + vector3(-fQuartSize, fQuartSize, fQuartSize), fHalfSize);
	m_pChild[2] = new MyOctant(m_v3Center + vector3(-fQuartSize, -fQuartSize, fQuartSize), fHalfSize);
	m_pChild[3] = new MyOctant(m_v3Center + vector3(fQuartSize, -fQuartSize, fQuartSize), fHalfSize);

	m_pChild[4] = new MyOctant(m_v3Center + vector3(fQuartSize, fQuartSize, -fQuartSize), fHalfSize);
	m_pChild[5] = new MyOctant(m_v3Center + vector3(-fQuartSize, fQuartSize, -fQuartSize), fHalfSize);
	m_pChild[6] = new MyOctant(m_v3Center + vector3(-fQuartSize, -fQuartSize, -fQuartSize), fHalfSize);
	m_pChild[7] = new MyOctant(m_v3Center + vector3(fQuartSize, -fQuartSize, -fQuartSize), fHalfSize);

	/*//the other way that didnt work as well
	float fsize = m_fSize / 4.0f;
	float fsizeD = fsize * 2.0f;
	vector3 v3Center;

	v3Center = m_v3Center;
	v3Center.x -= fsize;
	v3Center.y -= fsize;
	v3Center.z -= fsize;
	m_pChild[0] = new MyOctant(v3Center, fsizeD);

	v3Center.x += fsize;
	m_pChild[1] = new MyOctant(v3Center, fsizeD);

	v3Center.z += fsize;
	m_pChild[2] = new MyOctant(v3Center, fsizeD);

	v3Center.x -= fsize;
	m_pChild[3] = new MyOctant(v3Center, fsizeD);

	v3Center.y += fsize;
	m_pChild[4] = new MyOctant(v3Center, fsizeD);

	v3Center.z -= fsize;
	m_pChild[5] = new MyOctant(v3Center, fsizeD);

	v3Center.x += fsize;
	m_pChild[6] = new MyOctant(v3Center, fsizeD);

	v3Center.z += fsize;
	m_pChild[7] = new MyOctant(v3Center, fsizeD);*/

	//children
	for(uint i = 0; i < m_uChildren; i++) {
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;

		//can it be subdivided
		if(m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount)){
			m_pChild[i]->Subdivide();
		}
	}
}
MyOctant* MyOctant::GetChild(uint a_nChild) {
	return m_pChild[a_nChild];
}
bool Simplex::MyOctant::IsColliding(uint a_uRBIndex){
	// get rigidbody of entity with RBIndex
	MyEntity* entity = m_pEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* rigidBody = entity->GetRigidBody();

	// get min/max variables
	vector3 minGlobal = rigidBody->GetMinGlobal();
	vector3 maxGlobal = rigidBody->GetMaxGlobal();

	//x, y, z
	if(m_v3Min.x > maxGlobal.x){
		return false;
	}
	if(m_v3Max.x < minGlobal.x){
		return false;
	}
	if(m_v3Min.y > maxGlobal.y){
		return false;
	}
	if(m_v3Max.y < minGlobal.y){
		return false;
	}
	if(m_v3Min.z > maxGlobal.z){
		return false;
	}
	if(m_v3Max.z < minGlobal.z){
		return false;
	}
	return true;
}
bool MyOctant::IsLeaf(void) {
	//is it a leaf
	if (m_uChildren == 0) {
		return true;
	}
	else {
		return false;
	}
}
void MyOctant::DisplayLeafs(vector3 a_v3Color){
	// same as display
	for(size_t i = 0; i < m_lChild.size(); i++){
		m_lChild[i]->DisplayLeafs(a_v3Color);
	}

	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color);
}
void MyOctant::ClearEntityList(void){
	m_EntityList.clear();
}
void MyOctant::KillBranches(void){
	//clear out children
	for(size_t i = 0; i < m_uChildren; i++){
		m_pChild[i]->KillBranches();
		SafeDelete(m_pChild[i]);
	}
	// reset children back to 0
	m_uChildren = 0;
}
bool MyOctant::ContainsMoreThan(uint a_nEntities) {
	uint count = 0;

	//incs count on collisions
	for (size_t i = 0; i < m_pEntityMngr->GetEntityCount(); i++) {
		if (IsColliding(i) == true) {
			count++;
		}
		if (count > a_nEntities) {
			return true;
		}
	}
	return false;
}
void MyOctant::ConstructTree(uint a_nMaxLevel){
	//inits variables
	m_uMaxLevel = a_nMaxLevel;
	m_uOctantCount = 1;

	//subdivide
	if(ContainsMoreThan(m_uIdealEntityCount) == true){
		Subdivide();
	}

	AssignIDtoEntity();
	ConstructList();
}
void MyOctant::AssignIDtoEntity(void){
	//assigns IDs if there are children
	if(m_uChildren != 0){
		for (size_t i = 0; i < m_uChildren; i++){
			m_pChild[i]->AssignIDtoEntity();
		}
	}
	else{
		for(size_t i = 0; i < m_pEntityMngr->GetEntityCount(); i++){
			if(IsColliding(i) == true){
				m_EntityList.push_back(i);
				m_pEntityMngr->AddDimension(i, m_uID);
			}
		}
	}
}
void MyOctant::ConstructList(void){
	if(m_pChild[0] != nullptr){
		for(size_t i = 0; i < m_uChildren; i++){
			m_pChild[i]->ConstructList();
		}
	}
	if(m_EntityList.size() > 0){
		m_pParent->m_lChild.push_back(this);
	}
}