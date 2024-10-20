#include "stdafx.h"
#include "FieldState.h"
#include "GameFramework.h"
#include "Globals.h"
#include "PauseMenuState.h"
#include "TitleMenuState.h"

void Display();
void Reshape(int w, int h);
void Idle();

// 마우스, 키보드 조작
// 마우스 버튼 클릭
void MouseInput(int button, int state, int x, int y);
// 마우스 버튼을 누른 채 이동
void MotionInput(int x, int y);
// 마우스 버튼을 누르지 않은 채 이동
void PassiveMotionInput(int x, int y);
// 마우스 스크롤 휠 
void MouseWheel(int button, int dir, int x, int y);
// 키보드 키 다운
void KeyboardInput(unsigned char key, int x, int y);
// 키보드 키 업
void KeyboardUpInput(unsigned char key, int x, int y);
// 키보드 특수 키 다운
void KeyboardSpecialInput(int key, int x, int y);
// 키보드 특수 키 업
void KeyboardSpecialUpInput(int key, int x, int y);

// 게임 루프
void GameLoop(int temp);

// 전역 변수
bool g_focused = true;

CGameFramework* g_gameFramework = NULL;
GameInputs g_inputs;

// 일시정지(시스템) 메뉴 스테이트
CPauseMenuState* g_pauseMenuState = NULL;

int main(int argc, char** argv)
{
	// 윈도우 생성
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glutCreateWindow("E-Fight");

	// 콘솔창 띄우지 않기
	//FreeConsole();

	// GLEW 초기화
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Unable to initialize GLEW\n";
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	if (glewIsSupported("GL_VERSION_4_6"))
		std::cout << " GLEW Version is 4.6\n ";
	else
		std::cout << " GLEW 4.6 not supported\n ";

	g_gameFramework = CGameFramework::GetInstance();
	g_gameFramework->SetWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);

	g_pauseMenuState = new CPauseMenuState();
	CFieldState* fieldState = new CFieldState();
	fieldState->SetPauseMenuState(g_pauseMenuState);
	g_gameFramework->PushState(fieldState);
	g_gameFramework->PushState(g_pauseMenuState);
	g_pauseMenuState->UseButton(4);

	// 입력 초기화
	memset(&g_inputs, 0, sizeof(GameInputs));
	memset(&g_inputs.processingEnd, 1, sizeof(GameInputsProcessingEnd));

	g_inputs.MouseXPos = WINDOW_SIZE_X / 2;
	g_inputs.MouseYPos = WINDOW_SIZE_Y / 2;

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutIdleFunc(Idle);

	glutMouseFunc(MouseInput);
	glutMotionFunc(MotionInput);
	glutPassiveMotionFunc(PassiveMotionInput);
	glutMouseWheelFunc(MouseWheel);
	glutKeyboardFunc(KeyboardInput);
	glutKeyboardUpFunc(KeyboardUpInput);
	glutSpecialFunc(KeyboardSpecialInput);
	glutSpecialUpFunc(KeyboardSpecialUpInput);

	GameLoop(0);

	glutMainLoop();

	return 0;
}

void GameLoop(int temp)
{
	// 플레이어 입력 복사
	GameInputs tempInputs;
	memcpy(&tempInputs, &g_inputs, sizeof(GameInputs));

	int waitTime = 0;
	if (g_focused)
	{
		if (g_gameFramework->GetIsEnableToEnterSystemMenu()) {
			if (!g_pauseMenuState->GetIsOpened()) {
				if (g_inputs.IsPressedDownKeyOnce(&g_inputs.KeyESC, &g_inputs.processingEnd.KeyESC)) {
					g_gameFramework->PushState(g_pauseMenuState);
				}
			}
		}

		// 게임 프레임워크 루프 실행
		waitTime = g_gameFramework->Loop(&g_inputs);	
	}

	glutSwapBuffers();

	glutTimerFunc(waitTime, GameLoop, temp);
}

void Display()
{
	g_focused = true;
}

void Reshape(int w, int h)
{
	g_focused = false;

	g_gameFramework->SetWindowSize(w, h);
}

void Idle()
{

}

void MouseInput(int button, int state, int x, int y)
{
	switch (button) 
	{
	case GLUT_LEFT_BUTTON:
		switch (state) 
		{
		case GLUT_DOWN:
			g_inputs.processKey(&g_inputs.MouseLeftButton, &g_inputs.processingEnd.MouseLeftButton, true);
			break;
		case GLUT_UP:
			g_inputs.processKey(&g_inputs.MouseLeftButton, &g_inputs.processingEnd.MouseLeftButton, false);
			break;
		}
		break;

	case GLUT_RIGHT_BUTTON:
		switch (state) 
		{
		case GLUT_DOWN:
			g_inputs.processKey(&g_inputs.MouseRightButton, &g_inputs.processingEnd.MouseRightButton, true);
			break;
		case GLUT_UP:
			g_inputs.processKey(&g_inputs.MouseRightButton, &g_inputs.processingEnd.MouseRightButton, false);
			break;
		}
		break;
	}
}

