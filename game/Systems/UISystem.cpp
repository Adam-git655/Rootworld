#include "UISystem.h"

void UISystem::renderUI(EntityManager& mgr, Entt playerEntity, 
						sf::Texture& heartTexture, bool renderInv,
						std::unordered_map<std::string, ItemDef>& itemRegistry, ChunksManager& chunksManager, sf::Texture& shortSwordTex,
						RenderSystem& renderSystem, SoundManager& soundManager, bool& isLighting, bool& showAIDebugLines, float& masterVolume,
						sf::RenderWindow& window)
{
	renderHearts(mgr, playerEntity, heartTexture, window);
	renderHotbar(mgr, playerEntity, itemRegistry, chunksManager, shortSwordTex);
	renderInventory(mgr, playerEntity, renderInv, itemRegistry, chunksManager, shortSwordTex);
	renderSettings(chunksManager, renderSystem, soundManager, isLighting, showAIDebugLines, masterVolume);
}

void UISystem::renderHearts(EntityManager& mgr, Entt playerEntity, sf::Texture& heartTexture, sf::RenderWindow& window) const
{
	auto& health = mgr.getComponent<HealthComponent>(playerEntity);
	int totalHearts = health.maxHealth / 10;
	float playerHp = health.health;

	sf::Sprite heartSprite(heartTexture);
	heartSprite.setScale(2, 2);

	for (int i = 0; i < 10; ++i)
	{
		float currentHeartHealth = std::clamp(playerHp - (i * 10.0f), 0.0f, 10.0f);
		float healthRatioInHeart = currentHeartHealth / 10.0f;

		int frame;
		if (healthRatioInHeart > 0.75f)
			frame = 0;
		else if (healthRatioInHeart > 0.5f)
			frame = 1;
		else if (healthRatioInHeart > 0.25f)
			frame = 2;
		else if (healthRatioInHeart > 0.0f)
			frame = 3;
		else
			frame = 4;

		heartSprite.setTextureRect({ frame * heartWidth, 0, heartWidth, heartHeight });
		heartSprite.setPosition(healthBarPosX + i * ((heartWidth * heartSprite.getScale().x) + 4.0f), healthBarPosY);
		window.draw(heartSprite);
	}

}

void UISystem::renderHotbar(EntityManager& mgr, Entt playerEntity, std::unordered_map<std::string, ItemDef>& itemRegistry, ChunksManager& chunksManager, sf::Texture& shortSwordTex)
{
	auto& inv = mgr.getComponentStorage<InventoryComponent>().get(playerEntity);

	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	//set position to top left
	ImVec2 window_pos = ImVec2(viewport->WorkPos.x + 10, viewport->WorkPos.y + 10);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);

	const float SLOT_SIZE = 50.0f;

	ImGui::Begin("Hotbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
	for (int i = 0; i < inv.HOTBAR_SIZE; i++)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

		ItemStack& item = inv.slots[i];

		//draw empty slots
		if (item.count == 0)
		{
			std::string buttonId = "##btn" + std::to_string(i);
			if (ImGui::Button(buttonId.c_str(), ImVec2(SLOT_SIZE, SLOT_SIZE))) //normal button
				inv.activeHotbarSlot = i;
		}
		else
		{
			//if not empty, get texture of item in slot
			const sf::Texture* tex = nullptr;

			if (itemRegistry[item.itemId].isBlock)
				tex = &chunksManager.getTexture(item.itemId);
			else
				tex = &shortSwordTex;

			std::string buttonId = "##btn" + std::to_string(i);
			if (tex)
			{
				//draw texture through image button
				ImGui::PushID(i);
				if (ImGui::ImageButton((void*)(intptr_t)tex->getNativeHandle(), ImVec2(SLOT_SIZE, SLOT_SIZE), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
					inv.activeHotbarSlot = i;
				ImGui::PopID();
			}
			else
			{
				//fallback, wont happen likely
				if (ImGui::Button(buttonId.c_str(), ImVec2(SLOT_SIZE, SLOT_SIZE)))
					inv.activeHotbarSlot = i;
			}
		}

		ImGui::PopStyleVar();

		//add boundary rect when hovered
		ImVec2 rectMin = ImGui::GetItemRectMin();
		ImVec2 rectMax = ImGui::GetItemRectMax();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		if (i == inv.activeHotbarSlot)
			drawList->AddRect(rectMin, rectMax, IM_COL32(255, 255, 0, 255), 0.0f, 0, 2.0f);
		else if (ImGui::IsItemHovered())
			drawList->AddRect(rectMin, rectMax, IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);

		//add slot number on top left
		std::string numStr = std::to_string(i + 1);
		if (numStr == "10")
			numStr = "0";

		ImVec2 textPos = ImVec2(rectMin.x + 3, rectMin.y + 3);
		//Shadow
		drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), numStr.c_str());
		//White text on top
		drawList->AddText(ImVec2(textPos), IM_COL32(255, 255, 255, 255), numStr.c_str());

		//add item count on bottom right
		if (item.count > 1)
		{
			std::string countStr = std::to_string(item.count);
			ImVec2 textSize = ImGui::CalcTextSize(countStr.c_str());
			textPos = ImVec2(rectMax.x - textSize.x - 5, rectMax.y - textSize.y - 3);

			//Shadow
			drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), countStr.c_str());
			//White text on top
			drawList->AddText(ImVec2(textPos), IM_COL32(255, 255, 255, 255), countStr.c_str());
		}

		//ensure same line
		if (i < inv.HOTBAR_SIZE - 1)
			ImGui::SameLine();
	}

	ImGui::End();
}

