#include "Skyroads.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

Skyroads::Skyroads()
{
}

Skyroads::~Skyroads()
{
}

void Skyroads::Init()
{
	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;

		Mesh* mesh1 = new Mesh("sphere");
		mesh1->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh1->GetMeshID()] = mesh1;
	}

	//Shader pt platforme
	{
		Shader* shader = new Shader("SkyroadsShaderPlatforme");
		shader->AddShader("Source/Laboratoare/Skyroads/Shaders/VertexShaderPlatforme.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Skyroads/Shaders/FragmentShaderPlatforme.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//Shader pt sfera
	{
		Shader* shader = new Shader("SkyroadsShaderSfera");
		shader->AddShader("Source/Laboratoare/Skyroads/Shaders/VertexShaderSfera.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Skyroads/Shaders/FragmentShaderSfera.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//Shader pt UI
	{
		Shader* shader = new Shader("SkyroadsShaderUI");
		shader->AddShader("Source/Laboratoare/Skyroads/Shaders/VertexShaderUI.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Skyroads/Shaders/FragmentShaderUI.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	camera = GetSceneCamera();

	ox[0] = 0.0f;
	oy = -3.0f;
	for (int i = 0; i < 5; i++)
		oz[i] = -6.0f;

	sphereOX = 0.0f;
	sphereOY = 0.5f;
	sphereOZ = 0.0f;
	
	sphereScale = 0.5f;

	firstPersonCam = 0;
	poateSari = 0;
	blocheazaMiscare = 0;
	deformeaza = 0;

	for (int i = 0; i < 50; i++)
		contactPlatforma[i] = 0;

	benzina = 1.0f;
	maxBenzina = 1.0f;
	minBenzina = 0.0f;

	vitezaCadere = 0.0f;
	vitezaCadereMax = 0.2f;
	vitezaCadereMin = -0.5f;

	speed = 5.0f;
	maxSpeed = 10.0f;
	minSpeed = 5.0f;

	duratiePutereViteza = 0.0f;
	duratieDeformare = 0.0f;

	srand(time(NULL));

	for (int i = 0; i < 50; i++)
	{
		//genereaza numere random pt lungimile initiale ale platformelor
		randLength[i] = ((float)rand() / RAND_MAX) * (3.0f - 1.0f) + 1.0f;

		//genereaza valoarea random pt puteri [0-20]
		randPutere[i] = rand() % 20;
	}

	//genereaza pozitiile fixe ale coloanelor
	for (int i = -2; i < 3; i++)
	{
		ox[i + 2] = (float)i;
	}

	//genereaza numere random pt pozitiile initiale ale platformelor pe axa oz
	for (int i = 5; i < 50; i++)
	{
		oz[i] = oz[i - 5] - randLength[i - 5] / 2 - (((float)rand() / RAND_MAX) * (3.0f - 1.0f) + 1.0f) - randLength[i] / 2;
	}

	//genereaza chenarul dreptunghiului contor de benzina
	Mesh* chenarContor = Object2D::CreateSquare("chenarContor", glm::vec3(0, 0, 0), 1, glm::vec3(0, 1, 0), false);
	AddMeshToList(chenarContor);

	//genereaza dreptunghiul contor de benzina
	Mesh* contor = Object2D::CreateSquare("contor", glm::vec3(0, 0, 0), 1, glm::vec3(0, 1, 0), true);
	AddMeshToList(contor);
}

void Skyroads::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);	
}

void Skyroads::Update(float deltaTimeSeconds)
{
	//camera sa fie de tip third person
	if (firstPersonCam == 0)
	{
		camera->SetPosition(glm::vec3(0, 1.6f, 2.5f));
		camera->Update();
	}
	//camera sa fie de tip first person
	else
	{
		camera->SetPosition(glm::vec3(0, 0.4f, -0.5f));
		camera->Update();
	}

	//limiteaza viteza de miscare
	if (speed > maxSpeed)
		speed = maxSpeed;
	else if (speed < minSpeed)
		speed = minSpeed;

	//limiteaza viteza de cadere
	if (vitezaCadere > vitezaCadereMax)
		vitezaCadere = vitezaCadereMax;
	else if (vitezaCadere < vitezaCadereMin)
		vitezaCadere = vitezaCadereMin;

	//aplica caderea/saritura; daca vitezaCadere este pozitiv atunci cade, altfel se ridica
	vitezaCadere += 0.15f * deltaTimeSeconds;
	oy += vitezaCadere;

	//limiteaza benzina
	if (benzina > maxBenzina)
		benzina = maxBenzina;

	//consuma niste benzina in functie de timp
	benzina -= 0.05f * deltaTimeSeconds;

	//cand esti mai jos de nivelul platformelor sa mori
	if (oy > 2.0f)
		benzina = 0.0f;

	//sa scada timpul setat pt cronometrarea puterii de viteza
	duratiePutereViteza -= deltaTimeSeconds;

	
	//limiteaza cronometrul pt puterea de viteza
	if (duratiePutereViteza <= 0.0f)
	{
		duratiePutereViteza = 0.0f;
	}
	else if (duratiePutereViteza <= 0.1f)
	{
		//sa puna viteza de dinainte de puterea de viteza cand s-a terminat puterea
		minSpeed = 5.0f;
		maxSpeed = 10.0f;
		speed = 5.0f;
	}
	else if (duratiePutereViteza > 0.1f)
	{
		//sa puna viteza maxima cand are puterea de viteza
		minSpeed = 15.0f;
		maxSpeed = 15.0f;
		speed = 15.0f;
	}

	//sa scada timpul setat pt cronometrarea deformarii
	duratieDeformare -= deltaTimeSeconds;

	if (duratieDeformare <= 0.0f)
	{
		//sa nu scada sub 0 cronometrul pt deformare
		duratieDeformare = 0.0f;

		//sa nu deformeze jucatorul cand nu e activ cronometrul pt deformare
		deformeaza = 0;
	}
	else
	{
		//sa deformeze jucatorul cand este activ cronometrul pt deformare
		deformeaza = 1;
	}

	for (int i = 0; i < 50; i++)
	{
		//deplaseaza platformele in jos daca ai benzina
		if (benzina > minBenzina)
		{
			oz[i] += speed * deltaTimeSeconds;
		}
		//mori cand nu mai ai benzina
		else
		{
			benzina = 0.0f;

			oz[i] += 0.0f;
			blocheazaMiscare = 1;

			//animatie moarte
			if (sphereScale > 0.0f)
				sphereScale -= 0.01f * deltaTimeSeconds;
		}

		//daca platforma a iesit din scena
		if (oz[i] > 2.0f)
		{
			//da o lungime aleatoare noua platformei
			randLength[i] = ((float)rand() / RAND_MAX) * (3.0f - 1.0f) + 1.0f;

			//muta platforma la ultima pozitie
			oz[i] = oz[(i + 50 - 5) % 50] - randLength[(i + 50 - 5) % 50] / 2 - (((float)rand() / RAND_MAX) * (3.0f - 0.5f) + 0.5f) - randLength[i] / 2;

			//da o putere aleatoare noua platformei
			randPutere[i] = rand() % 20;
		}
	}

	//calculeaza coordonatele minime si maxime ale coloanelor
	for (int i = 0; i < 5; i++)
	{
		minX[i] = ox[i] - 0.5f / 2;
		maxX[i] = ox[i] + 0.5f / 2;
	}

	//calculeaza coordonatele minime si maxime ale grosimii platformelor
	minY = oy - 0.1f / 2;
	maxY = oy + 0.1f / 2;

	//calculeaza coordonatele minime si maxime ale lungimii fiecarei platforme in parte
	/* se iau toate platformele pt ca o platforma de pe alta coloana nu trebuie neaparat
	sa aibe aceleasi coordonate min si max ca alta de pe alta coloana, centrul si lungimea
	lor sunt calculate arbitrar */
	for (int i = 0; i < 50; i++)
	{
		minZ[i] = oz[i] - randLength[i] / 2;
		maxZ[i] = oz[i] + randLength[i] / 2;
	}

	//verifica coliziunea pt toate platformele
	for (int i = 0; i < 50; i++)
	{
		float x = max(minX[i % 5], min(sphereOX, maxX[i % 5]));
		float y = max(minY, min(sphereOY, maxY));
		float z = max(minZ[i], min(sphereOZ, maxZ[i]));

		float distance = sqrt((x - sphereOX) * (x - sphereOX) + (y - sphereOY) * (y - sphereOY) + (z - sphereOZ) * (z - sphereOZ));

		//a avut loc coliziunea
		if (distance <= 0.5f / 2)
		{
			//ai facut contact cu platforma
			contactPlatforma[i] = 1;
			//nu te lasa sa cazi prin platforme
			vitezaCadere = 0.0f;
			oy = 0.2f;
			//poti sari din nou
			poateSari = 1;
		}
		else
		{
			//nu ai facut contact cu platforma
			contactPlatforma[i] = 0;
		}
	}

	for (int j = -2; j < 3; j++)
	{
		//da culoare albastra la platformele initiale
		if (contactPlatforma[j + 2] == 0)
			culoarePutere[j + 2] = glm::vec3(0, 0, 1);

		//cand atingi platformele fa-le mov
		else
			culoarePutere[j + 2] = glm::vec3(0.5f, 0, 0.5f);

		//genereaza platforma
		{
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(ox[j + 2], oy, oz[j + 2]));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.1f, 3.0f));
			RenderSimpleMesh(meshes["box"], shaders["SkyroadsShaderPlatforme"], modelMatrix, culoarePutere[j + 2]);
		}
	}

	//genereaza platformele
	for (int i = 1; i < 10; i++)
		for (int j = -2; j < 3; j++)
		{
			//da puteri aleatoare la platforme si coloreaza-le in functie de puteri
			if (contactPlatforma[i * 5 + j + 2] == 0)
			{
				//[0-3] (20%) = primesti benzina <VERDE>
				if (randPutere[i * 5 + j + 2] >= 0 && randPutere[i * 5 + j + 2] <= 3)
					culoarePutere[i * 5 + j + 2] = glm::vec3(0, 1, 0);
				//[4-5] (10%) = pierzi benzina <GALBEN>
				else if (randPutere[i * 5 + j + 2] >= 4 && randPutere[i * 5 + j + 2] <= 5)
					culoarePutere[i * 5 + j + 2] = glm::vec3(1, 1, 0);
				//[6-8] (15%) = mergi cu viteza <PORTOCALIU>
				else if (randPutere[i * 5 + j + 2] >= 6 && randPutere[i * 5 + j + 2] <= 8)
					culoarePutere[i * 5 + j + 2] = glm::vec3(1, 0.65f, 0);
				//[9] (5%) = mori <ROSU>
				else if (randPutere[i * 5 + j + 2] == 9)
					culoarePutere[i * 5 + j + 2] = glm::vec3(1, 0, 0);
				//[10-19] (50%) = normal <ALBASTRU>
				else
					culoarePutere[i * 5 + j + 2] = glm::vec3(0, 0, 1);
			}

			//cand atingi platforma fa-o mov si ia puterea
			else
			{
				culoarePutere[i * 5 + j + 2] = glm::vec3(0.5f, 0, 0.5f);

				if (randPutere[i * 5 + j + 2] >= 0 && randPutere[i * 5 + j + 2] <= 3)
				{
					benzina += 0.02f;

					duratieDeformare = 1.0f;
				}
				else if (randPutere[i * 5 + j + 2] >= 4 && randPutere[i * 5 + j + 2] <= 5)
				{
					benzina -= 0.02f;

					duratieDeformare = 1.0f;
				}
				else if (randPutere[i * 5 + j + 2] >= 6 && randPutere[i * 5 + j + 2] <= 8)
				{
					duratiePutereViteza = 5.1f;

					duratieDeformare = 1.0f;
				}
				else if (randPutere[i * 5 + j + 2] == 9)
				{
					benzina = 0.0f;

					duratieDeformare = 1.0f;
				}
			}

			//genereaza platforma
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(ox[j + 2], oy, oz[i * 5 + j + 2]));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.1f, randLength[i * 5 + j + 2]));
				RenderSimpleMesh(meshes["box"], shaders["SkyroadsShaderPlatforme"], modelMatrix, culoarePutere[i * 5 + j + 2]);
			}
		}

	//genereaza jucatorul
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(sphereOX, sphereOY, sphereOZ));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(sphereScale));
		RenderSimpleMesh(meshes["sphere"], shaders["SkyroadsShaderSfera"], modelMatrix);
	}

	//plaseaza chenarul contorului de benzina
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.9f, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.07f, 1.45f, 1));
		meshes["box"]->SetDrawMode(GL_LINES);
		RenderSimpleMesh(meshes["box"], shaders["SkyroadsShaderUI"], modelMatrix);
		meshes["box"]->SetDrawMode(GL_TRIANGLES);
	}

	//plaseaza contorul de benzina
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.9f, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05, benzina * 1.4f, 1));
		meshes["box"]->SetDrawMode(GL_TRIANGLES);
		RenderSimpleMesh(meshes["box"], shaders["SkyroadsShaderUI"], modelMatrix);
	}
}