void MotionInput(int x, int y)
{
	g_inputs.MouseXPos = x;
	g_inputs.MouseYPos = y;
}

void PassiveMotionInput(int x, int y)
{
	g_inputs.MouseXPos = x;
	g_inputs.MouseYPos = y;
}

void MouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0)
	{
		g_inputs.MouseWheelUp = true;
	}
	else if (dir < 0)
	{
		g_inputs.MouseWheelDown = true;
	}
}

void KeyboardInput(unsigned char key, int x, int y)
{
	switch (key) 
	{
	case 'w':
	case'W':
		g_inputs.processKey(&g_inputs.KeyW, &g_inputs.processingEnd.KeyW, true);
		break;
	case 's':
	case'S':
		g_inputs.processKey(&g_inputs.KeyS, &g_inputs.processingEnd.KeyS, true);
		break;
	case 'a':
	case'A':
		g_inputs.processKey(&g_inputs.KeyA, &g_inputs.processingEnd.KeyA, true);
		break;
	case 'd':
	case'D':
		g_inputs.processKey(&g_inputs.KeyD, &g_inputs.processingEnd.KeyD, true);
		break;

	case 'e':
	case'E':
		g_inputs.processKey(&g_inputs.KeyE, &g_inputs.processingEnd.KeyE, true);
		break;

	case 'f':
	case'F':
		g_inputs.processKey(&g_inputs.KeyF, &g_inputs.processingEnd.KeyF, true);
		break;

	case 'q':
	case'Q':
		g_inputs.processKey(&g_inputs.KeyQ, &g_inputs.processingEnd.KeyQ, true);
		break;

	case 't':
	case'T':
		g_inputs.processKey(&g_inputs.KeyT, &g_inputs.processingEnd.KeyT, true);
		break;

	case 'r':
	case'R':
		g_inputs.processKey(&g_inputs.KeyR, &g_inputs.processingEnd.KeyR, true);
		break;

	case 'c':
	case'C':
		g_inputs.processKey(&g_inputs.KeyC, &g_inputs.processingEnd.KeyC, true);
		break;

	case 'b':
	case'B':
		g_inputs.processKey(&g_inputs.KeyB, &g_inputs.processingEnd.KeyB, true);
		break;

	case 'x':
	case'X':
		g_inputs.processKey(&g_inputs.KeyX, &g_inputs.processingEnd.KeyX, true);
		break;
	case 'z':
	case'Z':
		g_inputs.processKey(&g_inputs.KeyZ, &g_inputs.processingEnd.KeyZ, true);
		break;

	case '+':
		g_inputs.processKey(&g_inputs.KeyPlus, &g_inputs.processingEnd.KeyPlus, true);
		break;
	case '-':
		g_inputs.processKey(&g_inputs.KeyMinus, &g_inputs.processingEnd.KeyMinus, true);
		break;

	case ' ':
		g_inputs.processKey(&g_inputs.KeySpace, &g_inputs.processingEnd.KeySpace, true);
		break;

	case KEY_ESC:
		g_inputs.processKey(&g_inputs.KeyESC, &g_inputs.processingEnd.KeyESC, true);
		break;

	case KEY_TAB:
		g_inputs.processKey(&g_inputs.KeyTab, &g_inputs.processingEnd.KeyTab, true);
		break;
	}
}

