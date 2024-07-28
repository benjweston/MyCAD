#include "DRW_glrenderthread.h"

#include "fault.h"
#include "log.h"

#include "glExtension.h"
#include "wglext.h"

#include <chrono>
#include <commctrl.h>
#include <windowsx.h>

#define FPSCOUNTER 0x0010
#define SWAPBUFFERS 0x0020

//#define TCN_FIRST (0U-550U)
//#define TCN_FIRST 0xFFFFFFFFU-549U
//#define TCN_SELCHANGE (0xFFFFFFFFU-550U)

#define MAKEPOINT(lParam) POINT({ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) })

using namespace mycad;

const double c_scaleKeyboard = 1.0;
const double c_scaleMouse = 1.0;
const double c_scaleScroll = 0.25;
const double c_scaleWheel = -1.0 / 6.0;

GLRenderThread::GLRenderThread()
	: m_lpCommandManager(nullptr),
	m_hWnd(0),
	m_lpGLRenderContext(0),
	m_cx(0),
	m_cy(0),
	m_lpLayoutManager(0),
	m_nFPS(60),
	m_bRenderLoop(true),
	m_bMouseInRect(true),
	m_bVBOSupported(false),
	m_bGLSLSupported(false),
	m_dwKeystate(0),
	m_ptMouseDown({ 0,0 }),
	m_ptMouseUp({ 0,0 }),
	m_ptMouseMove({ 0,0 }),
	m_ptCursor({ 0,0 }),
	m_bMouseMoved(false)
{
	dwThreadId = GetCurrentThreadId();
}
GLRenderThread::GLRenderThread(CommandManager* commandmanager)
	: m_lpCommandManager(commandmanager),
	m_hWnd(0),
	m_lpGLRenderContext(0),
	m_cx(0),
	m_cy(0),
	m_lpLayoutManager(0),
	m_nFPS(60),
	m_bRenderLoop(true),
	m_bSwapBuffers(false),
	m_bMouseInRect(true),
	m_bVBOSupported(false),
	m_bGLSLSupported(false),
	m_dwKeystate(0),
	m_ptMouseDown({ 0,0 }),
	m_ptMouseUp({ 0,0 }),
	m_ptMouseMove({ 0,0 }),
	m_ptCursor({ 0,0 }),
	m_bMouseMoved(false)
{
	dwThreadId = GetCurrentThreadId();
}
GLRenderThread::~GLRenderThread()
{
	if (m_lpCommandManager->getActiveObject(CommandContext::VIEW) != nullptr) {
		m_lpCommandManager->setObjectInactive(CommandContext::VIEW);
	}

	if (m_lpCommandManager->getActiveObject(CommandContext::COMMAND) != nullptr) {
		m_lpCommandManager->setObjectInactive(CommandContext::COMMAND);
	}

	delete m_lpLayoutManager;
	m_lpLayoutManager = NULL;
}

void GLRenderThread::setSwapBuffers() { m_bSwapBuffers = true; }

CommandManager* GLRenderThread::getCommandManager()
{
	std::lock_guard<std::mutex> lkg(m_CommandManagerMutex);
	return m_lpCommandManager;
}
DataBlock* GLRenderThread::Entities()
{
	std::lock_guard<std::mutex> lkg(m_DataBlocksMutex);
	return &m_DataBlocks.m_mBlock;
}
LayerManager* GLRenderThread::getLayerManager()
{
	std::lock_guard<std::mutex> lkg(m_LayerManagerMutex);
	return &m_LayerManager;
}
LayoutManager* GLRenderThread::getLayoutManager()
{
	std::lock_guard<std::mutex> lkg(m_LayoutManagerMutex);
	return m_lpLayoutManager;
}
ViewManager* GLRenderThread::getViewManager()
{
	std::lock_guard<std::mutex> lkg(m_ViewManagerMutex);
	return &m_ViewManager;
}

Vector3<double>GLRenderThread::getCoordinates()
{
	Vector3<double>v3Coordinates = { 0.0,0.0,0.0 };

	if (m_lpLayoutManager != nullptr) {
		POINT ptCoordinates = WinToGL(m_ptMouseMove);
		ptCoordinates.x -= m_cx / 2;
		ptCoordinates.y -= m_cy / 2;
		v3Coordinates = m_lpLayoutManager->getActiveObject()->getActiveViewport()->getCamera()->UnProject(ptCoordinates);
	}

	return v3Coordinates;
}

