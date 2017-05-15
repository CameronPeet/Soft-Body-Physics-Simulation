#pragma once


#include <iostream>

#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

//Bullet includes in steps 

//STEP 1 - Setting up a world
#include <bullet\btBulletDynamicsCommon.h>

//STEP 2 - Collision Type Algorithms
#include <bullet\BulletCollision\Gimpact\btGImpactCollisionAlgorithm.h>
#include "btBulletDynamicsCommon.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btAlignedObjectArray.h" 

#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

