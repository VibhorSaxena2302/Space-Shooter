//Libraries
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <cstdlib>
#include <string>
using namespace std;

//GameSettings
	int MeteorSpawnProbability = 50; //Meteor spawn chance every meteor spawn check
	int MeteorSpawnDelay = 5; //Time between consecutive meteor spawn checks
	int MeteorSpawnCountPerCheck = 2; //Number of meteors that spawn every meteor spawn check
	int MaxMeteorSpawnCountPerCheck = 10; //Max number of meteors that spawn every meteor spawn check
	float LastMeteorSpawnTime = GetTime(); //Float value consisting of time when last meteor spawn check occured
	
	float LastIncrementTime = GetTime(); //Float value consisting of time when last increment occured
	int MeteorSpawnIncrementDelay = 10; //Time between consecutive meteor spawn increments
	int MeteorSpawnIncrementCount = 1; //Int value representing how much meteor spawn increments

	int Score = 0; //Current player in-game score
	int HighScore = 0; //Highest score player achieved during play-time

	int GameState = 0; //0-Menu, 1-PlayGame
	
	float MouseRadius = 25.0f; //Mouse Circumference Boundary

//GamepadSettings
	int Gamepad = 0; //Gamepad ID
	Vector2 mousePosition = { 1250 / 2, 750 / 2 }; //Initial mouse position
	float mouseXSens = 25.0f; //Ship rotation horizontal sensitivity
	float mouseYSens = 25.0f; //Ship rotation vertical sensitivity
	
Rectangle screen = { 0, 0, 1250, 750 }; //Game window width and height

//SoundEffects
	Sound LaserPewSound;
	Sound SpaceBGM;
	Sound MeteorExplode;
	Sound EarthHit;
	Sound GameOver;

//PlayerShip class consisting of Data members related to Player's ship
class PlayerShip {
	public:
	Texture2D PlayerShipTexture = LoadTexture("/Users/harshit/Documents/RaylibProjects/Template-4.5.0/Assets/Spaceship.png"); //Player's ship texture
	Rectangle ShipRectangle = { 0, 0, (float)(PlayerShipTexture.width), (float)(PlayerShipTexture.height) }; //Player's ship hitbox
	Vector2 ShipCenter = { (float)(PlayerShipTexture.width) / 2.0f, (float)(PlayerShipTexture.height) / 2.0f }; //Player's ship center position
	Vector2 playerpos = { (float)(screen.width) / 2.0f, (float)(screen.height) - (float)(PlayerShipTexture.height) }; //Default position of Player's ship
	float PlayerSpeed = 500; //Float value representing how fast Player's ship moves
	float FireRate = 0.4f; //Float value representing delay between projectile shots (in seconds)
	float LastProjectileShot = GetTime(); //Float value representing last projectile shot time
	float LastGamepadX = 0.00f;
	float LastGamepadY = 0.00f;
};

//Map class consisting of Data members related to Background map
class Map {
	public:
	Texture2D Background = LoadTexture("/Users/harshit/Documents/RaylibProjects/Template-4.5.0/Assets/SpaceBackground.png"); //Background map texture
};

//Earth class consisting of Data members related to Earth
class Earth {
	public:
	Texture2D EarthTExture = LoadTexture("/Users/harshit/Documents/RaylibProjects/Template-4.5.0/Assets/Earth.png"); //Earth's texture
	Rectangle EarthRectangle = { 0, 0, (float)(EarthTExture.width), (float)(EarthTExture.height) }; //Earth's hitbox
	Vector2 EarthCenter = { (float)(EarthTExture.width) / 2.0f, (float)(EarthTExture.height) / 2.0f }; //Earth's center position

	float MaxEarthHP = 1000; //Max Earth Health Points
	float CurrentHP = MaxEarthHP; //Current health point of Earth

	Texture2D HPBarEmpty = LoadTexture("/Users/harshit/Documents/RaylibProjects/Template-4.5.0/Assets/HPBarNil.png"); //HP bar background texture
	Rectangle HPBarEmptyRectangle = { 0, 0, (float)(HPBarEmpty.width), (float)(HPBarEmpty.height) }; //HP bar background hitbox
	Vector2 HPBarEmptyCenter = { (float)(HPBarEmpty.width) / 2.0f, (float)(HPBarEmpty.height) / 2.0f }; //HP bar background center position

	Texture2D HPBar = LoadTexture("/Users/harshit/Documents/RaylibProjects/Template-4.5.0/Assets/HPBarMax.png"); //HP bar texture
	Rectangle HPBarRectangle = { 0, 0, (float)(HPBar.width), (float)(HPBar.height) }; //HP bar hitbox
	Vector2 HPBarCenter = { (float)(HPBar.width) / 2.0f, (float)(HPBar.height) / 2.0f }; //HP bar center position

