#pragma once
#include <Component/SimpleScene.h>
#include "Transform2D.h"
#include "Object2D.h"

class Skyroads : public SimpleScene
{
	public:
		Skyroads();
		~Skyroads();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1));

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		//pointer catre clasa Camera
		EngineComponents::Camera* camera;

		//coordonate carteziene pt translatie platforme
		float ox[5];
		float oy;
		float oz[50];

		//coordonate carteziene pt pozitia jucatorului
		float sphereOX;
		float sphereOY;
		float sphereOZ;

		//dimensiunea bilei jucator
		float sphereScale;

		float minX[5];
		float minY;
		float minZ[50];

		float maxX[5];
		float maxY;
		float maxZ[50];

		//viteza de deplasare
		float speed;
		float maxSpeed;
		float minSpeed;

		//viteza cadere
		float vitezaCadere;
		float vitezaCadereMax;
		float vitezaCadereMin;

		//benzina jucatorului
		float benzina;
		float maxBenzina;
		float minBenzina;

		//valoare random pt lungimea platformelor
		float randLength[50];

		//valoare random pt puterile platformelor
		int randPutere[50];

		//culorile platformelor in functie de ce putere au
		glm::vec3 culoarePutere[50];

		//contact cu platforma
		int contactPlatforma[50];

		//cat sa dureze puterea de viteza
		float duratiePutereViteza;

		//cat sa dureze deformarea
		float duratieDeformare;

		//poate sari
		int poateSari;

		//blocheaza controlul jucatorului
		int blocheazaMiscare;

		//foloseste camera de tip first person
		int firstPersonCam;

		int deformeaza;
};