POINT GLRenderThread::WinToGL(POINT point)
{
	LONG x = point.x;
	LONG y = m_cy - point.y;
	return POINT{ x,y };
}
Vector2<double> GLRenderThread::getKeyBoardInput(UINT keycode)
{
	Vector2<double> vec2dReturnValue = { 0.0,0.0 };
	switch (keycode) {
		case VK_NUMPAD1: {
			vec2dReturnValue = { -10.0, 10.0 };
			break;
		}
		case VK_NUMPAD2:
		case VK_DOWN: {
			vec2dReturnValue = { 0.0, 10.0 };
			break;
		}
		case VK_NUMPAD3: {
			vec2dReturnValue = { 10.0, 10.00 };
			break;
		}
		case VK_NUMPAD4:
		case VK_LEFT: {
			vec2dReturnValue = { -10.0, 0.0 };
			break;
		}
		case VK_NUMPAD5: {
			if ((GetKeyState(VK_SHIFT) & 0x0001) == 0) {
				vec2dReturnValue = { 0.0, -10.0 };
			}
			else if ((GetKeyState(VK_SHIFT) & 0x0001) != 0) {
				vec2dReturnValue = { 0.0, 10.0 };
			}
			break;
		}
		case VK_NUMPAD6:
		case VK_RIGHT: {
			vec2dReturnValue = { 10.0, 0.0 };
			break;
		}
		case VK_NUMPAD7: {
			vec2dReturnValue = { -10.0, -10.0 };
			break;
		}
		case VK_NUMPAD8:
		case VK_UP: {
			vec2dReturnValue = { 0.0, -10.0 };
			break;
		}
		case VK_NUMPAD9: {
			vec2dReturnValue = { 10.0, -10.0 };
		}
		default: {
			break;
		}
	}
	return vec2dReturnValue;
}

void GLRenderThread::SendThreadMessage(UINT uMsg)
{
	std::shared_ptr<MessageParameters> mp(new MessageParameters(0, 0));
	SendThreadMessage(uMsg, mp);
}
void GLRenderThread::SendThreadMessage(UINT uMsg, WPARAM wParam)
{
	std::shared_ptr<MessageParameters> mp(new MessageParameters(wParam, 0));
	SendThreadMessage(uMsg, mp);
}
void GLRenderThread::SendThreadMessage(UINT uMsg, LPARAM lParam)
{
	std::shared_ptr<MessageParameters> mp(new MessageParameters(0, lParam));
	SendThreadMessage(uMsg, mp);
}
void GLRenderThread::SendThreadMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	std::shared_ptr<MessageParameters> mp(new MessageParameters(wParam, lParam));
	SendThreadMessage(uMsg, mp);
}