	float HPPercent = (CurrentHP / MaxEarthHP); //Ratio of CurrentHP to MaxHP
};

//Projectile class consisting of data members related to Projectile
class Projectile {
	public:
	Texture2D ProjectileTexture = LoadTexture("/Users/harshit/Documents/RaylibProjects/Template-4.5.0/Assets/Projectile.png"); //Projectile texture
	Rectangle ProjectileRectangle = { 0, 0, (float)(ProjectileTexture.width), (float)(ProjectileTexture.height) }; //Projectile hitbox
	Vector2 ProjectileCenter = { (float)(ProjectileTexture.width) / 2.0f, (float)(ProjectileTexture.height) / 2.0f }; //Projectile's center position
	Vector2 position; //Vector2 Datatype consisting of projectile's position in X and Y co-ordinates
	Vector2 velocity; //Vector2 Datatype representing the velocity of Projectile
	Vector2 mouseposition; //Mouse Position in terms of X and Y co-ordinates
	float SpawnTime; //Float value representing the spawn time of the projectile
	int Speed = 20; //Int value representing the speed of the projectile
	float SpawnDuration = .50f; //Float value representing the duration (in seconds) projectile lasts for
	float Rotation = 0.00f;
};
std::vector<Projectile> projectiles; //Variable projectiles consisting of all Projectile objects 
//Projectile function which returns a projectile object after creating and changing the initial values of projectile from the Projectile class
Projectile ShootProjectile(float PosX, float PosY, PlayerShip&_PlayerShip) {
	Projectile projectile; //Creating projectile object
	projectile.position = { PosX, PosY }; //Setting the position of projectile to PosX and PosY given
	Vector2 mousePosition = GetMousePosition(); //Getting the position of Mouse
	projectile.mouseposition = mousePosition; //Setting the projectile's mouseposition to Mouse position obtained in above line
	projectile.velocity = Vector2Normalize(Vector2Subtract(mousePosition, projectile.position)); //Setting projectile's velocity based on mouse position and projectile's spawn position
	projectile.velocity = Vector2Scale(projectile.velocity, projectile.Speed); //Multiplying the projectile's velocity by a scaler value (projectile's speed) to set how fast the projectile moves
	projectile.SpawnTime = GetTime(); //Setting the spawn time of projectile
	projectile.Rotation = 90 + (atan2(projectile.mouseposition.x - _PlayerShip.playerpos.x, projectile.mouseposition.y - _PlayerShip.playerpos.y) * -57.29578f);
	PlaySound(LaserPewSound);
	return projectile; //Returning the projectile object after modifying it's data members
}

//Meteor class consisting of data members related to Meteor
class Meteor {
	public:
	Texture2D MeteorTexture = LoadTexture("/Users/harshit/Documents/RaylibProjects/Template-4.5.0/Assets/Meteor.png"); //Meteor's texture
	Rectangle MeteorRectangle = { 0, 0, (float)(MeteorTexture.width), (float)(MeteorTexture.height) }; //Meteor's hitbox
	Vector2 MeteorCenter = { (float)(MeteorTexture.width) / 2.0f, (float)(MeteorTexture.height) / 2.0f }; //Meteor's center position
	Vector2 position; //Meteor's position
	Vector2 velocity; //Meteor's velocity determining where meteor travels
	int Speed = 2; //Int value representing the speed of the meteor
};
//Meteor function which returns a meteor object after spawning it
Meteor SpawnMeteor(float TargetPosX, float TargetPosY) {
	Meteor meteor; //Creating Meteor object
	float PosX; //X Co-ordinates of meteor object
	float PosY; //Y Co-ordinates of meteor object
	int Side = rand() % 2; //Determining whether meteor spawns on Horizontal edges or Vertical edges
	if (Side == 0) { //Meteor spawns on Horizontal edge
		int HorizontalSide = rand() % 2; //Determining whether meteor spawns on Left edge or Right edge
		if (HorizontalSide == 0) { //Meteor spawns on Left edge
			PosX = 0.0f; //Setting X co-ordinates to Left edge
		}
		else { //Meteor spawns on Right edge
			PosX = 1250.0f; //Setting X co-ordinates to Right edge
		}
		PosY = (float)(rand() % 750); //Randomly choose Y co-ordinates from Top edge to Bottom edge
	}
	else { //Meteor spawns on Vertical edge
		int VerticalSide = rand() % 2; //Determining whether meteor spawns on Top edge or Bottom edge
		if (VerticalSide == 0) { //Meteor spawns on Top edge
			PosY = 0.0f; //Setting Y co-ordinates to Top edge
		}
		else { //Meteor spawns on Bottom edge
			PosY = 750.0f; //Setting Y co-ordinates to Bottom edge
		} 
		PosX = (float)(rand() % 1250); //Randomly choose X co-ordinates from Left edge to Right edge
	}
	meteor.position = { PosX, PosY }; //Setting the position of meteor using PosX and posY acquired aboce
	Vector2 TargetPosition = { TargetPosX, TargetPosY }; //Setting the target location where meteor will travel to
	meteor.velocity = Vector2Normalize(Vector2Subtract(TargetPosition, meteor.position)); //Setting meteor's velocity based on meteor's position and target position
	meteor.velocity = Vector2Scale(meteor.velocity, meteor.Speed); //Scaling meteor's velocity by meteor's speed to increase how fast meteor travels
	return meteor; //returning meteor object
}
std::vector<Meteor> meteors; //Variable meteors consisting of all Meteor objects