void KeyboardUpInput(unsigned char key, int x, int y)
{
	switch (key) 
	{
	case 'w':
	case'W':
		g_inputs.processKey(&g_inputs.KeyW, &g_inputs.processingEnd.KeyW, false);
		break;
	case 's':
	case'S':
		g_inputs.processKey(&g_inputs.KeyS, &g_inputs.processingEnd.KeyS, false);
		break;
	case 'a':
	case'A':
		g_inputs.processKey(&g_inputs.KeyA, &g_inputs.processingEnd.KeyA, false);
		break;
	case 'd':
	case'D':
		g_inputs.processKey(&g_inputs.KeyD, &g_inputs.processingEnd.KeyD, false);
		break;

	case 'e':
	case'E':
		g_inputs.processKey(&g_inputs.KeyE, &g_inputs.processingEnd.KeyE, false);
		break;

	case 'f':
	case'F':
		g_inputs.processKey(&g_inputs.KeyF, &g_inputs.processingEnd.KeyF, false);
		break;

	case 'q':
	case'Q':
		g_inputs.processKey(&g_inputs.KeyQ, &g_inputs.processingEnd.KeyQ, false);
		break;

	case 't':
	case'T':
		g_inputs.processKey(&g_inputs.KeyT, &g_inputs.processingEnd.KeyT, false);
		break;

	case 'r':
	case'R':
		g_inputs.processKey(&g_inputs.KeyR, &g_inputs.processingEnd.KeyR, false);
		break;


	case 'c':
	case'C':
		g_inputs.processKey(&g_inputs.KeyC, &g_inputs.processingEnd.KeyC, false);
		break;

	case 'b':
	case'B':
		g_inputs.processKey(&g_inputs.KeyB, &g_inputs.processingEnd.KeyB, false);
		break;

	case 'x':
	case'X':
		g_inputs.processKey(&g_inputs.KeyX, &g_inputs.processingEnd.KeyX, false);
		break;
	case 'z':
	case'Z':
		g_inputs.processKey(&g_inputs.KeyZ, &g_inputs.processingEnd.KeyZ, false);
		break;

	case '+':
		g_inputs.processKey(&g_inputs.KeyPlus, &g_inputs.processingEnd.KeyPlus, false);
		break;
	case '-':
		g_inputs.processKey(&g_inputs.KeyMinus, &g_inputs.processingEnd.KeyMinus, false);
		break;

	case ' ':
		g_inputs.processKey(&g_inputs.KeySpace, &g_inputs.processingEnd.KeySpace, false);
		break;

	case KEY_ESC:
		g_inputs.processKey(&g_inputs.KeyESC, &g_inputs.processingEnd.KeyESC, false);
		break;

	case KEY_TAB:
		g_inputs.processKey(&g_inputs.KeyTab, &g_inputs.processingEnd.KeyTab, false);
		break;
	}
}

void KeyboardSpecialInput(int key, int x, int y)
{
	switch (key) 
	{
	case GLUT_KEY_F1:
		// 전체 화면<->창 화면 설정
		glutFullScreenToggle();
		break;

	case GLUT_KEY_F2:
		g_inputs.processKey(&g_inputs.KeyF2, &g_inputs.processingEnd.KeyF2, true);
		break;

	case GLUT_KEY_F3:
		// 월드에서의 현재 주인공 위치 출력
		std::cout << CBlackBoard::GetInstance()->GetHeroPosition().x << ", " <<
			CBlackBoard::GetInstance()->GetHeroPosition().y << ", " <<
			CBlackBoard::GetInstance()->GetHeroPosition().z << std::endl;
		break;

	case GLUT_KEY_F4:
		// 프로그램 종료
		glutExit();
		break;

	case GLUT_KEY_SHIFT_L:
		g_inputs.processKey(&g_inputs.KeyLeftShift, &g_inputs.processingEnd.KeyLeftShift, true);
		break;

	case GLUT_KEY_CTRL_L:
		g_inputs.processKey(&g_inputs.KeyLeftCtrl, &g_inputs.processingEnd.KeyLeftCtrl, true);
		break;

	case GLUT_KEY_UP:
		g_inputs.processKey(&g_inputs.KeyArrowUp, &g_inputs.processingEnd.KeyArrowUp, true);
		break;
	case GLUT_KEY_DOWN:
		g_inputs.processKey(&g_inputs.KeyArrowDown, &g_inputs.processingEnd.KeyArrowDown, true);
		break;
	case GLUT_KEY_LEFT:
		g_inputs.processKey(&g_inputs.KeyArrowLeft, &g_inputs.processingEnd.KeyArrowLeft, true);
		break;
	case GLUT_KEY_RIGHT:
		g_inputs.processKey(&g_inputs.KeyArrowRight, &g_inputs.processingEnd.KeyArrowRight, true);
		break;
	}
}

void KeyboardSpecialUpInput(int key, int x, int y)
{
	switch (key) 
	{

	case GLUT_KEY_F2:
		g_inputs.processKey(&g_inputs.KeyF2, &g_inputs.processingEnd.KeyF2, false);
		break;

	case GLUT_KEY_SHIFT_L:
		g_inputs.processKey(&g_inputs.KeyLeftShift, &g_inputs.processingEnd.KeyLeftShift, false);
		break;

	case GLUT_KEY_CTRL_L:
		g_inputs.processKey(&g_inputs.KeyLeftCtrl, &g_inputs.processingEnd.KeyLeftCtrl, false);
		break;

	case GLUT_KEY_UP:
		g_inputs.processKey(&g_inputs.KeyArrowUp, &g_inputs.processingEnd.KeyArrowUp, false);
		break;
	case GLUT_KEY_DOWN:
		g_inputs.processKey(&g_inputs.KeyArrowDown, &g_inputs.processingEnd.KeyArrowDown, false);
		break;
	case GLUT_KEY_LEFT:
		g_inputs.processKey(&g_inputs.KeyArrowLeft, &g_inputs.processingEnd.KeyArrowLeft, false);
		break;
	case GLUT_KEY_RIGHT:
		g_inputs.processKey(&g_inputs.KeyArrowRight, &g_inputs.processingEnd.KeyArrowRight, false);
		break;
	}
}

