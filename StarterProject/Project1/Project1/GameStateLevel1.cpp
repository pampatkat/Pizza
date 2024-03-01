
#include "GameStateLevel1.h"
#include "CDT.h"
#include <iostream>


#include <irrKlang.h>
using namespace irrklang; 
ISoundEngine* SoundEngine;
// -------------------------------------------
// Defines
// -------------------------------------------

#define MESH_MAX					32				// The total number of Mesh (Shape)
#define TEXTURE_MAX					32				// The total number of texture
#define GAME_OBJ_INST_MAX			1024			// The total number of different game object instances
#define PLAYER_INITIAL_NUM			3				// initial number of ship lives
#define NUM_ASTEROID				30
#define SHIP_ACC_FWD				150.0f			// ship forward acceleration (in m/s^2)
#define SHIP_ACC_BWD				-180.0f			// ship backward acceleration (in m/s^2)
#define SHIP_ROT_SPEED				(2.0f * PI)		// ship rotation speed (degree/second)
#define HOMING_MISSILE_ROT_SPEED	(PI / 4.0f)		// homing missile rotation speed (degree/second)
#define BULLET_SPEED				300.0f			
#define ASTEROID_SPEED				250.0f	
#define MAX_SHIP_VELOCITY			200.0f

#define GIRL_SCALE					2

#define WINDOW_EDGE					25.0f

enum GAMEOBJ_TYPE
{
	// list of game object types
	//layer
	//TYPE_WHATYOUWANT,
	TYPE_ONE,
	TYPE_TWO,
	TYPE_THREE,
	TYPE_FOUR,
	TYPE_CONFIRM,
	TYPE_BACON,
	TYPE_BREAD,
	TYPE_SAUSE,
	TYPE_PEPEROINE,
	TYPE_INGREDIANT,
	TYPE_SAUSAGE,
	TYPE_GREENPEPPER,
	TYPE_ORDER,
	TYPE_SALE,
	TYPE_EGG,
	TYPE_GIRL,
	TYPE_BACKGROUND,
	TYPE_CHEESEONPIZZA,
	TYPE_SAUSAGEPIZZA,
	TYPE_POOP,
	TYPE_RAINBOW_POOP,
	TYPE_BULLET,
	TYPE_ASTEROID,
	TYPE_MISSILE
	
};

#define FLAG_INACTIVE		0
#define FLAG_ACTIVE			1


// -------------------------------------------
// Structure definitions
// -------------------------------------------

struct GameObj
{
	CDTMesh*		mesh;
	CDTTex*			tex;
	int				type;				// enum GAMEOBJ_TYPE
	int				flag;				// 0 - inactive, 1 - active
	glm::vec3		position;			// usually we will use only x and y
	glm::vec3		velocity;			// usually we will use only x and y
	glm::vec3		scale;				// usually we will use only x and y
	float			orientation;		// 0 radians is 3 o'clock, PI/2 radian is 12 o'clock
	glm::mat4		modelMatrix;
	int				frame;
	int				direction;
};


// -------------------------------------------
// Level variable, static - visible only in this file
// -------------------------------------------

static CDTMesh		sMeshArray[MESH_MAX];							// Store all unique shape/mesh in your game
static int			sNumMesh;
static CDTTex		sTexArray[TEXTURE_MAX];							// Corresponding texture of the mesh
static int			sNumTex;
static GameObj		sGameObjInstArray[GAME_OBJ_INST_MAX];			// Store all game object instance
static int			sNumGameObj;

//static GameObj*		swhatyouwant;
static GameObj*		sOne;
static GameObj*		sTwo;
static GameObj*		sThree;
static GameObj*		sFour;
static GameObj*		sBacon;
static GameObj*		sConfirm;
static GameObj*		sBread;
static GameObj*		sTomato;
static GameObj*		sPeperoni;
static GameObj*		sSausage;
static GameObj*		sGreenPepper;
static GameObj*		sIngrediant;
static GameObj*		sOrder;
static GameObj*		sCheeseOnPizza;
static GameObj*		sSuasageOnPizza;
static GameObj*		sSALE;
static GameObj*		sEgg;
static GameObj*		sPlayer;	
// Pointer to the Player game object instance
static GameObj*		sBackground;
static GameObj*		sPOOP;
static GameObj*		sRAINBOW_POOP;

static int			sHeartLives;									// The number of lives left
static int			sScore;

