#include "Game.h"

Game::Game(unsigned int width, unsigned int height)
	:windowWidth(width), windowHeight(height),
	chunksManager(std::time(nullptr), &lightingSystem)
{
	window.create(sf::VideoMode(windowWidth, windowHeight), "BAD TERRARIA CLONE");
	Init();
}	

Game::~Game()
{
	ImGui::SFML::Shutdown();
}

void Game::Init()
{
	//Initialize settings
	window.setFramerateLimit(0);
	camera.setSize(1920.0f, 1080.0f);
	ImGui::SFML::Init(window);

	//intialize textures
	if (!playerTex.loadFromFile(RESOURCES_PATH "playerSheet2.png"))
	{
		std::cout << "ERROR LOADING PLAYER TEX\n";
	}
	if (!zombieTex.loadFromFile(RESOURCES_PATH "zombieSheet.png"))
	{
		std::cout << "ERROR LOADING ZOMBIE TEX\n";
	}
	if (!bloodBatTex.loadFromFile(RESOURCES_PATH "BloodBatSpriteSheet.png"))
	{
		std::cout << "ERROR LOADING BLOOD BAT TEX\n";
	}
	if (!shortSwordTex.loadFromFile(RESOURCES_PATH "ShortSword.png"))
	{
		std::cout << "ERROR LOADING SHORT SWORD TEX\n";
	}
	if (!heartTex.loadFromFile(RESOURCES_PATH "heart_animated_2.png"))
	{
		std::cout << "ERROR LOADING HEART TEX\n";
	}

	//Create player
	playerEntity = entityFactory.createPlayer(playerSpawnPos, playerTex);
	
	//Initialize hotbar
	inventorySystem.addItem(entityManager, playerEntity, "ShortSword", 1, itemRegistry);
	inventorySystem.addItem(entityManager, playerEntity, "Torch", 64, itemRegistry);

	lastTime = gameClock.getElapsedTime().asSeconds();
}

void Game::Run()
{
	while (window.isOpen())
	{
		double currentTime = gameClock.getElapsedTime().asSeconds();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		ProcessEvents();
		Update();
		Render();
	}
}

void Game::ProcessEvents()
{
	sf::Event event;
	ImGuiIO& io = ImGui::GetIO();

	while (window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);

		if (event.type == sf::Event::Closed)
			window.close();

		auto& input = entityManager.getComponentStorage<InputComponent>().get(playerEntity);
		auto& inv = entityManager.getComponentStorage<InventoryComponent>().get(playerEntity);

		if (event.type == sf::Event::KeyPressed)
		{
			input.movement_keys[event.key.code] = true;

			if (event.key.code == sf::Keyboard::Num1)
				inv.activeHotbarSlot = 0;
			if (event.key.code == sf::Keyboard::Num2)
				inv.activeHotbarSlot = 1;
			if (event.key.code == sf::Keyboard::Num3)
				inv.activeHotbarSlot = 2;
			if (event.key.code == sf::Keyboard::Num4)
				inv.activeHotbarSlot = 3;
			if (event.key.code == sf::Keyboard::Num5)
				inv.activeHotbarSlot = 4;
			if (event.key.code == sf::Keyboard::Num6)
				inv.activeHotbarSlot = 5;
			if (event.key.code == sf::Keyboard::Num7)
				inv.activeHotbarSlot = 6;
			if (event.key.code == sf::Keyboard::Num8)
				inv.activeHotbarSlot = 7;
			if (event.key.code == sf::Keyboard::Num9)
				inv.activeHotbarSlot = 8;
			if (event.key.code == sf::Keyboard::Num0)
				inv.activeHotbarSlot = 9;

			if (event.key.code == sf::Keyboard::E)
			{
				renderInventory = !renderInventory;
				inv.selectedInventorySlot = -1;
			}
		}

		if (event.type == sf::Event::KeyReleased)
			input.movement_keys[event.key.code] = false;

		HandleMouseInput(event, io);
	}
}

