#include "Scene.h"

using namespace std;

Scene::Scene(OpenGLClass* OpenGLObject, HWND hwnd) {
	handlerWindow = hwnd;
	OpenGL = OpenGLObject;
	Camera = 0;
	LightShader = 0;
	ColorShader = 0;
	ShaderBounding = 0;
	Triangulo = 0;
	Light = 0;
	speedAxisX = 0.0f;
	speedAxisY = 0.0f;
	speed = 0.0f;
	angulo = 0.0f;
	Object3d = 0;
	LoaderTexture = new TextureClass(OpenGL);
}

Scene::~Scene() {

}

bool Scene::Initialize() {
	bool result = true;
	string resultDetail = "";

	angulo = 0.0f;
	speedAxisX = SPEED_AXIS_X;
	speedAxisY = SPEED_AXIS_Y;
	speed = SPEED_CAMERA;

	// Crea un objeto camara.
	Camera = new CameraClass;
	if (!Camera) {
		result = false;
		_RPT1(0, "Alert! Camera has not been initialized. \n", 0);
		return result;
	}
	else {
		// Asigna las coordenadas de posicion y rotacion por defecto a los arreglos delta.
		DeltaPosition->X = InitialPosition[0]; DeltaPosition->Y = InitialPosition[1]; DeltaPosition->Z = InitialPosition[2];
		DeltaRotation->X = InitialRotation[0]; DeltaRotation->Y = InitialRotation[1]; DeltaRotation->Z = InitialRotation[2];
		// Luego se los pasamos a la camara.
		Camera->SetPosition(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z);
		Camera->SetRotation(DeltaRotation->X, DeltaRotation->Y, DeltaRotation->Z);
		// Con esto podriamos aislar siempre el punto de partida inicial de la escena para evitar teletrasportarse sin querer.
	}

	LightShader = new LightShaderClass(OpenGL, handlerWindow, "shaders/terrain.vs", "shaders/terrain.ps");
	if (!LightShader) {
		result = false;
		MessageBoxA(handlerWindow, "Could not declared and initialized the light shader object.", "Error", MB_OK);
		_RPT1(0, "Alert! LightShader has an error on declare and not been initialized. \n", 0);
		return result;
	}

	ColorShader = new ColorShaderClass(OpenGL, handlerWindow, "shaders/color.vs", "shaders/color.ps");
	if (!ColorShader) {
		result = false;
		MessageBoxA(handlerWindow, "Could not declared and initialized the color shader object.", "Error", MB_OK);
		_RPT1(0, "Alert! ColorShader has an error on declare and not been initialized. \n", 0);
		return result;
	}

	//Terreno = new Land(handlerWindow, OpenGL, LoaderTexture, L"recursos/heightmap.jpg", "recursos/test.tga", "recursos/Piedras_normal.jpg",(float)400, (float)400, 0, 1);
	Terreno = new Land(handlerWindow, OpenGL, LoaderTexture, L"recursos/suelo/heightmap2.jpg", "recursos/suelo/suelo_height.png", "recursos/suelo/suelo.jpg", "recursos/suelo/suelo_normal.jpg", (float)300, (float)300);
	if (!Terreno){
		result = false;
		_RPT1(0, "Alert! Land has not been initialized. \n", 0);
		return result;
	}
	else {
		Terreno->SetLandShader(LightShader);
	}

   Agua = new Water(handlerWindow, OpenGL, LoaderTexture, L"recursos/Agua/heightmap2.png", "recursos/Agua/agua_height.png", "recursos/Agua/agua.jpg", "recursos/Agua/agua_normal.jpg", (float)300, (float)300);
	if (!Agua) {
		result = false;
		_RPT1(0, "Alert! Land has not been initialized. \n", 0);
		return result;
	}
	else {
		Agua->SetLandShader(LightShader);
	}
	/*Triangulo = new TriangleClass(OpenGL);
	if (!Triangulo) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the triangle model object.", "Error", MB_OK);
		_RPT1(0, "Alert! Triangle has not been initialized. \n", 0);
		return result;
	}*/
	/*else {
		Triangulo->SetShaderTriangle(ColorShader);
	}*/

	ShaderModel = new ShaderTexture(OpenGL, handlerWindow, "shaders/light.vs", "shaders/light.ps");
	if (!ShaderModel) {
		result = false;
		MessageBoxA(handlerWindow, "Could not declared and initialized the shader model object.", "Error", MB_OK);
		_RPT1(0, "Alert! ShaderModel has an error on declare and not been initialized. \n", 0);
		return result;
	}

	ShaderBounding = new ShaderBoundingBox(OpenGL, handlerWindow, "shaders/vertexcolor.vs", "shaders/vertexcolor.ps");
	if (!ShaderBounding) {
		result = false;
		MessageBoxA(handlerWindow, "Could not declared and initialized the shader model object.", "Error", MB_OK);
		_RPT1(0, "Alert! ShaderModel has an error on declare and not been initialized. \n", 0);
		return result;
	}
	// Skydome
	ShaderSky = new SkydomeShaderClass(OpenGL, handlerWindow, "shaders/SkydomeShader.vs", "shaders/SkydomeShader.ps");
	if (!ShaderSky) {
		result = false;
		MessageBoxA(handlerWindow, "Could not declared and initialized the shader skydome.", "Error", MB_OK);
		_RPT1(0, "Alert! ShaderModel has an error on declare and not been initialized. \n", 0);
		return result;
	}

	Skydome = new Dome("recursos/sky3.jpg", OpenGL, LoaderTexture, 500);
	if (!Skydome) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the Skydome.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Skydome->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the Skydome.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Skydome->SetIterpolationDay(false, true);
		Skydome->SetShader(ShaderSky);
	}
	//Modelos

	EdificioP = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/EdificioP/EdificioP2.obj",
		"recursos/Texturas/TexturaEdificio.png");
	if (!EdificioP) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result =EdificioP->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		EdificioP->SetShaders(ShaderModel, ShaderBounding);
	}

	EdificioT = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/EdificioT/EdificioT2.obj",
		"recursos/Texturas/Piso.jpg");
	if (!EdificioT) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = EdificioT->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		EdificioT->SetShaders(ShaderModel, ShaderBounding);
	}

	Tren = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Tren/Tren3.obj",
		"recursos/Texturas/TexturaTren.png");
	if (!Tren) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Tren->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Tren->SetShaders(ShaderModel, ShaderBounding);
	}
	Cancha = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Cancha/Cancha1.obj",
		"recursos/Texturas/cancha.png");
	if (!Cancha) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Cancha->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Cancha->SetShaders(ShaderModel, ShaderBounding);
	}
	Rocas = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Rocas/Rocas1.obj",
		"recursos/Texturas/rocas.jpg");
	if (!Rocas) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Rocas->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Rocas->SetShaders(ShaderModel, ShaderBounding);
	}
	Oso = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Oso/Oso.obj",
		"recursos/Texturas/Oso.jpg");
	if (!Oso) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result =Oso->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Oso->SetShaders(ShaderModel, ShaderBounding);
	}

	Caja = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Caja/Caja.obj",
		"recursos/Texturas/caja1.jpg");
	if (!Caja) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Caja->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Caja->SetShaders(ShaderModel, ShaderBounding);
	}

	Tronco= new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Arbol/tronco.obj",
		"recursos/Texturas/tronco1.jpg");
	if (!Tronco) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Tronco->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Tronco->SetShaders(ShaderModel, ShaderBounding);
	}


	Tronco2 = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Arbol/Tronco2.obj",
		"recursos/Texturas/Tronco2.jpg");

	if (!Tronco2) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject1.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Tronco2->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject2.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Tronco2->SetShaders(ShaderModel, ShaderBounding);
	}
	
	Vagon = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Vagon/Vagon.obj",
		"recursos/Texturas/Vagon.jpg");

	if (!Vagon) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject1.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Vagon->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject2.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Vagon->SetShaders(ShaderModel, ShaderBounding);
	}

	Tienda2 = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Tienda/Tienda2.obj",
		"recursos/Texturas/Tienda.jpg");

	if (!Tienda2) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject1.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Tienda2->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject2.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Tienda2->SetShaders(ShaderModel, ShaderBounding);
	}

	Pared1 = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Tienda/Pared1.obj",
		"recursos/Texturas/Pared11.jpg");

	if (!Pared1) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject1.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Pared1->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject2.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Pared1->SetShaders(ShaderModel, ShaderBounding);
	}

	Pared2 = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Tienda/Pared2.obj",
		"recursos/Texturas/Pared22.jpg");

	if (!Pared2) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject1.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Pared2->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject2.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Pared2->SetShaders(ShaderModel, ShaderBounding);
	}

	Pared3 = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Tienda/Pared3.obj",
		"recursos/Texturas/Pared33.jpg");

	if (!Pared3) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject1.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Pared3->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject2.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Pared3->SetShaders(ShaderModel, ShaderBounding);
	}

	Pared4 = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Tienda/Pared4.obj",
		"recursos/Texturas/Pared44.jpg");

	if (!Pared4) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject1.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Pared4->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject2.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Pared4->SetShaders(ShaderModel, ShaderBounding);
	}

	Pared5 = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Tienda/Pared5.obj",
		"recursos/Texturas/Pared55.jpg");

	if (!Pared5) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject1.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Pared5->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject2.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Pared5->SetShaders(ShaderModel, ShaderBounding);
	}

	Pared6 = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Tienda/Pared6.obj",
		"recursos/Texturas/Pared66.jpg");

	if (!Pared6) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject1.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Pared6->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject2.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Pared6->SetShaders(ShaderModel, ShaderBounding);
	}

	Carro = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"recursos/modelos/Carro/carro.obj",
		"recursos/Texturas/Carro.tga");

	if (!Carro) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject1.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Carro->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject2.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Carro->SetShaders(ShaderModel, ShaderBounding);
	}

	// Collision Boxes
	box = new BoundingBoxRenderer(OpenGL,
		BoundingBox::GLFLOAT3{ -145.0f, 45.0f, 147.0f }, BoundingBox::GLFLOAT3{ -147.0f, -15.0f, -145.0f });
	if (!box) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the box.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		box->SetShader(ShaderBounding);
	}



	box2 = new BoundingBoxRenderer(OpenGL,
		BoundingBox::GLFLOAT3{ -147.0f, 45.0f, -145.0f }, BoundingBox::GLFLOAT3{ 145.0f, -15.0f, -147.0f });
	if (!box2) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the box.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		box2->SetShader(ShaderBounding);
	}

	box3 = new BoundingBoxRenderer(OpenGL,
		BoundingBox::GLFLOAT3{ 145.0f,45.0f, -147.0f }, BoundingBox::GLFLOAT3{ 147.0f, -15.0f, 145.0f });
	if (!box3) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the box.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		box3->SetShader(ShaderBounding);
	}
	box4 = new BoundingBoxRenderer(OpenGL,
		BoundingBox::GLFLOAT3{ 147.0f, 45.0f, 145.0f }, BoundingBox::GLFLOAT3{ -145.0f, -15.0f, 147.0f });
	if (!box4) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the box.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}

	else {
		box4->SetShader(ShaderBounding);
	}

	box5 = new BoundingBoxRenderer(OpenGL,
		BoundingBox::GLFLOAT3{ -71.0f, 45.0f, -33.0f }, BoundingBox::GLFLOAT3{ -137.0f, -15.0f, -35.0f });
	if (!box5) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the box.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}

	else {
		box5->SetShader(ShaderBounding);
	}

	box6 = new BoundingBoxRenderer(OpenGL,
		BoundingBox::GLFLOAT3{ -138.0f, 45.0f, -34.0f }, BoundingBox::GLFLOAT3{ -136.0f, -15.0f, 60.0f });
	if (!box6) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the box.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}

	else {
		box6->SetShader(ShaderBounding);
	}

	box7 = new BoundingBoxRenderer(OpenGL,
		BoundingBox::GLFLOAT3{ -137.0f, 45.0f, 59.0f }, BoundingBox::GLFLOAT3{ -71.0f, -15.0f, 61.0f });
	if (!box7) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the box.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}

	else {
		box7->SetShader(ShaderBounding);
	}

	box8 = new BoundingBoxRenderer(OpenGL,
		BoundingBox::GLFLOAT3{ -70.0f, 45.0f, -8.0f }, BoundingBox::GLFLOAT3{ -72.0f, -15.0f, -13.0f });
	if (!box8) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the box.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}

	else {
		box8->SetShader(ShaderBounding);
	}

	box9 = new BoundingBoxRenderer(OpenGL,
		BoundingBox::GLFLOAT3{ -70.0f, 45.0f, -30.0f }, BoundingBox::GLFLOAT3{ -72.0f, -15.0f, -33.0f });
	if (!box9) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the box.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}

	else {
		box9->SetShader(ShaderBounding);
	}

	box10 = new BoundingBoxRenderer(OpenGL,
		BoundingBox::GLFLOAT3{ -71.0f, 45.0f, -31.0f }, BoundingBox::GLFLOAT3{ -138.0f, -15.0f, -32.0f });
	if (!box10) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the box.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}

	else {
		box10->SetShader(ShaderBounding);
	}

	box11 = new BoundingBoxRenderer(OpenGL,
		BoundingBox::GLFLOAT3{ -138.0f, 45.0f, -33.0f }, BoundingBox::GLFLOAT3{ -138.0f, -15.0f, 60.0f });
	if (!box11) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the box.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}

	else {
		box11->SetShader(ShaderBounding);
	}

	box12 = new BoundingBoxRenderer(OpenGL,
		BoundingBox::GLFLOAT3{ -138.0f, 45.0f, 60.0f }, BoundingBox::GLFLOAT3{ -71.0f, -15.0f, 60.0f });
	if (!box12) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the box.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}

	else {
		box12->SetShader(ShaderBounding);
	}

	// Billboards
	ShaderBill = new BillboardShaderClass(OpenGL, handlerWindow, "shaders/billboard.vs", "shaders/billboard.ps");
	if (!ShaderBill) {
		result = false;
		MessageBoxA(handlerWindow, "Could not declared and initialized the shader billboard.", "Error", MB_OK);
		_RPT1(0, "Alert! ShaderModel has an error on declare and not been initialized. \n", 0);
		return result;
	}


	Pasto1 = new Billboard(OpenGL, handlerWindow, LoaderTexture, "recursos/Bilboars/grass2.png");
	if (!Pasto1) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the billboard.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		Pasto1->Initialize(1.0f);
		Pasto1->SetShader(ShaderBill);
	}
	Pasto2 = new Billboard(OpenGL, handlerWindow, LoaderTexture, "recursos/Bilboars/grass.png");
	if (!Pasto2) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the billboard.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		Pasto2->Initialize(2.0f);
		Pasto2->SetShader(ShaderBill);
	}

	Pasto3 = new Billboard(OpenGL, handlerWindow, LoaderTexture, "recursos/Bilboars/Bote.png");
	if (!Pasto3) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the billboard.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		Pasto3->Initialize(5.0f);
		Pasto3->SetShader(ShaderBill);
	}

	Pasto4 = new Billboard(OpenGL, handlerWindow, LoaderTexture, "recursos/Bilboars/grass4.png");
	if (!Pasto4) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the billboard.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		Pasto4->Initialize(2.0f);
		Pasto4->SetShader(ShaderBill);
	}


	Pasto5 = new Billboard(OpenGL, handlerWindow, LoaderTexture, "recursos/Bilboars/Piedra.png");
	if (!Pasto5) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the billboard.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		Pasto5->Initialize(2.0f);
		Pasto5->SetShader(ShaderBill);
	}


	Pasto6 = new Billboard(OpenGL, handlerWindow, LoaderTexture, "recursos/Bilboars/basura1.png");
	if (!Pasto6) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the billboard.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		Pasto6->Initialize(5.0f);
		Pasto6->SetShader(ShaderBill);
	}


	return result;
}