// functions to create/destroy a game object instance
static GameObj*		gameObjInstCreate(int type, glm::vec3 pos, glm::vec3 vel, glm::vec3 scale, float orient);
static void			gameObjInstDestroy(GameObj &pInst);


// -------------------------------------------
// Game object instant functions
// -------------------------------------------

GameObj* gameObjInstCreate(int type, glm::vec3 pos, glm::vec3 vel, glm::vec3 scale, float orient)
{
	// loop through all object instance array to find the free slot
	for (int i = 0; i < GAME_OBJ_INST_MAX; i++){
		GameObj* pInst = sGameObjInstArray + i;

		if (pInst->flag == FLAG_INACTIVE){
			
			pInst->mesh			= sMeshArray + type;
			pInst->tex			= sTexArray + type;
			pInst->type			= type;
			pInst->flag			= FLAG_ACTIVE;
			pInst->position		= pos;
			pInst->velocity		= vel;
			pInst->scale		= scale;
			pInst->orientation	= orient;
			pInst->modelMatrix  = glm::mat4(1.0f);
			pInst->frame		= 0;
			pInst->direction	= 0;

			sNumGameObj++;
			std::cout << "GameObj(" << sNumGameObj << ")\n";
			return pInst;
		}
	}

	// Cannot find empty slot => return 0
	return NULL;
}