void GLRenderThread::tm_hScroll(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	if (m_lpLayoutManager == nullptr) return;

	double dx = (double)(int)lParam * c_scaleScroll;
	m_lpLayoutManager->getActiveSpace()->move(dx, 0.0);

	tm_paint();
}
void GLRenderThread::tm_keyDown(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	WORD vkCode = LOWORD(wParam);									// Virtual-key code

	if (m_lpLayoutManager == nullptr) return;

	if (m_lpCommandManager->getActiveObject(CommandContext::VIEW) != nullptr) {
		UINT command = m_lpCommandManager->getActiveObject(CommandContext::VIEW)->getID();
		switch (command) {
			case IDC_PAN: {
				Vector2<double> vec2d = getKeyBoardInput(vkCode);
				m_lpLayoutManager->getActiveSpace()->move(vec2d.x, -vec2d.y);
				break;
			}
			case IDC_ZOOM: {
				Vector2<double> vec2d = getKeyBoardInput(vkCode);
				m_lpLayoutManager->getActiveSpace()->zoom(vec2d.y);
				break;
			}
			case IDC_3DDISTANCE: {
				Vector2<double> vec2d = getKeyBoardInput(vkCode);
				m_lpLayoutManager->getActiveObject()->getActiveViewport()->getCamera()->setDistance(-vec2d.y);
				break;
			}
			case IDC_3DORBIT:
			case IDC_3DFORBIT:
			case IDC_3DCORBIT: {
				Vector2<double> vec2d = getKeyBoardInput(vkCode);
				m_lpLayoutManager->getActiveSpace()->rotate(vec2d.y, 0.0, vec2d.x);
				break;
			}
			default: {
				break;
			}
		}
		tm_paint();
	}
}
void GLRenderThread::tm_lButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	if (m_lpLayoutManager == nullptr) return;

	m_ptMouseDown = MAKEPOINT(lParam);
	m_ptMouseMove = m_ptMouseDown;

	m_lpLayoutManager->getActiveObject()->setActiveSpace(WinToGL(m_ptMouseDown));

	m_lpLayoutManager->getActiveObject()->mousemove(WinToGL(m_ptMouseMove));
}
void GLRenderThread::tm_lButtonDown(WPARAM wParam, LPARAM lParam)
{
	m_dwKeystate = GET_KEYSTATE_WPARAM(wParam);

	if (m_lpLayoutManager == nullptr) return;

	POINT ptMouseDown = MAKEPOINT(lParam);

	if (m_lpLayoutManager->getActiveObject()->getActiveSpace()->getContext() == Space::Context::ModelSpace) { 
		if (m_lpLayoutManager->getActiveObject()->getActiveSpace()->PtInRect(WinToGL(ptMouseDown)) == FALSE) return;
	}

	m_ptMouseDown = ptMouseDown;
	m_ptMouseMove = m_ptMouseDown;

	tm_paint();
}
void GLRenderThread::tm_lButtonUp(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	m_dwKeystate = 0x0000;

	if (m_lpLayoutManager == nullptr) return;

	POINT ptMouseUp = MAKEPOINT(lParam);

	if (m_lpLayoutManager->getActiveObject()->getActiveSpace()->getContext() == Space::Context::ModelSpace) {
		if (m_lpLayoutManager->getActiveObject()->getActiveSpace()->PtInRect(WinToGL(ptMouseUp)) == FALSE) return;
	}

	m_ptMouseUp = ptMouseUp;
	m_ptMouseMove = m_ptMouseUp;

	tm_paint();
}
void GLRenderThread::tm_mouseLeave() { m_bMouseInRect = false; }
void GLRenderThread::tm_mouseMove(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	POINT ptPrevMouseMove = m_ptMouseMove;
	m_ptMouseMove = MAKEPOINT(lParam);
	SIZE slMouseMove = SIZE({ m_ptMouseMove.x - ptPrevMouseMove.x,m_ptMouseMove.y - ptPrevMouseMove.y });

	if (m_lpLayoutManager == nullptr) return;

	if (m_bMouseInRect) {
		if (::PtInRect(m_lpLayoutManager->getActiveObject()->getActiveViewport(), WinToGL(m_ptMouseMove)) == FALSE) {
			m_bMouseInRect = false;//Cursor is not in active viewport.
		}
	}
	else {
		if (::PtInRect(m_lpLayoutManager->getActiveObject()->getActiveViewport(), WinToGL(m_ptMouseMove)) == TRUE) {
			m_bMouseInRect = true;//Cursor is in active viewport.
		}
	}

	m_lpLayoutManager->getActiveObject()->mousemove(WinToGL(m_ptMouseMove));

	//if ((GetKeyState(VK_LBUTTON) & 0x8000) != 0) {
	if (m_dwKeystate == VK_LBUTTON) {
		double dx = (double)slMouseMove.cx * c_scaleMouse;
		double dy = (double)slMouseMove.cy * c_scaleMouse;
		if (m_lpCommandManager->getActiveObject(CommandContext::VIEW) != nullptr) {
			UINT command = m_lpCommandManager->getActiveObject(CommandContext::VIEW)->getID();
			switch (command) {
				case IDC_PAN: {
					m_lpLayoutManager->getActiveSpace()->move(dx, -dy);
					break;
				}
				case IDC_ZOOM: {
					m_lpLayoutManager->getActiveSpace()->zoom(dy);
					break;
				}
				case IDC_3DDISTANCE: {
					m_lpLayoutManager->getActiveObject()->getActiveViewport()->getCamera()->setDistance(-dy);
					break;
				}
				case IDC_3DORBIT:
				case IDC_3DCORBIT:
				case IDC_3DFORBIT: {
					m_lpLayoutManager->getActiveSpace()->rotate(dy, 0.0, dx);
					break;
				}
				default: {
					break;
				}
			}
		}
		SendThreadMessage(WM_PAINT, 0, 0);
	}
	else {
		if (m_bMouseInRect == TRUE) {
			SendThreadMessage(WM_PAINT, 0, 0);
		}
	}
}
void GLRenderThread::tm_mouseWheel(WPARAM wParam, LPARAM lParam)
{
	int nWheelMove = GET_WHEEL_DELTA_WPARAM(wParam);
	double dz = (double)nWheelMove * c_scaleWheel;

	if (m_lpLayoutManager == nullptr) return;

	POINT ptMousePosition = MAKEPOINT(lParam);
	POINT ptGLMousePosition = WinToGL(ptMousePosition);

	m_lpLayoutManager->getActiveSpace()->zoom(dz);

	SendThreadMessage(WM_PAINT, 0, 0);
}
void GLRenderThread::tm_notify(WPARAM wParam, LPARAM lParam)
{
#define lpnmhdr (LPNMHDR(lParam))

	switch (lpnmhdr->idFrom) {
		case IDC_DOCUMENT: {
			log("Log::Application::wm_notify() TCN_SELCHANGE lpnm->idFrom = IDC_DOCUMENT");
			break;
		}
		case IDC_LAYOUT: {
			log("Log::Application::wm_notify() TCN_SELCHANGE lpnm->idFrom = IDC_LAYOUT");
			break;
		}
		default: {
			log("Log::Application::wm_notify() TCN_SELCHANGE lpnm->idFrom = default");
			break;
		}
	}

	NMHDR nmh;
	nmh.hwndFrom = m_hWnd;
	nmh.idFrom = IDC_LAYOUT;
	nmh.code = TCN_SELCHANGE;

	::SendMessage(lpnmhdr->hwndFrom, WM_NOTIFY, wParam, (LPARAM)&nmh);
	//::PostMessage(lpnmhdr->hwndFrom, WM_NOTIFY, wParam, (LPARAM)&nmh);
	//::PostThreadMessage(dwThreadId, WM_NOTIFY, wParam, (LPARAM)&nmh);
	m_bSwapBuffers = true;
}
void GLRenderThread::tm_paint()
{
	if (m_lpLayoutManager == nullptr) return;
	if (m_lpLayoutManager->getActiveObject() == nullptr) return;

	m_lpLayoutManager->getActiveObject()->paint();

	m_lpGLRenderContext->swapBuffers();
}
void GLRenderThread::tm_size(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	if (m_lpLayoutManager == nullptr) return;

	m_cx = GET_X_LPARAM(lParam);
	m_cy = GET_Y_LPARAM(lParam);

	if ((m_cx > 0) && (m_cy > 0)) {
		m_lpLayoutManager->size(m_cx, m_cy);
	}
}
void GLRenderThread::tm_timer(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	if (m_lpLayoutManager == nullptr) return;

	m_lpLayoutManager->getActiveObject()->timer();

	tm_paint();//Update display with new FPS value.
}
void GLRenderThread::tm_vScroll(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	if (m_lpLayoutManager == nullptr) return;

	double dy = (double)(int)lParam * c_scaleScroll;
	m_lpLayoutManager->getActiveSpace()->move(0.0, dy);

	tm_paint();
}