void Game::HandleMouseInput(const sf::Event event, ImGuiIO& io)
{
	if (io.WantCaptureMouse)
		return;

	if (renderInventory)
		return;

	auto& inv = entityManager.getComponentStorage<InventoryComponent>().get(playerEntity);

	if (event.type == sf::Event::MouseButtonPressed)
	{
		sf::Vector2f pointWorldCoords = window.mapPixelToCoords
		({ event.mouseButton.x, event.mouseButton.y }, camera);

		if (event.mouseButton.button == sf::Mouse::Left)
		{
			ItemStack& item = inv.slots[inv.activeHotbarSlot];

			//Attack
			if (!itemRegistry[item.itemId].isBlock)
			{
				auto& weaponsStorage = entityManager.getComponentStorage<WeaponComponent>();
				
				if (weaponsStorage.has(playerEntity))
					weaponsStorage.get(playerEntity).attackRequested = true;
			}
			//Mine
			else
			{
				isMining = true;
				Tile::TileType tileRemoved = chunksManager.DestroyTile(pointWorldCoords);
				std::string itemId = Tile::tileTypeToItemId(tileRemoved);
				if (!itemId.empty())
				{
					soundManager.play(Sounds::Break);
					inventorySystem.addItem(entityManager, playerEntity, itemId, 1, itemRegistry);
				}
			}
		}

		//Place
		else if (event.mouseButton.button == sf::Mouse::Right)
		{
			ItemStack& item = inv.slots[inv.activeHotbarSlot];
			//Ensure item exists in current equipped slot
			if (itemRegistry.count(item.itemId) == 0)
				return;

			//Ensure item is block
			if (!itemRegistry[inv.slots[inv.activeHotbarSlot].itemId].isBlock)
				return;

			if (item.count > 0)
			{
				isPlacing = true;
				bool placed = chunksManager.PlaceTile(pointWorldCoords, tileRegistry[item.itemId].type, tileRegistry[item.itemId].isSolid);
				if (placed)
				{
					soundManager.play(Sounds::Place);
					inventorySystem.removeItem(entityManager, playerEntity, item.itemId, 1);
				}
			}
		}
	}

	if (event.type == sf::Event::MouseButtonReleased)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
			isMining = false;
		else if (event.mouseButton.button == sf::Mouse::Right)
			isPlacing = false;
	}

	//Drag mining/placing
	if (event.type == sf::Event::MouseMoved)
	{
		sf::Vector2f pointWorldCoords = window.mapPixelToCoords
		({ event.mouseMove.x, event.mouseMove.y }, camera);

		if (isMining)
		{
			Tile::TileType tileRemoved = chunksManager.DestroyTile(pointWorldCoords);
			std::string itemId = Tile::tileTypeToItemId(tileRemoved);
			if (!itemId.empty())
			{
				soundManager.play(Sounds::Break);
				inventorySystem.addItem(entityManager, playerEntity, itemId, 1, itemRegistry);
			}
		}
		if (isPlacing)
		{
			ItemStack& item = inv.slots[inv.activeHotbarSlot];
			if (item.count > 0 && itemRegistry.count(item.itemId) && itemRegistry[item.itemId].isBlock)
			{
				bool placed = chunksManager.PlaceTile(pointWorldCoords, tileRegistry[item.itemId].type, tileRegistry[item.itemId].isSolid);
				if (placed)
				{
					soundManager.play(Sounds::Place);
					inventorySystem.removeItem(entityManager, playerEntity, item.itemId, 1);
				}
			}
		}
	}
}

void Game::Update()
{
	ImGui::SFML::Update(window, deltaClock.restart());

	//Update systems

	aiSystem.update(entityManager, chunksManager, playerEntity, deltaTime);
	movementSystem.update(entityManager, soundManager, deltaTime);
	chunksManager.collisionsWithTerrain(entityManager.getComponentStorage<CollisionComponent>(), 
										entityManager.getComponentStorage<TransformComponent>(),
										entityManager.getComponentStorage<PhysicsComponent>());

	enemySpawnerSystem.update(entityManager, entityFactory, enemyTextures, chunksManager, playerEntity, deltaTime);
	dayNightSystem.update(entityManager, worldClock, deltaTime);
	combatSystem.update(entityManager, soundManager);
	healthSystem.update(entityManager, playerSpawnPos);
	renderSystem.update(entityManager, deltaTime);

	auto& playerTransform = entityManager.getComponentStorage<TransformComponent>().get(playerEntity);
	camera.setCenter(sf::Vector2f(playerTransform.position.x, playerTransform.position.y));


	//EQUIP / UNEQUIP WEAPONS LOGIC
	
	auto& inv = entityManager.getComponentStorage<InventoryComponent>().get(playerEntity);
	ItemStack& item = inv.slots[inv.activeHotbarSlot];
	auto& weaponsStorage = entityManager.getComponentStorage<WeaponComponent>();

	if (itemRegistry.count(item.itemId) == 0)
	{
		//empty slot, uneqip weapon
		if (weaponsStorage.has(playerEntity))
			entityManager.removeComponent<WeaponComponent>(playerEntity);

	}
	else if (itemRegistry[item.itemId].isBlock)
	{
		//uneqip weapon
		if (weaponsStorage.has(playerEntity))
			entityManager.removeComponent<WeaponComponent>(playerEntity);
	}
	else
	{
		//equips weapon because sword
		if (item.itemId == "ShortSword" && !weaponsStorage.has(playerEntity))
			entityManager.addComponent<WeaponComponent>(playerEntity, { "ShortSword", 10.0f, {}, 0.7f, 105.0f, false });
	}
}

void Game::Render()
{
	float t = worldClock.dayProgress();
	window.clear(lightingSystem.getSkyColor(t)); //Sky color based on time
	window.setView(camera);

	auto& transform = entityManager.getComponentStorage<TransformComponent>().get(playerEntity);

	lightingSystem.setLightingForTimeOfDay(t);
	chunksManager.UpdateAndRenderChunks(static_cast<float>(deltaTime), transform.position, t, window);
	renderSystem.draw(entityManager, window);

	window.setView(window.getDefaultView()); //set view mode to screen space before drawing ui
	uiSystem.renderUI(entityManager, playerEntity, heartTex, 
					renderInventory, itemRegistry, chunksManager, shortSwordTex,
					renderSystem, soundManager, isLighting, showAIDebugLines, masterVolume,
					window);
	window.setView(camera); //revert view mode to world space

	ImGui::SFML::Render(window);
	window.display();
}