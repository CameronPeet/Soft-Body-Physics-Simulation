#pragma once


#include <iostream>

//Bullet includes in steps 

//STEP 1 - Setting up a world
#include <bullet\btBulletDynamicsCommon.h>

//STEP 2 - Collision Type Algorithms
#include <bullet\BulletCollision\Gimpact\btGImpactCollisionAlgorithm.h>


#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"


int mai2n(int argc, char ** argv)
{
	std::cout << "Hello World!" << std::endl;


	//STEP 1:
	//Defines the broadphase algorithm
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();

	//Collision configuration allows us to fine tune the algorithms used for the full collision detection (not broadphase)
	btSoftBodyRigidBodyCollisionConfiguration* collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	//If you introduce different types of collision objects later  on (eg meshes using btGimpactMeshShape) then we may need
	//to register a collision algorithm to get collision recognised
	//Register the collision dispatcher with an addative collison algorithm
	btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);

	//Adding a solver, this causes objects to interact properly, taking into account gravity, game logic, supplied forces, collisons and hinge constrains.
	//Does well if not pushed, bottlenecks at high performance simulations. Parralel versions available for threading models
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

	//Instantiate the dynamics world
	btSoftRigidDynamicsWorld* dynamicsWorld = new btSoftRigidDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	//Set the Gravity!
	dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

	btSoftBodyWorldInfo softBodyWorldInfo;
	softBodyWorldInfo.m_broadphase = broadphase;
	softBodyWorldInfo.m_dispatcher =dispatcher;
	softBodyWorldInfo.m_gravity = dynamicsWorld->getGravity();
	softBodyWorldInfo.m_sparsesdf.Initialize();
	//Interem Code
	//Creating shapes

	const btScalar s = 4;
	const int numX = 31;
	const int numY = 31;
	const int fixed = 1 + 2;
	btSoftBody* cloth = btSoftBodyHelpers::CreatePatch(softBodyWorldInfo,
		btVector3(-s / 2, s + 1, 0),
		btVector3(+s / 2, s + 1, 0),
		btVector3(-s / 2, s + 1, +s),
		btVector3(+s / 2, s + 1, +s),
		numX, numY,
		fixed, true);

	cloth->getCollisionShape()->setMargin(0.001f);
	cloth->generateBendingConstraints(2, cloth->appendMaterial());
	cloth->setTotalMass(10);
	//cloth->m_cfg.citerations = 10;
	//	cloth->m_cfg.diterations = 10;
	cloth->m_cfg.piterations = 5;
	cloth->m_cfg.kDP = 0.005f;
	dynamicsWorld->addSoftBody(cloth);


	for (int i = 0; i < 300; i++) {

		dynamicsWorld->stepSimulation(1 / 60.f, 10);

	}


	//Exiting code - he who allocates must also delete
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;

	return 0;
}