//Explosion class consisting of all data members related to Explosion
class Explosion{
	public:
	Texture2D ExplosionTexture = LoadTexture("/Users/harshit/Documents/RaylibProjects/Template-4.5.0/Assets/ExplosionSprite.png"); //Explosion's texture
	float SpriteWidth = 65.00f; //Setting sprite width from sprite sheet
	float SpriteHeight = 96.00f; //Setting sprite height from sprite sheet
	int SpriteCount = 7; //Setting sprite count
	int SpriteIndex = 1; //Setting current index of sprite sheet
	float AnimationDuration = .50f; //How fast animation plays
	float SpawnTime; //Float value representing the spawn time of Explosion
	Vector2 position; //Position of Explosion
	Rectangle SpriteRectangle = {0, 0, SpriteWidth, SpriteHeight}; //Explosion's hitbox
};
std::vector<Explosion> explosions; //Variable explosions consisting of all Explosion objects

//PlayerMovement Function called every frame controlling how Player's ship moves using WASD keys
void PlayerMovement(PlayerShip&_PlayerShip){
	Vector2 playermotion = { 0, 0 }; //Initial player motion
	
	if (IsGamepadAvailable(Gamepad)) {
		if (GetGamepadAxisMovement(0, 0)<0 and _PlayerShip.playerpos.x > (_PlayerShip.PlayerShipTexture.width / 3)){
			playermotion.x += -1; //Increase player's X motion by -1 (Towards left of screen)	
		}
		if (GetGamepadAxisMovement(0, 0)>0 and _PlayerShip.playerpos.x < (screen.width - _PlayerShip.PlayerShipTexture.width / 3)){
			playermotion.x += 1; //Increase player's X motion by 1 (Towards right of screen)
		}
		if (GetGamepadAxisMovement(0, 1)<0 and _PlayerShip.playerpos.y > (_PlayerShip.PlayerShipTexture.height / 3)){
			playermotion.y += -1; //Increase player's Y motion by -1 (Towards top of screen)
		}
		if (GetGamepadAxisMovement(0, 1)>0 and _PlayerShip.playerpos.y < (screen.height - _PlayerShip.PlayerShipTexture.height / 3)){
			playermotion.y += 1; //Increase player's Y motion by 1 (Towards bottom of screen)
		}
	}

	if ((IsKeyDown(KEY_A) and _PlayerShip.playerpos.x > (_PlayerShip.PlayerShipTexture.width / 3))) { //If key held is A and Player's ship is not out of Left edge of screen
		playermotion.x += -1; //Increase player's X motion by -1 (Towards left of screen)
	}
	if (IsKeyDown(KEY_D) and _PlayerShip.playerpos.x < (screen.width - _PlayerShip.PlayerShipTexture.width / 3)) {//If key held is D and Player's ship is not out of Right edge of screen
		playermotion.x += 1; //Increase player's X motion by 1 (Towards right of screen)
	}
	if (IsKeyDown(KEY_W) and _PlayerShip.playerpos.y > (_PlayerShip.PlayerShipTexture.height / 3)) {//If key held is W and Player's ship is not out of Top edge of screen
		playermotion.y += -1; //Increase player's Y motion by -1 (Towards top of screen)
	}
	if (IsKeyDown(KEY_S) and _PlayerShip.playerpos.y < (screen.height - _PlayerShip.PlayerShipTexture.height / 3)) {//If key held is S and Player's ship is not out of Bottom edge of screen
		playermotion.y += 1; //Increase player's Y motion by 1 (Towards bottom of screen)
	}

	Vector2 movementThisFrame = Vector2Scale(playermotion, GetFrameTime() * _PlayerShip.PlayerSpeed); //Determine the movement the player's ship will move towards in current frame

	_PlayerShip.playerpos = Vector2Add(_PlayerShip.playerpos, movementThisFrame); //Setting player's ship position based on Movement occured in this frame
}