void GLRenderThread::SendThreadMessage(UINT uMsg, std::shared_ptr<MessageParameters> mp)
{
	std::shared_ptr<ThreadMessage> threadMsg(new ThreadMessage(uMsg, mp));

	std::unique_lock<std::mutex> lk(m_renderMutex);
	m_renderQueue.push(threadMsg);
	m_renderCV.notify_one();

	std::lock_guard<std::mutex> lkg(m_messageMutex);
}

void GLRenderThread::getSupportedExtensions()
{
	if (!GL_ARB_framebuffer_object) { //OpenGL 2.1 doesn't require this, 3.1+ does
		log("Log::Your GPU does not support framebuffer objects. Use a texture instead.");
	}
	else {
		log("Log::Your GPU supports frame buffer objects");
	}

	if (!GL_ARB_vertex_buffer_object) {
		log("Log::GL_ARB_vertex_buffer_object not supported");
	}
	else {
		log("Log::GL_ARB_vertex_buffer_object supported");
	}

	if (!GL_ARB_vertex_shader) {
		log("Log::GL_ARB_vertex_shader not supported");
	}
	else {
		log("Log::GL_ARB_vertex_shader supported");
	}

	if (!GL_ARB_shader_objects) {
		log("Log::GL_ARB_shader_objects not supported");
	}
	else {
		log("Log::GL_ARB_shader_objects supported");
	}

	if (!GL_ARB_shading_language_100) {
		log("Log::GL_ARB_shading_language_100 not supported");
	}
	else {
		log("Log::GL_ARB_shading_language_100 supported");
	}

	if (!GL_ARB_fragment_shader) {
		log("Log::GL_ARB_fragment_shader not supported");
	}
	else {
		log("Log::GL_ARB_fragment_shader supported");
	}

	if (!GL_ARB_point_parameters) {
		log("Log::GL_ARB_point_parameters not supported");
	}
	else {
		log("Log::GL_ARB_point_parameters supported");
	}

	if (!GL_EXT_texture3D) {
		log("Log::GL_EXT_texture3D not supported");
	}
	else {
		log("Log::GL_EXT_texture3D supported");
	}

	if (!GL_ARB_texture_cube_map) {
		log("Log::GL_ARB_texture_cube_map not supported");
	}
	else {
		log("Log::GL_ARB_texture_cube_map supported");
	}

	if (!GL_ARB_debug_output) {
		log("Log::GL_ARB_debug_output not supported");
	}
	else {
		log("Log::GL_ARB_debug_output supported");
	}
}