void UISystem::renderInventory(EntityManager& mgr, Entt playerEntity, bool renderInv, std::unordered_map<std::string, ItemDef>& itemRegistry, ChunksManager& chunksManager, sf::Texture& shortSwordTex)
{
	if (!renderInv)
		return;

	auto& inv = mgr.getComponentStorage<InventoryComponent>().get(playerEntity);

	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	//set position to middle of screen
	ImVec2 window_pos = ImVec2(
		viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
		viewport->WorkPos.y + viewport->WorkSize.y * 0.5f
	);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	ImGui::Begin("Inventory", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);

	const int COLS = 10;
	const float SLOT_SIZE = 50.0f;

	for (int i = 0; i < inv.INVENTORY_SIZE; ++i)
	{
		int slotIndex = inv.HOTBAR_SIZE + i;
		ItemStack& item = inv.slots[slotIndex];

		int col = i % COLS;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

		if (item.count == 0)
		{
			std::string buttonId = "##btn" + std::to_string(slotIndex);

			if (ImGui::Button(buttonId.c_str(), ImVec2(SLOT_SIZE, SLOT_SIZE)))
			{
				if (inv.selectedInventorySlot != -1)
				{
					//move item from old slot to empty slot
					std::swap(inv.slots[inv.selectedInventorySlot], item);
					inv.selectedInventorySlot = -1;
				}
			}
		}
		else
		{
			const sf::Texture* tex = nullptr;

			if (itemRegistry[item.itemId].isBlock)
				tex = &chunksManager.getTexture(item.itemId);
			else
				tex = &shortSwordTex;

			std::string buttonId = "##btn" + std::to_string(slotIndex);
			if (tex)
			{
				ImGui::PushID(slotIndex);
				if (ImGui::ImageButton((void*)(intptr_t)tex->getNativeHandle(), ImVec2(SLOT_SIZE, SLOT_SIZE), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
				{
					if (inv.selectedInventorySlot == -1)
					{
						//select slot first time
						if (item.count > 0)
							inv.selectedInventorySlot = slotIndex;
					}
					else
					{
						//swap item from old slot with new slot
						std::swap(inv.slots[inv.selectedInventorySlot], item);
						inv.selectedInventorySlot = -1;
					}
				}
				ImGui::PopID();
			}
			else
			{
				ImGui::Button(buttonId.c_str(), ImVec2(SLOT_SIZE, SLOT_SIZE));
			}
		}

		ImGui::PopStyleVar();

		ImVec2 rectMin = ImGui::GetItemRectMin();
		ImVec2 rectMax = ImGui::GetItemRectMax();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		if (slotIndex == inv.selectedInventorySlot)
			drawList->AddRect(rectMin, rectMax, IM_COL32(0, 255, 0, 255), 0.0f, 0, 2.0f);
		if (ImGui::IsItemHovered())
			drawList->AddRect(rectMin, rectMax, IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);

		if (item.count > 1)
		{
			std::string countStr = std::to_string(item.count);
			ImVec2 textSize = ImGui::CalcTextSize(countStr.c_str());
			ImVec2 textPos = ImVec2(rectMax.x - textSize.x - 5, rectMax.y - textSize.y - 3);

			//Shadow
			drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), countStr.c_str());
			//White text on top
			drawList->AddText(ImVec2(textPos), IM_COL32(255, 255, 255, 255), countStr.c_str());
		}

		if (col < COLS - 1)
			ImGui::SameLine();
	}

	ImGui::End();


	//Draw another hotbar above inventory with some gap

	ImVec2 hotbar_win_pos = ImVec2(
		viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
		(viewport->WorkPos.y + viewport->WorkSize.y * 0.5f) - (2 * 50.0f + 50.0f)
	);
	ImGui::SetNextWindowPos(hotbar_win_pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	ImGui::Begin("Hotbar-Inv", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);

	for (int i = 0; i < inv.HOTBAR_SIZE; i++)
	{
		ItemStack& item = inv.slots[i];

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

		if (item.count == 0)
		{
			std::string buttonId = "##btn" + std::to_string(i);
			if (ImGui::Button(buttonId.c_str(), ImVec2(SLOT_SIZE, SLOT_SIZE))) //normal button
			{
				if (inv.selectedInventorySlot != -1)
				{
					std::swap(inv.slots[inv.selectedInventorySlot], item);
					inv.selectedInventorySlot = -1;
				}
			}
		}
		else
		{
			const sf::Texture* tex = nullptr;

			if (itemRegistry[item.itemId].isBlock)
				tex = &chunksManager.getTexture(item.itemId);
			else
				tex = &shortSwordTex;

			std::string buttonId = "##btn" + std::to_string(i);
			if (tex)
			{
				ImGui::PushID(i);
				if (ImGui::ImageButton((void*)(intptr_t)tex->getNativeHandle(), ImVec2(SLOT_SIZE, SLOT_SIZE), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
				{
					if (inv.selectedInventorySlot == -1)
					{
						if (item.count > 0)
							inv.selectedInventorySlot = i;
					}
					else
					{
						std::swap(inv.slots[inv.selectedInventorySlot], item);
						inv.selectedInventorySlot = -1;
					}
				}
				ImGui::PopID();
			}
			else
			{
				ImGui::Button(buttonId.c_str(), ImVec2(SLOT_SIZE, SLOT_SIZE));
			}
		}

		ImGui::PopStyleVar();

		ImVec2 rectMin = ImGui::GetItemRectMin();
		ImVec2 rectMax = ImGui::GetItemRectMax();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		if (i == inv.selectedInventorySlot)
			drawList->AddRect(rectMin, rectMax, IM_COL32(0, 255, 0, 255), 0.0f, 0, 2.0f);
		if (ImGui::IsItemHovered())
			drawList->AddRect(rectMin, rectMax, IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);

		//add slot number on top left
		std::string numStr = std::to_string(i + 1);
		if (numStr == "10")
			numStr = "0";

		ImVec2 textPos = ImVec2(rectMin.x + 3, rectMin.y + 3);
		//Shadow
		drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), numStr.c_str());
		//White text on top
		drawList->AddText(ImVec2(textPos), IM_COL32(255, 255, 255, 255), numStr.c_str());

		//add item count on bottom right
		if (item.count > 1)
		{
			std::string countStr = std::to_string(item.count);
			ImVec2 textSize = ImGui::CalcTextSize(countStr.c_str());
			textPos = ImVec2(rectMax.x - textSize.x - 5, rectMax.y - textSize.y - 3);

			drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), countStr.c_str());
			drawList->AddText(ImVec2(textPos), IM_COL32(255, 255, 255, 255), countStr.c_str());
		}

		//ensure same line
		if (i < inv.HOTBAR_SIZE - 1)
			ImGui::SameLine();
	}

	ImGui::End();
}

void UISystem::renderSettings(ChunksManager& chunksManager, RenderSystem& renderSystem, SoundManager& soundManager, bool& isLighting, bool& showAIDebugLines, float& masterVolume)
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImVec2 window_pos = ImVec2(
		viewport->WorkPos.x + viewport->WorkSize.x - 10.0f,  // 10 px from right
		viewport->WorkPos.y + 10.0f                          // 10 px from top
	);
	ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);  // Pivot: right-top corner

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::Checkbox("Lighting", &isLighting))
	{
		if (isLighting)
			chunksManager.EnableLighting();
		else
			chunksManager.DisableLighting();
	}

	if (ImGui::Checkbox("Show Pathfinding lines", &showAIDebugLines))
	{
		renderSystem.showAIDebugLines = showAIDebugLines;
	}

	if (ImGui::SliderFloat("Master Volume", &masterVolume, 0.0f, 100.0f))
	{
		soundManager.setMasterVolume(masterVolume);
	}

	ImGui::End();
}