//Reset_Game_Attributes Function called after game ends to reset certain changed values to default
void Reset_Game_Attributes(Earth&Game_Earth, PlayerShip&Game_PlayerShip){
	Game_PlayerShip.playerpos = { (float)(screen.width) / 2.0f, (float)(screen.height) - (float)(Game_PlayerShip.PlayerShipTexture.height) }; //Setting player's ship position to default position
	Game_Earth.CurrentHP = Game_Earth.MaxEarthHP; //Setting Earth's current HP back to Max HP
	if (Score > HighScore){ //Checking if player's score is higher than highest score
		HighScore = Score; //If higher than highest score, Set highest score to current score
	}
	Score = 0; //Reset score back to 0
	MeteorSpawnCountPerCheck = 2; //Reset all meteor increments taken place
	//Destroy all projectiles
	for (int i = 0; i < projectiles.size(); i++) {
		projectiles.erase(projectiles.begin() + i);
	}
	//Destroy all meteors
	for (int i = 0; i < meteors.size(); i++) {
		meteors.erase(meteors.begin() + i);
	}
	//Destroy all explosions
	for (int i = 0; i < explosions.size(); i++) {
		explosions.erase(explosions.begin() + i);
	}
}

//GameOverCheck Function called every frame to check if Earth's HP > 0, if not end game
void GameOverCheck(Earth&_Earth, PlayerShip&_PlayerShip){
	if (_Earth.CurrentHP <= 0) {
		PlaySound(GameOver);
		Reset_Game_Attributes(_Earth, _PlayerShip); //If Earth's HP <= 0, reset game attributes
		GameState = 0; //Return to Menu
	}
}

//ProjectileSpawnCheck Function called every frame to check if player pressed C and a projectile should spawn or not
void ProjectileSpawnCheck(PlayerShip&_PlayerShip, float Current_Time){
	if ((IsKeyDown(KEY_C)&&(Current_Time-_PlayerShip.LastProjectileShot>=_PlayerShip.FireRate))||((IsGamepadAvailable(Gamepad)&&(GetGamepadButtonPressed()==9))&&(Current_Time-_PlayerShip.LastProjectileShot>=_PlayerShip.FireRate))) { //Checking if player pressed C and Projectile is off cooldown
		_PlayerShip.LastProjectileShot = GetTime(); //Setting player's ship last projectile shot time to current time
		Projectile projectile = ShootProjectile(_PlayerShip.playerpos.x, _PlayerShip.playerpos.y, _PlayerShip); //Calling function ShootProjecile and specifying player's ship position to get a projectile object 
		projectiles.push_back(projectile); //Putting the projectile object in projectiles
	}
}

//ProjectileUpdate Function called every frame to update Projectile's position based on projectile's velocity
void ProjectileUpdate(float Current_Time){
	for (int i = 0; i < projectiles.size(); i++) { //Looping through all projectiles
		Projectile& projectile = projectiles[i];
		if ((Current_Time - projectile.SpawnTime)>=projectile.SpawnDuration){ //Checking if projectile should be deleted based on it's spawn time and spawn duration
				projectiles.erase(projectiles.begin() + i); //Deleting projectile from projectiles
		}
		else{
			projectile.position = Vector2Add(projectile.position, projectile.velocity); //Update projectile's position if not deleted
		}
	}
}

//MeteorUpdate Function called every frame to update Meteor's position based on meteor's velocity
void MeteorUpdate(float Current_Time){
	if ((Current_Time-LastIncrementTime)>=MeteorSpawnIncrementDelay){ //Checking if Meteor spawn count should increment
		if (MeteorSpawnCountPerCheck<MaxMeteorSpawnCountPerCheck){ //Checking if Current meteor spawn count < Max meteor spawn count
			MeteorSpawnCountPerCheck += MeteorSpawnIncrementCount; //Incrementing meteor spawn count
		}
		LastIncrementTime = GetTime(); //Setting Last increment time to current time
	}
	if (Current_Time - LastMeteorSpawnTime >= MeteorSpawnDelay) { //Checking if meteor can spawn
		LastMeteorSpawnTime = GetTime(); //Setting meteor spawn to current time
		for (int i = 1; i <= MeteorSpawnCountPerCheck; i++){ //Running loop to Meteor spawn count
			int randnum = rand() % 101; //Getting a random number upto 100
			if (randnum <= MeteorSpawnProbability) { //Checking if randnum is <= Meteor span probability (Acts as a system to spawn meteor based on probability percentage)
				Meteor meteor = SpawnMeteor((float)(screen.width) / 2.0f, (float)(screen.height) / 2.0f); //Spawning meteor
				meteors.push_back(meteor); //Putting meteor object in meteors
			}
		}
	}
	for (int i = 0; i < meteors.size(); i++) { //Looping through meteors
		Meteor& meteor = meteors[i];
		meteor.position = Vector2Add(meteor.position, meteor.velocity); //Updating meteor position in this frame based on meteor's position and meteor's velocity
	}
}