bool GLRenderThread::initVertexBuffers()
{
	bool vboReady = false;
	if (vboReady == false) {
		glExtension& extension = glExtension::getInstance();
		m_bVBOSupported = extension.isSupported("GL_ARB_vertex_buffer_object");
		log("Log::Drawing::initVertexBuffers = %d", m_bVBOSupported);
		if (m_bVBOSupported) {
			//createVertexBufferObjects();
			vboReady = true;
		}
	}
	return vboReady;
}
bool GLRenderThread::initShaders()
{
	bool glslReady = false;
	if (glslReady == false) {
		glExtension& extension = glExtension::getInstance();
		m_bGLSLSupported = extension.isSupported("GL_ARB_shader_objects");
		log("Log::Drawing::initShaders = %d", m_bGLSLSupported);
		if (m_bGLSLSupported) {
			//glslReady = createShaderPrograms();
			glslReady = true;
		}
	}
	return glslReady;
}
void GLRenderThread::initLights()
{
	//set up light colors (ambient, diffuse, specular)
	GLfloat lightKa[] = { .2f, .2f, .2f, 1.f };      //ambient light
	GLfloat lightKd[] = { .8f, .8f, .8f, 1.f };      //diffuse light
	GLfloat lightKs[] = { 1, 1, 1, 1 };               //specular light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

	//position the light in eye space
	float lightPos[4] = { 0, 0, 1, 0 };               //directional light
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0);                            //MUST enable each light source after configuration
}
void GLRenderThread::initGL()
{
	//const GLubyte *sVersion = glGetString(GL_VERSION);
	char* chGLVersion = (char*)glGetString(GL_VERSION);
	log("Log::OpenGL version %s", chGLVersion);

	char* chGLSLVersion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	log("Log::GLSL version %s", chGLSLVersion);

	//const GLubyte *szExtensions = glGetString(GL_EXTENSIONS);
	//wchar_t *szExtensions = (wchar_t*)glGetString(GL_EXTENSIONS);
	//log("Log::OpenGL extensions %s", szExtensions);

	//getSupportedExtensions();

	int contextprofileflags = 0;
	glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &contextprofileflags);

	if ((contextprofileflags & GL_CONTEXT_CORE_PROFILE_BIT) != 0) {
		log("Log::GL_CONTEXT_CORE_PROFILE_BIT  is supported");
	}
	else if ((contextprofileflags & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) != 0) {
		log("Log::GL_CONTEXT_COMPATIBILITY_PROFILE_BIT  is supported");
	}

	int contextflags = 0;
	glGetIntegerv(GL_CONTEXT_FLAGS, &contextflags);

	if ((contextflags & GL_CONTEXT_FLAG_DEBUG_BIT) != 0) {
		log("Log::GL_CONTEXT_FLAG_DEBUG_BIT is supported");
	}

	//wglSwapIntervalEXT(1);

	//glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(GLDebugMessageCallback, 0);

	glShadeModel(GL_SMOOTH);						//Shading mathod: GL_SMOOTH or GL_FLAT.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);			//4-byte pixel alignment.

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glClearColor(0.f, 0.f, 0.f, 0.f);				//Background color.
	glClearStencil(0);								//Clear stencil buffer.
	glClearDepth(1.0);								//0 is near, 1 is far.
	glDepthFunc(GL_LESS);

	initLights();

	glFrontFace(GL_CCW);							//OpenGL default value for winding is CCW.
	glPolygonMode(GL_FRONT, GL_LINES);		//Set default value for polygon mode.
}