bool Scene::Render() {
	bool result = true;

	// Limpia los buffers para iniciar la escena nueva.
	OpenGL->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Definimos nuestro espacio para las 3 matrices necesarias para renderizar.
	float worldMatrix[16];
	float viewMatrix[16];
	float projectionMatrix[16];

	// Actualiza la posicion y rotacion de la camara
	Camera->SetPosition(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z);
	Camera->SetRotation(DeltaRotation->X, DeltaRotation->Y, DeltaRotation->Z);

	// Genera la matriz de vista a partir de la posicion de la camara.
	Camera->Render();

	// Obtiene las matrices de vista, mundo y proyeccion de opengl y el objeto de la camara.
	OpenGL->GetWorldMatrix(worldMatrix);
	Camera->GetViewMatrix(viewMatrix);
	OpenGL->GetProjectionMatrix(projectionMatrix);

	// Renderizamos el Skydome
	Skydome->CicloInterpolaciones();
	Skydome->Render(viewMatrix, projectionMatrix);

	// Renderizamos terreno
	Terreno->Render(OpenGL, worldMatrix, viewMatrix, projectionMatrix);
	Agua->Render(OpenGL, viewMatrix, projectionMatrix);
	// Renderizamos Triangulo (ya se logra ver en escena)
	/*Triangulo->Render(viewMatrix, projectionMatrix);*/

	// Renderizamos nuestros objetos en la escena

	EdificioP->Render(viewMatrix, projectionMatrix, false);
	EdificioT->Render(viewMatrix, projectionMatrix, false);
	Tren->Render(viewMatrix, projectionMatrix, false);
	Cancha->Render(viewMatrix, projectionMatrix, false);
	Rocas->Render(viewMatrix, projectionMatrix, false);
	Oso->Render(viewMatrix, projectionMatrix, false);
	Caja->Render(viewMatrix, projectionMatrix, false);
	Tronco->Render(viewMatrix, projectionMatrix, false);
	Tronco2->Render(viewMatrix, projectionMatrix, false);
	Vagon->Render(viewMatrix, projectionMatrix, false);
	Tienda2->Render(viewMatrix, projectionMatrix, false);
	Pared1->Render(viewMatrix, projectionMatrix, false);
	Pared2->Render(viewMatrix, projectionMatrix, false);
	Pared3->Render(viewMatrix, projectionMatrix, false);
	Pared4->Render(viewMatrix, projectionMatrix, false);
	Pared5->Render(viewMatrix, projectionMatrix, false);
	Pared6->Render(viewMatrix, projectionMatrix, false);

	Carro->Render(viewMatrix, projectionMatrix, false);

	if (thirdPersonView == false) {
		Camera->SetPosition(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z);
	}
	else
	{
		Camera->SetPosition(DeltaPosition->X, DeltaPosition->Y + 5.0f, DeltaPosition->Z);
	}

	// Renderizamos las cajas de colisión

	/*box->Draw(viewMatrix, projectionMatrix);
	box2->Draw(viewMatrix, projectionMatrix);
	box3->Draw(viewMatrix, projectionMatrix);
	box4->Draw(viewMatrix, projectionMatrix);*/

	//box5->Draw(viewMatrix, projectionMatrix);
	//box6->Draw(viewMatrix, projectionMatrix);
	//box7->Draw(viewMatrix, projectionMatrix);
	//box8->Draw(viewMatrix, projectionMatrix);
	//box9->Draw(viewMatrix, projectionMatrix);
	//box10->Draw(viewMatrix, projectionMatrix);
	//box11->Draw(viewMatrix, projectionMatrix);
	//box12->Draw(viewMatrix, projectionMatrix);

	// Renderizamos los billboards

	Pasto1->Render(viewMatrix, projectionMatrix,
		0.0f, Terreno->Superficie(0.0f, 0.0f), 0.0f,
		DeltaPosition->X, DeltaPosition->Z);
	Pasto1->Render(viewMatrix, projectionMatrix,
		10.0f, Terreno->Superficie(10.0f, -10.0f), -10.0f,
		DeltaPosition->X, DeltaPosition->Z);

	Pasto1->Render(viewMatrix, projectionMatrix,
		35.0f, Terreno->Superficie(35.0f, 35.0f), 35.0f,
		DeltaPosition->X, DeltaPosition->Z);

	Pasto2->Render(viewMatrix, projectionMatrix,
		-10.0f, Terreno->Superficie(-10.0f, -10.0f), -10.0f,
		DeltaPosition->X, DeltaPosition->Z);

	Pasto2->Render(viewMatrix, projectionMatrix,
		-10.0f, Terreno->Superficie(-10.0f, 10.0f), 10.0f,
		DeltaPosition->X, DeltaPosition->Z);

	Pasto3->Render(viewMatrix, projectionMatrix,
		49.0f, Terreno->Superficie(49.0f, 29.0f), 29.0f,
		DeltaPosition->X, DeltaPosition->Z);

	Pasto4->Render(viewMatrix, projectionMatrix,
		-78.0f, Terreno->Superficie(-78.0f, -48.0f), -48.0f,
		DeltaPosition->X, DeltaPosition->Z);

	Pasto4->Render(viewMatrix, projectionMatrix,
		-20.0f, Terreno->Superficie(-20.0f, 95.0f), 95.0f,
		DeltaPosition->X, DeltaPosition->Z);
	Pasto5->Render(viewMatrix, projectionMatrix,
		-51.0f, Terreno->Superficie(-51.0f, -78.0f), -78.0f,
		DeltaPosition->X, DeltaPosition->Z);

	Pasto6->Render(viewMatrix, projectionMatrix,
		-2.0f, Terreno->Superficie(-2.0f, 85.0f), 85.0f,
		DeltaPosition->X, DeltaPosition->Z);
	/*arbol2D->Render(viewMatrix, projectionMatrix, 
		-10.0f, Terreno->Superficie(-10.0f, -10.0f), -10.0f, 
		DeltaPosition->X, DeltaPosition->Z);

	arbol2D->Render(viewMatrix, projectionMatrix, 
		10.0f, Terreno->Superficie(10.0f, 10.0f), 10.0f, 
		DeltaPosition->X, DeltaPosition->Z);

	arbol2D->Render(viewMatrix, projectionMatrix, 
		-10.0f, Terreno->Superficie(-10.0f, 10.0f), 10.0f, 
		DeltaPosition->X, DeltaPosition->Z);

	arbol2D->Render(viewMatrix, projectionMatrix, 
		10.0f, Terreno->Superficie(10.0f, -10.0f), -10.0f, 
		DeltaPosition->X, DeltaPosition->Z);*/

	// Damos la instruccion de que termino la escena para que nos muestre frame a frame.
	OpenGL->EndScene();

	return result;
}