//DrawTextures Function called every frame to draw textures such as player's ship, meteors, explosion, projectiles etc.
void DrawTextures(Map&_Map, Earth&_Earth, PlayerShip&_PlayerShip){
	DrawTexturePro(_Map.Background, screen, screen, {0, 0}, 0, WHITE); //Draw background texture

	DrawTexturePro(_Earth.EarthTExture, _Earth.EarthRectangle, { screen.width / 2, screen.height / 2, _Earth.EarthRectangle.width, _Earth.EarthRectangle.height }, _Earth.EarthCenter, 0, WHITE); //Draw Earth texture
	DrawTexturePro(_Earth.HPBarEmpty, _Earth.HPBarEmptyRectangle, { screen.width / 2, (float)(_Earth.HPBarEmpty.height / 2), _Earth.HPBarEmptyRectangle.width, _Earth.HPBarEmptyRectangle.height }, 
	_Earth.HPBarEmptyCenter, 0, WHITE); //Draw Background HP bar texture
	DrawTexturePro(_Earth.HPBar, _Earth.HPBarRectangle, { screen.width / 2, (float)(_Earth.HPBar.height / 2), _Earth.HPBarRectangle.width * _Earth.HPPercent, _Earth.HPBarRectangle.height },
	 { (((float)_Earth.HPBar.width) * _Earth.HPPercent) / 2, (float)(_Earth.HPBar.height) / 2 }, 0, GREEN); //Draw HP bar texture

	//Draw meteor texture by looping in meteors
	for (int i = 0; i < meteors.size(); i++) {
		Meteor& meteor = meteors[i];
		DrawTexturePro(meteor.MeteorTexture, meteor.MeteorRectangle, { meteor.position.x, meteor.position.y, meteor.MeteorRectangle.width, meteor.MeteorRectangle.height }, meteor.MeteorCenter, 0, WHITE);
	}

	DrawTexturePro(_PlayerShip.PlayerShipTexture, _PlayerShip.ShipRectangle, { _PlayerShip.playerpos.x, _PlayerShip.playerpos.y, _PlayerShip.ShipRectangle.width, _PlayerShip.ShipRectangle.height },
	 _PlayerShip.ShipCenter, 180 + (atan2(GetMouseX() - _PlayerShip.playerpos.x, GetMouseY() - _PlayerShip.playerpos.y) * -57.29578f), WHITE); //Draw player's ship texture

	//Draw projectile texture by looping in projectiles
	for (int i = 0; i < projectiles.size(); i++) {
		Projectile& projectile = projectiles[i];
		DrawTexturePro(projectile.ProjectileTexture, projectile.ProjectileRectangle, { projectile.position.x, projectile.position.y, projectile.ProjectileRectangle.width, projectile.ProjectileRectangle.height },
		 projectile.ProjectileCenter, projectile.Rotation, WHITE);
	}
}