void GLRenderThread::renderThreadProc(GLRenderContext* rendercontext, SIZE size)//Main render loop - look after it, this does eeeeeverything.
{
	m_lpGLRenderContext = rendercontext;
	m_hWnd = m_lpGLRenderContext->getHWND();

	if (m_lpGLRenderContext->activateContext() == true) {
		m_lpLayoutManager = new LayoutManager(m_lpCommandManager, &m_DataBlocks, size);

		initGL();
		tm_paint();

		NMHDR nmh;
		nmh.hwndFrom = GetParent(m_hWnd);
		nmh.idFrom = IDC_DOCUMENT;
		nmh.code = TCN_SELCHANGE;
		PostMessage(GetParent(m_hWnd), WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);//Send IDC_DOCUMENT to document to let document know it can populate layouts tabcontrol.

		while (m_bRenderLoop) {
			std::shared_ptr<ThreadMessage> tm; {
				std::unique_lock<std::mutex> lk(m_renderMutex);
				while (m_renderQueue.empty()) {
					m_renderCV.wait(lk);
				}
				if (m_renderQueue.empty()) {
					continue;
				}
				tm = m_renderQueue.front();
				m_renderQueue.pop();
			}

			//logMode(LOG_MODE_FILE);

			switch (tm->uMsg) {
				case WM_CLOSE: {
					//log("Log::GLRenderThread[%d]::renderThreadProc() WM_CLOSE", std::this_thread::get_id());
					m_bRenderLoop = false;
					break;
				}
				case WM_HSCROLL: {
					//log("Log::GLRenderThread[%d]::renderThreadProc() WM_HSCROLL", std::this_thread::get_id());
					ASSERT_TRUE(tm->lpParams != NULL);
					auto messageData = std::static_pointer_cast<MessageParameters>(tm->lpParams);
					tm_hScroll(messageData->wParam, messageData->lParam);
					break;
				}
				case WM_KEYDOWN: {
					//log("Log::GLRenderThread[%d]::renderThreadProc() WM_KEYDOWN", std::this_thread::get_id());
					ASSERT_TRUE(tm->lpParams != NULL);
					auto messageData = std::static_pointer_cast<MessageParameters>(tm->lpParams);
					tm_keyDown(messageData->wParam, messageData->lParam);
					break;
				}
				case WM_LBUTTONDBLCLK: {
					//log("Log::GLRenderThread[%d]::renderThreadProc() WM_LBUTTONDBLCLK", std::this_thread::get_id());
					ASSERT_TRUE(tm->lpParams != NULL);
					auto messageData = std::static_pointer_cast<MessageParameters>(tm->lpParams);
					tm_lButtonDblClk(messageData->wParam, messageData->lParam);
					//NMHDR nmh;
					//nmh.hwndFrom = m_hWnd;
					nmh.idFrom = IDC_LAYOUT;
					//nmh.code = TCN_SELCHANGE;
					PostMessage(GetParent(m_hWnd), WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
					break;
				}
				case WM_LBUTTONDOWN: {
					//log("Log::GLRenderThread[%d]::renderThreadProc() WM_LBUTTONDOWN", std::this_thread::get_id());
					ASSERT_TRUE(tm->lpParams != NULL);
					auto messageData = std::static_pointer_cast<MessageParameters>(tm->lpParams);
					tm_lButtonDown(messageData->wParam, messageData->lParam);
					break;
				}
				case WM_LBUTTONUP: {
					//log("Log::GLRenderThread[%d]::renderThreadProc() WM_LBUTTONUP", std::this_thread::get_id());
					ASSERT_TRUE(tm->lpParams != NULL);
					auto messageData = std::static_pointer_cast<MessageParameters>(tm->lpParams);
					tm_lButtonUp(messageData->wParam, messageData->lParam);
					break;
				}
				case WM_MOUSELEAVE: {
					//log("Log::GLRenderThread[%d]::renderThreadProc() WM_MOUSELEAVE", std::this_thread::get_id());
					tm_mouseLeave();
					break;
				}
				case WM_MOUSEMOVE: {
					//log("Log::GLRenderThread[%d]::renderThreadProc() WM_MOUSEMOVE", std::this_thread::get_id());
					ASSERT_TRUE(tm->lpParams != NULL);
					auto messageData = std::static_pointer_cast<MessageParameters>(tm->lpParams);
					tm_mouseMove(messageData->wParam, messageData->lParam);
					break;
				}
				case WM_MOUSEWHEEL: {
					//log("Log::GLRenderThread[%d]::renderThreadProc() WM_MOUSEWHEEL", std::this_thread::get_id());
					ASSERT_TRUE(tm->lpParams != NULL);
					auto messageData = std::static_pointer_cast<MessageParameters>(tm->lpParams);
					tm_mouseWheel(messageData->wParam, messageData->lParam);
					break;
				}
				case WM_NOTIFY: {
					//log("Log::GLRenderThread[%d]::renderThreadProc() WM_MOUSEWHEEL", std::this_thread::get_id());
					ASSERT_TRUE(tm->lpParams != NULL);
					auto messageData = std::static_pointer_cast<MessageParameters>(tm->lpParams);
					tm_notify(messageData->wParam, messageData->lParam);
					break;
				}
				case WM_PAINT: {
					//log("Log::GLRenderThread[%d]::renderThreadProc() WM_PAINT", std::this_thread::get_id());
					if (m_bSwapBuffers) {
						tm_paint();
						m_bSwapBuffers = false;
					}
					break;
				}
				case WM_SIZE: {
					//log("Log::GLRenderThread[%d]::renderThreadProc() WM_SIZE", std::this_thread::get_id());
					ASSERT_TRUE(tm->lpParams != NULL);
					auto messageData = std::static_pointer_cast<MessageParameters>(tm->lpParams);
					tm_size(messageData->wParam, messageData->lParam);
					break;
				}
				case WM_TIMER: {
					//log("Log::GLRenderThread[%d]::renderThreadProc() WM_TIMER", std::this_thread::get_id());
					ASSERT_TRUE(tm->lpParams != NULL);
					auto messageData = std::static_pointer_cast<MessageParameters>(tm->lpParams);
					switch (messageData->wParam) {
						case SWAPBUFFERS: {
							m_bSwapBuffers = true;//When a WM_PAINT message is sent to the main render loop, it checks the value of m_bSwapBuffers; if true, the active layout is painted and we tell OpenGL to swap buffers.
							break;
						}
						case FPSCOUNTER: {
							tm_timer(messageData->wParam, messageData->lParam);//Used to calculate and disply the actual frames per second.
							break;
						}
						default: {
							break;
						}
					}
					break;
				}
				case WM_VSCROLL: {
					//log("Log::GLRenderThread[%d]::renderThreadProc() WM_VSCROLL", std::this_thread::get_id());
					ASSERT_TRUE(tm->lpParams != NULL);
					auto messageData = std::static_pointer_cast<MessageParameters>(tm->lpParams);
					tm_vScroll(messageData->wParam, messageData->lParam);
					break;
				}
				default: {
					break;
				}
			}
			//logMode(LOG_MODE_BOTH);
		}
		m_lpGLRenderContext->releaseContext();
	}
}
void GLRenderThread::swapbuffersThreadProc()//Sends WM_TIMER message to main render loop when it's time to swap buffers.
{
	float fFrameDuration = 1000 / m_nFPS;
	int nFrameDuration = (int)fFrameDuration;
	while (m_bRenderLoop) {
		std::this_thread::sleep_for(std::chrono::milliseconds(nFrameDuration)); //sleep for m_nFrameDuration microseconds
		SendThreadMessage(WM_TIMER, (WPARAM)SWAPBUFFERS, 0);
	}
}
void GLRenderThread::calculateFPSThreadProc()//Sends WM_TIMER message to main render loop every 1 second to calculate actual frames per second
{
	while (m_bRenderLoop) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //sleep for 1 second

		if (m_lpLayoutManager == nullptr) break;

		Layout* lpActiveLayout = m_lpLayoutManager->getActiveObject();
		if (lpActiveLayout == nullptr) break;

		if (lpActiveLayout->getContext() == Layout::Context::ModelLayout) {//If the mouse is in the active space rectangle...
			SendThreadMessage(WM_TIMER, (WPARAM)FPSCOUNTER, 0);
		}
	}
}