void gameObjInstDestroy(GameObj &pInst)
{
	// Lazy deletion, not really delete the object, just set it as inactive
	if (pInst.flag == FLAG_INACTIVE)
		return;

	sNumGameObj--;
	pInst.flag = FLAG_INACTIVE;
}
void timer(int value) {
	/*setAngle(angle) {
		angle += 45;
	}*/
	//glutTimerFunc(40, Timer, 0);
}
void GameStateLevel1Load(void){

	// clear the Mesh array
	memset(sMeshArray, 0, sizeof(CDTMesh) * MESH_MAX);
	sNumMesh = 0;

	//+ clear the Texture array
	memset(sMeshArray, 0, sizeof(CDTTex) * TEXTURE_MAX);
	sNumMesh = 0;

	//+ clear the game object instance array
	memset(sGameObjInstArray, 0, sizeof(GameObj) * GAME_OBJ_INST_MAX);
	sNumMesh = 0;

	// Set the ship object instance to NULL
	sPlayer = NULL;
	

	// --------------------------------------------------------------------------
	// Create all of the unique meshes/textures and put them in MeshArray/TexArray
	//		- The order of mesh should follow enum GAMEOBJ_TYPE 
	/// --------------------------------------------------------------------------

	// Temporary variable for creating mesh
	CDTMesh* pMesh;
	CDTTex* pTex;
	std::vector<CDTVertex> vertices;
	CDTVertex v1, v2, v3, v4;
	/*vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 0.25f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u =  0.25f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("whatyouwant.png");*/

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("1.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("2.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("3.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("4.png");
	
	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("confirm.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("bacon.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("real_bread.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("sause.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("peperoni.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("ingrediants.png");	

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("sausage.png");
		
	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("green_pepper.png");
	
	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("order.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("SALE.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("egg.png");

	// Create Ship mesh/texture
	//walking
	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f / 10.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f / 10.0f; v3.v = 1.0f / 4.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f / 4.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("walking.png");

	//+ Create Background mesh/texture
	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("background.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("cheesepizaa.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("SausageOnPizza.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("SALE.png");

	printf("Level1: Load\n");
}


void GameStateLevel1Init(void){
	SoundEngine = createIrrKlangDevice();

	sBackground = gameObjInstCreate(TYPE_BACKGROUND, glm::vec3(0.0f, 0.0f , 0.0f),
								glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1500.0f, 800.0f, 1.0f), 0.0f);

	//swhatyouwant = gameObjInstCreate(TYPE_WHATYOUWANT, glm::vec3(-250.0f, -253.0f, 0.0f),
								//glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(374.0f, 264.0f, 1.0f), 0.0f);

	sConfirm = gameObjInstCreate(TYPE_CONFIRM, glm::vec3(200.0f, -253.0f, 0.0f),
								glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(150.0f, 150.0f, 1.0f), 0.0f);

	sBread = gameObjInstCreate(TYPE_BREAD, glm::vec3(-150.0f, 100.0f, 0.0f),
								glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(500.0f, 500.0f, 1.0f), 0.0f);

	sOrder = gameObjInstCreate(TYPE_ORDER, glm::vec3(300.0f, 100.0f, 0.0f),
								glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(350.0f, 450.0f, 1.0f), 0.0f);

	sPlayer = gameObjInstCreate(TYPE_GIRL, glm::vec3(0.0f,-GetWindowHeight()/4, 0.0f),
								glm::vec3(0.0f,0.0f,0.0f),glm::vec3(64.0f * GIRL_SCALE, 128.0f * GIRL_SCALE,1.0f),0.0f);

	sSALE = gameObjInstCreate(TYPE_SALE, glm::vec3(200.0f, -253.0f, 0.0f),
								glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -125.0f, 1.0f), 0.0f);

	sIngrediant = gameObjInstCreate(TYPE_INGREDIANT, glm::vec3(300.0f, 100.0f, 0.0f),
								glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(350.0f, 450.0f, 1.0f), 0.0f);

	printf("Level1: Init\n");
}


void GameStateLevel1Update(double dt, long frame, int &state){
	static int time = 0;
	static int Order_One = 0;
	//-----------------------------------------
	// Get user input
	//-----------------------------------------
	
	// Moving the Player
	//	- WS accelereate/deaccelerate the ship
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		
		// find acceleration vector
		glm::vec3 acc = glm::vec3(SHIP_ACC_FWD * glm::cos(sPlayer->orientation + PI / 2.0f), 
								  SHIP_ACC_FWD * glm::sin(sPlayer->orientation + PI / 2.0f), 0);

		// use acceleration to change velocity
		sPlayer->velocity += acc * glm::vec3(dt, dt, 0.0f);

		//+ velocity cap to MAX_SHIP_VELOCITY
		sPlayer->direction = 0;


	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		sPlayer->direction = 3;

		glm::vec3 acc = glm::vec3(SHIP_ACC_BWD * glm::cos(sPlayer->orientation + PI / 2.0f),
								  SHIP_ACC_BWD * glm::sin(sPlayer->orientation + PI / 2.0f), 0);

		sPlayer->velocity += acc * glm::vec3(dt, dt, 0.0f);
	}

	sPlayer->velocity = glm::vec3(0.0f, 0.0f, 0.0f);

	//+ AD: turn the ship
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		sPlayer->direction = 2;

		sPlayer->velocity = glm::vec3(-100.0f, 0.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		sPlayer->direction = 1;

		sPlayer->velocity = glm::vec3(100.0f, 0.0f, 0.0f);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		printf("Mouse at x: %f y: %f\n", xpos, ypos);

		if (xpos >= 824.0f && xpos <= 968.0f &&
			ypos >= 67.0f && ypos <= 154.0f) {
			std::cout << "TOMATO SAUSE Button pressed!" << std::endl;
			Order_One = +3;
			SoundEngine->play2D("splat.ogg", false);
			sTomato = gameObjInstCreate(TYPE_SAUSE, glm::vec3(-150.0f, 100.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(475.0f, 475.0f, 1.0f), 0.0f);
		}

		if (xpos >= 647.0f && xpos <= 765.0f &&
			ypos >= 67.0f && ypos <= 154.0f) {
			std::cout << "CHEESE Button pressed!" << std::endl;
			Order_One = +3;
			SoundEngine->play2D("splat.ogg", false);
			sCheeseOnPizza = gameObjInstCreate(TYPE_CHEESEONPIZZA, glm::vec3(-150.0f, 100.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(500.0f, 500.0f, 1.0f), 0.0f);
		}

		if (xpos >= 650.0f && xpos <= 769.0f &&
			ypos >= 369.0f && ypos <= 467.0f) {
			std::cout << "EGG Button pressed!" << std::endl;
			SoundEngine->play2D("splat.ogg", false);
			sEgg = gameObjInstCreate(TYPE_EGG, glm::vec3(-150.0f, 100.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(375.0f, 375.0f, 1.0f), 0.0f);
		}

		if (xpos >= 833.0f && xpos <= 899.0f &&
			ypos >= 370.0f && ypos <= 457.0f) {
			std::cout << "Greeb Pepper Button pressed!" << std::endl;
			SoundEngine->play2D("splat.ogg", false);
			sGreenPepper = gameObjInstCreate(TYPE_GREENPEPPER, glm::vec3(-150.0f, 100.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(500.0f, 500.0f, 1.0f), 0.0f);

		}

		if (xpos >= 665.0f && xpos <= 742.0f &&
			ypos >= 212.0f && ypos <= 300.0f) {
			std::cout << "Bacon Button pressed!" << std::endl;
			SoundEngine->play2D("splat.ogg", false);
			sBacon = gameObjInstCreate(TYPE_BACON, glm::vec3(-150.0f, 100.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(500.0f, 500.0f, 1.0f), 0.0f);

		}

		if (xpos >= 649.0f && xpos <= 775.0f &&
			ypos >= 577.0f && ypos <= 688.0f) {
			std::cout << "SALE Button pressed!" << std::endl;

			time++;
			std::cout << "TIME: " << time << std::endl;
			if (time == 0) {
				sTwo = gameObjInstCreate(TYPE_TWO, glm::vec3(-250.0f, -253.0f, 0.0f),
					glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(374.0f, 264.0f, 1.0f), 0.0f);
				if (Order_One == 15) 
				{
					std::cout << "Order Correct" << std::endl;
				}
				else 
				{
					std::cout << "Order   Wrong " << std::endl;
				}
			}
			else if (time == 1) {
				sTwo = gameObjInstCreate(TYPE_TWO, glm::vec3(-250.0f, -253.0f, 0.0f),
					glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(374.0f, 264.0f, 1.0f), 0.0f);
			}
			else if(time == 2)
				sThree = gameObjInstCreate(TYPE_THREE, glm::vec3(-250.0f, -253.0f, 0.0f),
					glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(374.0f, 264.0f, 1.0f), 0.0f);
			else if (time == 3)
				sFour = gameObjInstCreate(TYPE_FOUR, glm::vec3(-250.0f, -253.0f, 0.0f),
					glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(374.0f, 264.0f, 1.0f), 0.0f);

			time++;
			if (time >= 4)
				time = NULL;

			
			for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {

				GameObj* pInst1 = sGameObjInstArray + i;
				if(pInst1->type == TYPE_EGG || pInst1->type == TYPE_BACON || pInst1->type == TYPE_CHEESEONPIZZA || pInst1->type == TYPE_SAUSE)
				gameObjInstDestroy(*pInst1);

			}
			
		}
	}

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS){ }

	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS){
		ZoomIn(0.1f);
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
		ZoomOut(0.1f);
	}
	
	for (int i = 0; i < GAME_OBJ_INST_MAX; i++){
		GameObj* pInst = sGameObjInstArray + i;

		// skip inactive object
		if (pInst->flag == FLAG_INACTIVE)
			continue;

		if (pInst->type == TYPE_GIRL){

			//+ for ship: add some friction to slow it down

			//+ use velocity to update the position
			sPlayer->position += sPlayer->velocity * (float)dt;
		}
		else if ((pInst->type == TYPE_POOP)){
			
			//+ use velocity to update the position
			pInst->position += pInst->velocity * (float)dt;

		}
		if ((pInst->type == TYPE_RAINBOW_POOP)) {

			//+ use velocity to update the position
			pInst->position += pInst->velocity * (float)dt;

		}
	}


	//-----------------------------------------
	// Update some game obj behavior
	//	- wrap ship around the screen
	//	- destroy bullet that go out of the screen
	//-----------------------------------------

	for (int i = 0; i < GAME_OBJ_INST_MAX; i++){
		GameObj* pInst = sGameObjInstArray + i;

		// skip inactive object
		if (pInst->flag == FLAG_INACTIVE)
			continue;

		if ((pInst->type == TYPE_GIRL) || (pInst->type == TYPE_POOP) || (pInst->type == TYPE_RAINBOW_POOP)){

			//+ wrap the ship and asteroid around the screen 
			if (pInst->position.x < -GetWindowWidth() / 2 - WINDOW_EDGE) {
				pInst->position.x = GetWindowWidth() / 2 + WINDOW_EDGE;
			}
			if (pInst->position.x > GetWindowWidth() / 2 + WINDOW_EDGE) {
				pInst->position.x = -GetWindowWidth() / 2 - WINDOW_EDGE;
			}
			if (pInst->position.y < -GetWindowHeight() / 2 - WINDOW_EDGE) {
				pInst->position.y = GetWindowHeight() / 2 + WINDOW_EDGE;
			}
			if (pInst->position.y > GetWindowHeight() / 2 + WINDOW_EDGE) {
				pInst->position.y = -GetWindowHeight() / 2 - WINDOW_EDGE;
			}
		}
	}
	
	
	//-----------------------------------------
	// Check for collsion, O(n^2)
	//-----------------------------------------

	for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
		GameObj* pInst1 = sGameObjInstArray + i;

		// skip inactive object
		if (pInst1->flag == FLAG_INACTIVE)
			continue;

		// if pInst1 is an asteroid
		if (pInst1->type == TYPE_POOP || pInst1->type == TYPE_RAINBOW_POOP) {

			// compare pInst1 with all game obj instances 
			for (int j = 0; j < GAME_OBJ_INST_MAX; j++) {
				GameObj* pInst2 = sGameObjInstArray + j;

				// skip inactive object
				if (pInst2->flag == FLAG_INACTIVE)
					continue;

				// skip asteroid object
				if (pInst2->type == TYPE_POOP)
					continue;
				if (pInst2->type == TYPE_RAINBOW_POOP)
					continue;

				if (pInst2->type == TYPE_GIRL) {

					bool collide = true;

					//+ Check for collsion
					float distance = glm::length(pInst2->position - pInst1->position);
					if (distance > (pInst1->scale.x + pInst2->scale.x) / 2.0f)
					{
						collide = false;
					}
					if (collide) {
						gameObjInstDestroy(*pInst1);
						GameObj* pInst2 = sGameObjInstArray + j;
						sHeartLives--;

						break;
					}
				}

			}
		}
	}
	//-----------------------------------------
	// Update animations
	//-----------------------------------------

	if ((frame % 10) == 0 && ++(sPlayer->frame) == 10)
		sPlayer->frame = 0;
	

	//-----------------------------------------
	// Update modelMatrix of all game obj
	//-----------------------------------------
	
	for (int i = 0; i < GAME_OBJ_INST_MAX; i++  ){
		GameObj* pInst = sGameObjInstArray + i;

		// skip inactive object
		if (pInst->flag == FLAG_INACTIVE)
			continue;

		glm::mat4 rMat = glm::mat4(1.0f);
		glm::mat4 sMat = glm::mat4(1.0f);
		glm::mat4 tMat = glm::mat4(1.0f);

		// Compute the scaling matrix
		sMat = glm::scale(glm::mat4(1.0f), pInst->scale);

		//+ Compute the rotation matrix, we should rotate around z axis
		rMat = glm::rotate(glm::mat4(1.0f), pInst->orientation, glm::vec3(0, 0, 1));

		//+ Compute the translation matrix
		tMat = glm::translate(glm::mat4(1.0f), pInst->position);

		// Concatenate the 3 matrix to from Model Matrix
		pInst->modelMatrix = tMat * sMat * rMat;
	}

	//printf("Life> %i\n", sPlayerLives);
	//printf("Score> %i\n", sScore);
}

void GameStateLevel1Draw(void){

	// Clear the screen
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw all game object instance in the sGameObjInstArray
	for (int i = 0; i < GAME_OBJ_INST_MAX; i++){
		GameObj* pInst = sGameObjInstArray + i;

		// skip inactive object
		if (pInst->flag == FLAG_INACTIVE)
			continue;

		// 4 steps to draw sprites on the screen
		//	1. SetRenderMode()
		//	2. SetTexture()
		//	3. SetTransform()
		//	4. DrawMesh()

		SetRenderMode(CDT_TEXTURE, 1.0f);

	

		if (pInst->type == TYPE_GIRL)
			SetTexture(*pInst->tex, pInst->frame / 10.0f, pInst->direction / 4.0f);
		else
			SetTexture(*pInst->tex, 0.0f, 0.0f);

		SetTransform(pInst->modelMatrix);
		DrawMesh(*pInst->mesh);
	}

	// Swap the buffer, to present the drawing
	glfwSwapBuffers(window);
}

void GameStateLevel1Free(void){

	//+ call gameObjInstDestroy for all object instances in the sGameObjInstArray

	// reset camera
	ResetCam();

	printf("Level1: Free\n");
}

void GameStateLevel1Unload(void){

	// Unload all meshes in MeshArray
	for (int i = 0; i < sNumMesh; i++){
		UnloadMesh(sMeshArray[i]);
	}

	//+ Unload all textures in TexArray


	printf("Level1: Unload\n");
}