//DrawAnimatedTextures Function called every Frame to draw explosion sprite sheet animation
void DrawAnimatedTextures(){
	for (int i = 0; i < explosions.size(); i++) { //Looping through explosion objects in explosions
		Explosion& explosion = explosions[i]; //Getting explosion object
		
		//Setting sprite index of explosion based on time elapsed since it spawned and AnimationDuration of explosion
		if (GetTime() - explosion.SpawnTime <= explosion.AnimationDuration/explosion.SpriteCount){
			explosion.SpriteIndex = 1;
		}
		else if (GetTime() - explosion.SpawnTime <= 2*explosion.AnimationDuration/explosion.SpriteCount){
			explosion.SpriteIndex = 2;
		}
		else if (GetTime() - explosion.SpawnTime <= 3*explosion.AnimationDuration/explosion.SpriteCount){
			explosion.SpriteIndex = 3;
		}
		else if (GetTime() - explosion.SpawnTime <= 4*explosion.AnimationDuration/explosion.SpriteCount){
			explosion.SpriteIndex = 4;
		}
		else if (GetTime() - explosion.SpawnTime <= 5*explosion.AnimationDuration/explosion.SpriteCount){
			explosion.SpriteIndex = 5;
		}
		else if (GetTime() - explosion.SpawnTime <= 6*explosion.AnimationDuration/explosion.SpriteCount){
			explosion.SpriteIndex = 6;
		}
		else if (GetTime() - explosion.SpawnTime <= 7*explosion.AnimationDuration/explosion.SpriteCount){
			explosion.SpriteIndex = 7;
		}
    	explosion.SpriteRectangle = Rectangle{explosion.SpriteIndex * explosion.SpriteWidth, 0, explosion.SpriteWidth, explosion.SpriteHeight}; //Setting explosion's rectangle based on sprite index
		DrawTextureRec(explosion.ExplosionTexture, explosion.SpriteRectangle, { explosion.position.x, explosion.position.y}, WHITE); //Draw explosion texture
		if (explosion.SpriteIndex==7){
			explosions.erase(explosions.begin()+i); //If sprite index is 7, destroy explosion object as it reached the end of the sprite sheet animation
		}
	}
}

//CheckProjectMeteorCollision Function called every Frame to check if Projectile and Meteor collided
void CheckProjectileMeteorCollision(){
	for (int i = 0; i < projectiles.size(); i++) { //Looping through projectiles
		for (int j = 0; j < meteors.size(); j++) { //Looping through meteors
			if (CheckCollisionRecs({projectiles[i].position.x, projectiles[i].position.y, projectiles[i].ProjectileRectangle.width, projectiles[i].ProjectileRectangle.height},
			 {meteors[j].position.x, meteors[j].position.y, meteors[j].MeteorRectangle.width, meteors[j].MeteorRectangle.height})) { // Checking if meteor and projectile hitbox collide
				Score = Score + 1; //Incrementing the score
				Explosion explosion; //Creating explosion object
				explosion.SpawnTime = GetTime(); //Setting explosion spawn time to current time
				explosion.position = {(meteors[j].position.x), (meteors[j].position.y)}; //Setting explosion's position to meteor's position
				explosions.push_back(explosion); //Putting explosion object in explosions
				projectiles.erase(projectiles.begin() + i); //Deleting projectile object
				meteors.erase(meteors.begin() + j); //Deleting meteor object
				PlaySound(MeteorExplode);
			}
		}
	}
}

//CheckMeteorEarthCollision Function called every Frame to check if Meteor has collided with Earth
void CheckMeteorEarthCollision(Earth&_Earth){
	for (int j = 0; j < meteors.size(); j++) { //Looping through meteor objects
		if (CheckCollisionRecs({ screen.width / 2, screen.height / 2, _Earth.EarthRectangle.width, _Earth.EarthRectangle.height },
		 { meteors[j].position.x, meteors[j].position.y, meteors[j].MeteorRectangle.width, meteors[j].MeteorRectangle.height })) { //Checking if Meteor object hitbox and Earth hitbox collide
			meteors.erase(meteors.begin() + j); //Deleting meteor object from meteors
			_Earth.CurrentHP = _Earth.CurrentHP - 50; //Reducing Earth's HP by 50
			PlaySound(EarthHit);
		}
	}
}

//DisplayScore Function called every Frame to display Score
void DisplayScore(){
	DrawText(to_string(Score).c_str(), 20, 40, 50, WHITE); //Draw a text of score converted to string from int using to_string(Score).c_str() function
}

void SetMousePositionUsingGamepad(PlayerShip&_PlayerShip){
	if (IsGamepadAvailable(Gamepad)){
		float gamepadX = GetGamepadAxisMovement(0, 2);
        float gamepadY = GetGamepadAxisMovement(0, 3);

		if (gamepadX == 0){
			gamepadX = _PlayerShip.LastGamepadX;
		}
		else{
			_PlayerShip.LastGamepadX = gamepadX;
		}

		if (gamepadY == 0){
			gamepadY = _PlayerShip.LastGamepadY;
		}
		else{
			_PlayerShip.LastGamepadY = gamepadY;
		}

        // Adjust the mouse cursor position based on gamepad input
        mousePosition.x += gamepadX * mouseXSens; // Adjust the sensitivity as needed
        mousePosition.y += gamepadY * mouseYSens;

        // Make sure the mouse cursor doesn't go outside the window
        mousePosition.x = _PlayerShip.playerpos.x + (MouseRadius * cos(atan2(gamepadY, gamepadX)));
        mousePosition.y = _PlayerShip.playerpos.y + (MouseRadius * sin(atan2(gamepadY, gamepadX)));

        // Set the new mouse cursor position
        SetMousePosition((int)mousePosition.x, (int)mousePosition.y);
	}
}