void Skyroads::FrameEnd()
{
	DrawCoordinatSystem();
}

void Skyroads::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	int object_color = glGetUniformLocation(shader->program, "object_color");
	glUniform3f(object_color, color.r, color.g, color.b);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	int deformare = glGetUniformLocation(shader->program, "deformare");
	glUniform1i(deformare, deformeaza);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void Skyroads::OnInputUpdate(float deltaTime, int mods)
{
	if (blocheazaMiscare == 0)
	{
		if (window->KeyHold(GLFW_KEY_W))
			speed += 5.0f * deltaTime;
		else if (window->KeyHold(GLFW_KEY_S))
			speed -= 5.0f * deltaTime;

		if (window->KeyHold(GLFW_KEY_A))
			for (int i = 0; i < 5; i++)
				ox[i] += speed * deltaTime;
		else if (window->KeyHold(GLFW_KEY_D))
			for (int i = 0; i < 5; i++)
				ox[i] -= speed * deltaTime;
	}
}

void Skyroads::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_SPACE)
	{
		if (poateSari && blocheazaMiscare == 0)
		{
			poateSari = 0;

			if (vitezaCadere > 0)
				vitezaCadere = 0.0f;

			vitezaCadere -= 0.1f;
		}
	}

	if (key == GLFW_KEY_C)
		firstPersonCam = !firstPersonCam;
}

void Skyroads::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Skyroads::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Skyroads::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Skyroads::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Skyroads::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Skyroads::OnWindowResize(int width, int height)
{
}