bool Scene::Update(InputClass* input, float deltaTime) {
	bool result = true;

	this->input = input;
	this->deltaTime = deltaTime;
	result = ManageCommands();
	if (!result) {
		result = false;
		_RPT1(0, "Alert! Error on key. \n", 0);
		return result;
	}
	float* auxMatrix = new float[16] { 0.0f };
	OpenGL->BuildIdentityMatrix(auxMatrix);

	float* matrixGameObject = EdificioP->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject, -105.0f, 37.0f, 14.0f);

	float* matrixGameObject1 = EdificioT->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject1, -105.0f, 37.0f, 14.0f);

	float* matrixGameObject2 = Tren->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject2, -30.0f, 16.0f, -88.0f);

	float* matrixGameObject3 = Cancha->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject3, 35.0f, 14.5, 115.0f);

	float* matrixGameObject4 = Rocas->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject4, 130.0f,5.0f, 0.0f);

	float* matrixGameObject5 = Oso->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject5, -17.0f,11.0f, 101.0f);

	float* matrixGameObject6 = Caja->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject6, -60.0f, 13.0f, 59.0f);

	float* matrixGameObject7 = Tronco->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject7, -20.0f, 13.0f, -29.0f);

	float* matrixGameObject8 = Tronco2->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject8, 141.0f, 50.0f, 118.0f);

	float* matrixGameObject9 = Vagon->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject9, 56.0f, 10.0f, -122.0f);

	float* matrixGameObject10 = Tienda2->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject10, -90.0f, 35.0f, 111.0f);

	float* matrixGameObject11 = Pared1->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject11, -90.0f, 35.0f, 111.0f);

	float* matrixGameObject12 = Pared2->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject12, -90.0f, 35.0f, 111.0f);

	float* matrixGameObject13 = Pared3->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject13, -90.0f, 35.0f, 111.0f);

	float* matrixGameObject14 = Pared4->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject14, -90.0f, 35.0f, 111.0f);

	float* matrixGameObject15 = Pared5->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject15, -90.0f, 35.0f, 111.0f);

	float* matrixGameObject16 = Pared6->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixGameObject16, -90.0f, 35.0f, 111.0f);

	

	if (thirdPersonView == false) {
		float* matrixGameObject17 = Carro->GetWorldMatrix();
		OpenGL->MatrixTranslation(matrixGameObject17, 10.0f, 18.0f, -10.0f);
		
	}
	else {
		float* matrixGameObject18 = Carro->GetWorldMatrix();
		OpenGL->MatrixRotationY(matrixGameObject18, DeltaRotation->Y*(M_PI/180));
		OpenGL->MatrixTranslation(auxMatrix, DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z);
		OpenGL->MatrixMultiply( matrixGameObject18, matrixGameObject18, auxMatrix);
	}
	
	/*float* matrixTriangle = Triangulo->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixTriangle, -20.0f, 8.0f, 0.0f);*/


	//Tranformaciones de cajas de colisión

	/*float* auxMatrix = new float[16]{ 0.0f };
	OpenGL->BuildIdentityMatrix(auxMatrix);*/

	/*float* matrixBox = box->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixBox, -145.0f, 15.0f, 147.0f);*/

	/*OpenGL->MatrixScale(auxMatrix, 1.0f, 1.0f, 0.3f);
	OpenGL->MatrixMultiply(matrixBox, matrixBox, auxMatrix);*/

	//float* matrixBox2 = box2->GetWorldMatrix();
	//OpenGL->MatrixTranslation(matrixBox2, -147.0f,15.0f, -145.0f);

	/*OpenGL->MatrixScale(auxMatrix, 0.3f, 1.0f, 1.0f);
	OpenGL->MatrixMultiply(matrixBox2, matrixBox2, auxMatrix);*/

	//Colisión por esfera
	/*if (Object3d->GetSphericalCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z, 2)) {
		MessageBox(handlerWindow, L"Colisionando", L"Aviso", MB_OK);
	}*/

	//Colisión por caja

	/*if (EdificioP->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}*/

	if (Tren->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Cancha->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Rocas->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Oso->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Caja->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Tronco->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}

	if (Tronco2->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}

	if (Vagon->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}

	if (Tienda2->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}

	if (Carro->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}


	if (box->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}

	if (box2->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (box3->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (box4->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (box5->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (box6->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (box7->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (box8->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (box9->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (box10->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (box11->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (box12->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}

	return result;
}

bool Scene::ManageCommands() {
	bool result = true;
	
	//Guardar la posición de la Cámara antes de actualizarla
	LastDeltaPosition->X = DeltaPosition->X;
	LastDeltaPosition->Y = DeltaPosition->Y;
	LastDeltaPosition->Z = DeltaPosition->Z;

	float vr[2]{ 0,0 };
	float radians = DeltaRotation->Y * 0.0174532925f;

	// Mostrar cordenadas de nuestra posicion
	if (input->GetKey(KeyCode.F10)) {
		std::string info = "X: " + std::to_string(DeltaPosition->X)
			+ " Y: " + std::to_string(DeltaPosition->Y)
			+ " Z: " + std::to_string(DeltaPosition->Z);

		int bufferLength = MultiByteToWideChar(CP_UTF8, 0, info.c_str(), -1, NULL, 0);

		wchar_t* wideInfo = new wchar_t[bufferLength];

		MultiByteToWideChar(CP_UTF8, 0, info.c_str(), -1, wideInfo, bufferLength);

		MessageBox(NULL, wideInfo, L"Coordenadas", MB_OK);

		delete[] wideInfo;
	}

	if((input->GetKey(KeyCode.W) || input->GetKey(KeyCode.I))) {
		vr[0] += sinf(radians)*speed * deltaTime;
		vr[1] += cosf(radians) * speed * deltaTime;
	}
	if ((input->GetKey(KeyCode.S) || input->GetKey(KeyCode.K))) {
		vr[0] -= sinf(radians) * speed * deltaTime;
		vr[1] -= cosf(radians) * speed * deltaTime;
	}
	if ((input->GetKey(KeyCode.A) || input->GetKey(KeyCode.J))) {
		vr[0] -= sinf(radians + (M_PI * 0.5f)) * speed * deltaTime;
		vr[1] -= cosf(radians + (M_PI * 0.5f)) * speed * deltaTime;
	}
	if ((input->GetKey(KeyCode.D) || input->GetKey(KeyCode.L))) {
		vr[0] += sinf(radians + (M_PI * 0.5f)) * speed * deltaTime;
		vr[1] += cosf(radians + (M_PI * 0.5f)) * speed * deltaTime;
	}

	if (input->GetRightAxisY() || input->GetRightAxisX()) {
		float angleX = input->GetRightAxisY() * speedAxisX * deltaTime;
		float angleY = input->GetRightAxisX() * speedAxisY * deltaTime;
		DeltaRotation->X -= angleX;
		DeltaRotation->Y += angleY;
	}

	if (input->GetKey(KeyCode.Up)) {
		DeltaRotation->X -= speedAxisX * deltaTime;
	}
	if (input->GetKey(KeyCode.Down)) {
		DeltaRotation->X += speedAxisX * deltaTime;
	}
	if (input->GetKey(KeyCode.Left)) {
		DeltaRotation->Y -= speedAxisY * deltaTime;
	}
	if (input->GetKey(KeyCode.Right)) {
		DeltaRotation->Y += speedAxisY * deltaTime;
	}
	
	int i = 2;

	if (input->GetKey(KeyCode.E)) {
		thirdPersonView = !thirdPersonView;  // Cambia entre verdadero y falso al presionar E
	}

	if (input->GetKey(KeyCode.Space)) {
		DeltaPosition->Y = DeltaPosition->Y + i;
		//DeltaRotation->X -= speedAxisX * deltaTime * 3;
		i = i + 1;
	}
	if (input->GetKey(KeyCode.Control)) {
		DeltaPosition->Y = DeltaPosition->Y - i;
		//	DeltaRotation->X += speedAxisX * deltaTime * 3;
		i = i - 1;
	}



	//if (input->GetKey(KeyCode.One)) {
	//	Tienda2->ChangeTexture(0, 0);
	//}

	/*if (input->GetKey(KeyCode.One)) {
		Object3d1->ChangeTexture(0,0);
	}
	if (input->GetKey(KeyCode.Two)) {
		Object3d1->ChangeTexture(1, 1);
	}
	if (input->GetKey(KeyCode.Three)) {
		Object3d1->ChangeTexture(2, 2);
	}
	if (input->GetKey(KeyCode.Four)) {
		Object3d1->ChangeTexture(3, 3);
	}
	if (input->GetKey(KeyCode.Five)) {
		Object3d1->ChangeTexture(4, 4);
	}
	if (input->GetKey(KeyCode.Six)) {
		Object3d1->ChangeTexture(5, 5);
	}
	if (input->GetKey(KeyCode.Seven)) {
		Object3d1->ChangeTexture(6, 6);
	}
	if (input->GetKey(KeyCode.Eight)) {
		Object3d1->ChangeTexture(7, 7);
	}
	if (input->GetKey(KeyCode.Nine)) {
		Object3d1->ChangeTexture(8, 8);
	}
	if (input->GetKey(KeyCode.Zero)) {
		Object3d1->ChangeTexture(9, 9);
	}*/

	if (!thirdPersonView) {
		DeltaPosition->Y = Terreno->Superficie(DeltaPosition->X, DeltaPosition->Z) + 5;
	}
	/*DeltaPosition->Y = Terreno->Superficie(DeltaPosition->X, DeltaPosition->Z) + 2;*/

	DeltaPosition->X += vr[0];
	DeltaPosition->Z += vr[1];

	return result;
}

bool Scene::Shutdown() {
	bool result = true;

	if (Camera)
	{
		delete Camera;
		Camera = 0;
	}
	

	if (LightShader)
	{
		LightShader->Shutdown();
		delete LightShader;
		LightShader = 0;
	}
	if (Terreno) {
		Terreno->Shutdown();
		delete Terreno;
		Terreno = 0;
	}
	if (Agua) {
		Agua->Shutdown();
		delete Agua;
		Agua = 0;
	}
	if (Skydome) {
		Skydome->Shutdown();
		delete Skydome;
		Skydome = 0;
	}
	
	if (EdificioP) {
		EdificioP->Shutdown();
		delete EdificioP;
		EdificioP = 0;
	}
	if (EdificioT) {
		EdificioT->Shutdown();
		delete EdificioT;
		EdificioT = 0;
	}
	if (Tren) {
		Tren->Shutdown();
		delete Tren;
		Tren = 0;
	}
	if (Cancha) {
		Cancha->Shutdown();
		delete Cancha;
		Cancha = 0;
	}
	if (Rocas) {
		Rocas->Shutdown();
		delete Rocas;
		Rocas = 0;
	}
	if (Caja) {
		Caja->Shutdown();
		delete Caja;
		Caja = 0;
	}
	if (Tronco) {
		Tronco->Shutdown();
		delete Tronco;
		Tronco = 0;
	}
	if (Tronco2) {
		Tronco2->Shutdown();
		delete Tronco2;
		Tronco2 = 0;
	}
	if (Vagon) {
		Vagon->Shutdown();
		delete Vagon;
		Vagon = 0;
	}
	if (Tienda2) {
		Tienda2->Shutdown();
		delete Tienda2;
		Tienda2 = 0;
	}
	if (Pared1) {
		Pared1->Shutdown();
		delete Pared1;
		Pared1 = 0;
	}
	if (Pared2) {
		Pared2->Shutdown();
		delete Pared2;
		Pared2 = 0;
	}
	if (Pared3) {
		Pared3->Shutdown();
		delete Pared3;
		Pared3 = 0;
	}
	if (Pared4) {
		Pared4->Shutdown();
		delete Pared4;
		Pared4 = 0;
	}
	if (Pared5) {
		Pared5->Shutdown();
		delete Pared5;
		Pared5 = 0;
	}
	if (Pared6) {
		Pared6->Shutdown();
		delete Pared6;
		Pared6 = 0;
	}
	if (Pasto1) {
		Pasto1->Shutdown();
		delete Pasto1;
		Pasto1 = 0;
	}
	if (Pasto2) {
		Pasto2->Shutdown();
		delete Pasto2;
		Pasto2 = 0;
	}
	if (Carro) {
		Carro->Shutdown();
		delete Carro;
		Carro = 0;
	}
	
	return result;
}