//Menu Function called every Frame when GameState = 0
void Menu(Map&_Map, PlayerShip&_PlayerShip){
	float Current_Time = GetTime(); //Getting Current Time
	
	SetMousePositionUsingGamepad(_PlayerShip); //Set mouse position through joystick

	ProjectileSpawnCheck(_PlayerShip, Current_Time); //Call ProjectileSpawnCheck Function to check if projectile has to be spawned

	ProjectileUpdate(Current_Time); //Calling ProjectileUpdate Function to update projectile's velocity
	
	BeginDrawing();
	ClearBackground(BLACK);

	DrawTexturePro(_Map.Background, screen, screen, {0, 0}, 0, WHITE); //Draw background texture
	
	DrawTexturePro(_PlayerShip.PlayerShipTexture, _PlayerShip.ShipRectangle, { _PlayerShip.playerpos.x, _PlayerShip.playerpos.y, _PlayerShip.ShipRectangle.width, _PlayerShip.ShipRectangle.height },
	 _PlayerShip.ShipCenter, 180 + (atan2(GetMouseX() - _PlayerShip.playerpos.x, GetMouseY() - _PlayerShip.playerpos.y) * -57.29578f), WHITE); //Draw Player's ship texture

	DrawText("SPACE SHOOTER", (screen.width/2)-200, (screen.height/2)-50, 50, WHITE); //Draw "SPACE SHOOTER" text
	
	DrawText("HIGH SCORE - ", (screen.width/2)-140, (screen.height/2), 25, WHITE); //Draw "HIGH SCORE - " text
	DrawText(to_string(HighScore).c_str(), (screen.width/2)+50, (screen.height/2), 25, BLUE); //Draw HighScore text which is converted from string to int using to_string(HighScore).c_str()
	
	//DrawRectangle((screen.width/2)-200, (screen.height/2)+50, 135, 50, RED); (Hitbox of "PLAY" button)
	DrawText("PLAY", (screen.width/2)-200, (screen.height/2)+50, 50, WHITE); //Draw "PLAY" text

	//DrawRectangle((screen.width/2)+115, (screen.height/2)+50, 125, 50, RED); (Hitbox of "QUIT" button)
	DrawText("QUIT", (screen.width/2)+115, (screen.height/2)+50, 50, WHITE); //Draw "QUIT" text

	if (IsGamepadAvailable(Gamepad)){
		DrawText("Move - Left Joystick", (screen.width)-300, 25, 25, RED); //Draw "Move - W/A/S/D" text
		DrawText("Shoot - L1", (screen.width)-300, 50, 25, RED); //Draw "Shoot - C" text
		DrawText("Aim - Right Joystick", (screen.width)-300, 75, 25, RED); //Draw "Aim - Mouse" text
	}
	else{
		DrawText("Move - W/A/S/D", (screen.width)-300, 25, 25, RED); //Draw "Move - W/A/S/D" text
		DrawText("Shoot - C", (screen.width)-300, 50, 25, RED); //Draw "Shoot - C" text
		DrawText("Aim - Mouse", (screen.width)-300, 75, 25, RED); //Draw "Aim - Mouse" text
	}

	//Draw projectile texture
	for (int i = 0; i < projectiles.size(); i++) {
		Projectile& projectile = projectiles[i];
		DrawTexturePro(projectile.ProjectileTexture, projectile.ProjectileRectangle, { projectile.position.x, projectile.position.y, projectile.ProjectileRectangle.width, projectile.ProjectileRectangle.height },
		 projectile.ProjectileCenter, 90 + (atan2(projectile.mouseposition.x - _PlayerShip.playerpos.x, projectile.mouseposition.y - _PlayerShip.playerpos.y) * -57.29578f), WHITE);
	}
	
	//Collision checker for projectile hitbox and "PLAY" and "QUIT" hitbox
	for (int i = 0; i < projectiles.size(); i++) {
		if (CheckCollisionRecs({projectiles[i].position.x, projectiles[i].position.y, projectiles[i].ProjectileRectangle.width, projectiles[i].ProjectileRectangle.height}, {(screen.width/2)-200,
		 (screen.height/2)+50, 135, 50})) { //Check if Projectile hit "PLAY"
			projectiles.erase(projectiles.begin() + i); //Delete projectile
			GameState = 1; //Change Game State to 1 (Play Game State)
		}
		if (CheckCollisionRecs({projectiles[i].position.x, projectiles[i].position.y, projectiles[i].ProjectileRectangle.width, projectiles[i].ProjectileRectangle.height}, {(screen.width/2)+115,
		 (screen.height/2)+50, 125, 50})) { //Check if Projectile hit "QUIT"
			projectiles.erase(projectiles.begin() + i); //Delete Projectile
			EndDrawing();

			UnloadSound(LaserPewSound);
			UnloadSound(SpaceBGM);
			UnloadSound(MeteorExplode);
			UnloadSound(EarthHit);
			UnloadSound(GameOver);

    		CloseAudioDevice();
			CloseWindow(); //Close Game
		}
	}
}

//PlayGame Function called every Frame when Game State = 1
void PlayGame(Earth&_Earth, PlayerShip&_PlayerShip, Map&_Map){
	GameOverCheck(_Earth, _PlayerShip); //Call GameOverCheck every Frame

	float Current_Time = GetTime(); //Get Current Time

	_Earth.HPPercent = _Earth.CurrentHP / _Earth.MaxEarthHP; //Calculate Ratio of CurrentHP to MaxHP

	SetMousePositionUsingGamepad(_PlayerShip); //Set mouse position through joystick

	PlayerMovement(_PlayerShip); //Call PlayerMovement Function to handle player's movement
	ProjectileSpawnCheck(_PlayerShip, Current_Time); //Call ProjectileSpawnCheck Function to check if projectile has to be spawned

	ProjectileUpdate(Current_Time); //Update Projectile's velocity
	MeteorUpdate(Current_Time); //Update Meteor's velocity and Spawn meteors

	CheckProjectileMeteorCollision(); //Call CheckProjectileMeteorCollision to check for collisions between projectile and meteor every frame
	CheckMeteorEarthCollision(_Earth);//Call CheckMeteorEarthCollision to check for collisions between Earth and meteor every frame

	BeginDrawing();
	ClearBackground(BLACK);

	DrawTextures(_Map, _Earth, _PlayerShip); //Draw Textures on screen
	DrawAnimatedTextures(); //Draw animated textures (Sprite sheet explosion animation) on screen

	DisplayScore(); //Display player's score
}

int main() {

	InitWindow(screen.width, screen.height, "SPACE SHOOTER"); //Create game window
	
	InitAudioDevice();

	LaserPewSound = LoadSound("/Users/harshit/Documents/RaylibProjects/Template-4.5.0/Assets/LaserPew.wav");
	SpaceBGM = LoadSound("/Users/harshit/Documents/RaylibProjects/Template-4.5.0/Assets/SpaceBGM.wav");
	MeteorExplode = LoadSound("/Users/harshit/Documents/RaylibProjects/Template-4.5.0/Assets/MeteorExplode.wav");
	EarthHit = LoadSound("/Users/harshit/Documents/RaylibProjects/Template-4.5.0/Assets/EarthHit.ogg");
	GameOver = LoadSound("/Users/harshit/Documents/RaylibProjects/Template-4.5.0/Assets/GameOver.wav");

	SetTargetFPS(60); //Setting Target FPS
	//HideCursor();

	PlayerShip Game_PlayerShip; //Creating PlayerShip object
	Map _Map; //Creating Map object
	Earth Game_Earth; //Creating Earth object

	PlayerShip Menu_PlayerShip; //Creating PlayerShip object for Menu

	while (!WindowShouldClose()) { //Loop until game window does not close

		if (!IsSoundPlaying(SpaceBGM)) {
            PlaySound(SpaceBGM);
        }

		if (GameState==0){ //If game state is on Menu
			Menu(_Map, Menu_PlayerShip); //Call Menu Function every frame
		}
		else if (GameState==1){//If game state is on PlayGame
			if (Game_PlayerShip.LastGamepadX == 0){
				Game_PlayerShip.LastGamepadX = Menu_PlayerShip.LastGamepadX;
			}
			if (Game_PlayerShip.LastGamepadY == 0){
				Game_PlayerShip.LastGamepadY = Menu_PlayerShip.LastGamepadY;
			}
			PlayGame(Game_Earth, Game_PlayerShip, _Map); //Call PlayGame Function every frame
		}
	
		DrawFPS(10, 10); //Display FPS

		EndDrawing();

	}

	UnloadSound(LaserPewSound);
	UnloadSound(SpaceBGM);
	UnloadSound(MeteorExplode);
	UnloadSound(EarthHit);
	UnloadSound(GameOver);

    CloseAudioDevice();
	CloseWindow();

	return 0;
}