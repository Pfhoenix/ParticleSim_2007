#include "includes.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	MSG msg;
	WNDCLASSEX wndclass;

	if (FindWindow(szAppName, NULL)) return 0;

	LoadSettings();

	hMainInst = hInstance;

	wndclass.cbSize = sizeof (wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = NULL;//LoadIcon(hInstance, "");
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
	wndclass.lpszMenuName = "APPMENU";
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = NULL;//LoadIcon(hInstance, "");

	if (!RegisterClassEx(&wndclass)) return 1;

	hMainWnd = CreateWindow(szAppName, szAppName, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, Width + GetSystemMetrics(SM_CXBORDER) * 2, Height + GetSystemMetrics(SM_CYBORDER) * 2 + GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION), NULL, NULL, hInstance, NULL);

	if (!hMainWnd) return 1;

	ShowWindow(hMainWnd, iCmdShow);
	mt.init_genrand(timeGetTime());

	InitEngine(hMainWnd);
	if (!RenderArea || !RenderArea->hWindow)
	{
		if (RenderArea) delete RenderArea;
		return 0;
	}
	if (!Physics)
	{
		delete RenderArea;
		return 0;
	}

	ToggleToolbox();

	DWORD tT = 0, lastTime = 0;
	NxActor *UnderMouseActor = NULL;

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// loop engine
		tT = timeGetTime();
		// to prevent first loop weirdness, skip it
		if (lastTime == 0)
		{
			lastTime = tT;
			continue;
		}
		if ((tT - lastTime) <= 0) continue;
		DeltaTime = (tT - lastTime) / 1000.0f;
		lastTime = tT;
		// skip a frame to ensure we have a good LastDeltaTime
		if (LastDeltaTime < 0.0f)
		{
			LastDeltaTime = DeltaTime;
			continue;
		}

		DeltaTime = 0.05f;

		//if (!bFreezeEngine)
		//{
			TickAll();
			Physics->Simulate(bFreezeEngine ? 0.0f : DeltaTime);
		//}
		LastDeltaTime = DeltaTime;

		RenderArea->ScreenToWorld(LastMouse.x, LastMouse.y, &Mouse3D, &MouseNormal);
		UnderMouseActor = Physics->FindActorUnderMouse(Mouse3D, MouseNormal);
		if (UnderMouseActor) UnderMouse = (GenericObject*) UnderMouseActor->userData;
		else UnderMouse = NULL;

		if (msg.message == WM_QUIT) break;

		if (RenderArea)
		{
			RenderArea->StartRenderScene(D3DCOLOR_XRGB(0,0,0));
			RenderScreen();
			RenderArea->EndRenderScene();
		}
	}

	CleanupEngine();

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static POINT p;
	D3DXVECTOR4 v0, v1;
	Vector3D<float> graboffset, tv;
	ChargedObject *CO = NULL;

	switch(iMsg)
	{
		case WM_CREATE:
			hMainMenu = GetMenu(hWnd);
			return 0;

		case WM_PAINT:
			/*if (RenderArea)
			{
				RenderArea->StartRenderScene(D3DCOLOR_XRGB(0,0,0));
				RenderScreen();
				RenderArea->EndRenderScene();
			}*/
			ValidateRect(hWnd, NULL);
			return 0;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
			LastMouse.x = LOWORD(lParam);
			LastMouse.y = HIWORD(lParam);
			Physics->PickActor(Mouse3D, MouseNormal);
			return 0;

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
			LastMouse.x = LOWORD(lParam);
			LastMouse.y = HIWORD(lParam);
			if (HeldObject) Physics->LetGoActor();
			return 0;

		case WM_RBUTTONDOWN:
			LastMouse.x = LOWORD(lParam);
			LastMouse.y = HIWORD(lParam);
			if (PlaceType)
			{
				if (PlaceType == IDC_ELECTRON) CO = new Electron;
				else if (PlaceType == IDC_POSITRON) CO = new Positron;
				else if (PlaceType == IDC_NUCLEUS) CO = new Nucleus(NucleusType);
				else if (PlaceType == IDC_MAGNET) CO = new Magnet;
				else if (PlaceType == IDC_DELETE)
				{
					if (HeldObject)
					{
						DeleteObject(HeldObject);
						Physics->Selected = NULL;
						HeldObject = NULL;
					}
					else
					{
						Physics->PickActor(Mouse3D, MouseNormal);
						if (Physics->Selected)
						{
							DeleteObject(HeldObject);
							Physics->Selected = NULL;
							HeldObject = NULL;
						}
					}
				}
				if (CO && !CO->Actor)
				{
					DeleteObject(CO);
					Log("placing actor didn't have PhysX");
				}
				else if (CO) CO->Actor->setGlobalPosition(Mouse3D);
			}
			return 0;

		/*case WM_MOUSEWHEEL:
			if ((short) HIWORD(wParam) > 0) ScaleVec *= 1.25;
			else if ((short) HIWORD(wParam) < 0) ScaleVec /= 1.25;
			bUpdateCamera = true;
			return 0;*/

		case WM_MOUSEMOVE:
			p.x = LastMouse.x;
			p.y = LastMouse.y;
			LastMouse.x = (short) LOWORD(lParam);
			LastMouse.y = (short) HIWORD(lParam);
			if (HeldObject)
			{
				if (wParam & MK_MBUTTON)// && dynamic_cast<Magnet*>(HeldObject))
				{
					RenderArea->ScreenToWorld(LastMouse.x, LastMouse.y, &Mouse3D, &MouseNormal);
					NxVec3 v = Mouse3D - HeldObject->Actor->getGlobalPosition();
					v.setMagnitude(1.0f);
					NxMat33 m = HeldObject->Actor->getGlobalOrientation();
					m.rotZ(v.y >= 0.0f ? acos(v.x) : -acos(v.x));
					HeldObject->Actor->setGlobalOrientation(m);
				}
				else if (wParam & MK_LBUTTON)
				{
					RenderArea->ScreenToWorld(LastMouse.x, LastMouse.y, &Mouse3D, &MouseNormal);
					HeldObject->Actor->setGlobalPosition(Mouse3D);
				}
			}
			/*if (wParam & MK_RBUTTON)
			{
				CameraRot.Z -= (LastMouse.x - p.x) * 0.01;
				if (CameraRot.Z > CAMERA_ROT_LIMIT) CameraRot.Z = CAMERA_ROT_LIMIT;
				else if (CameraRot.Z < -CAMERA_ROT_LIMIT) CameraRot.Z = -CAMERA_ROT_LIMIT;

				CameraRot.Y -= (LastMouse.y - p.y) * 0.01;
				if (CameraRot.Y > CAMERA_ROT_LIMIT) CameraRot.Y = CAMERA_ROT_LIMIT;
				else if (CameraRot.Y < 0.0) CameraRot.Y = 0.0;

				bUpdateCamera = true;
			}*/
			return 0;

		case WM_KEYDOWN:
			switch (wParam)
			{
				case 'G':
					bGravity = !bGravity;
					if (bGravity && Physics) Physics->gScene->setGravity(NxVec3(0.0f, 9.8f, 0.0f));
					else if (!bGravity && Physics) Physics->gScene->setGravity(NxVec3(0.0f, 0.0f, 0.0f));
					break;

				case VK_PAUSE:
					bFreezeEngine = !bFreezeEngine;
					if (hToolbox) SetDlgItemText(hToolbox, IDC_PAUSE, (bFreezeEngine ? "Unpause" : "Pause"));
					break;

				/*case VK_BACK:
					CameraLoc.SetValue(0.0, 0.0, 0.0);
					CameraRot.SetValue(0.0, 0.0, 0.0);
					ScaleVec.SetValue(1.0, 1.0, 1.0);
					bUpdateCamera = true;
					break;*/

				/*case VK_UP:
					if (bCanRenderField && bRenderField && (FieldType == FT_REAL))
					{
						RealFieldScale += 1.0f;
						if (RealFieldScale > 10.0f) RealFieldScale = 10.0f;
					}
					break;

				case VK_DOWN:
					if (bCanRenderField && bRenderField && (FieldType == FT_REAL))
					{
						RealFieldScale -= 1.0f;
						if (RealFieldScale < 1.0f) RealFieldScale = 1.0f;
					}
					break;

				case VK_LEFT:
					if (bCanRenderField && bRenderField)
					{
						FieldType -= 1;
						if (FieldType < FT_NONE) FieldType = FT_NONE;
					}
					break;

				case VK_RIGHT:
					if (bCanRenderField && bRenderField)
					{
						FieldType += 1;
						if (FieldType >= FT_ROLL) FieldType = FT_ROLL - 1;
					}
					break;*/

				/*case VK_RETURN:
					bForceSmallStep = !bForceSmallStep;
					break;*/

				case VK_BACK:
				case VK_DELETE:
					if (HeldObject)
					{
						DeleteObject(HeldObject);
						Physics->Selected = NULL;
						HeldObject = NULL;
					}
					break;

				/*case VK_INSERT:
					CameraLoc.X += 10.0 / ScaleVec.X;
					//if (CameraLoc.X > AREA_BOUND_EXTENT) CameraLoc.X = AREA_BOUND_EXTENT;
					bUpdateCamera = true;
					break;

				case VK_DELETE:
					CameraLoc.X -= 10.0 / ScaleVec.X;
					//if (CameraLoc.X < -AREA_BOUND_EXTENT) CameraLoc.X = -AREA_BOUND_EXTENT;
					bUpdateCamera = true;
					break;

				case VK_HOME:
					CameraLoc.Y -= 10.0 / ScaleVec.Y;
					//if (CameraLoc.Y < -AREA_BOUND_EXTENT) CameraLoc.Y = -AREA_BOUND_EXTENT;
					bUpdateCamera = true;
					break;

				case VK_END:
					CameraLoc.Y += 10.0 / ScaleVec.Y;
					//if (CameraLoc.Y > AREA_BOUND_EXTENT) CameraLoc.Y = AREA_BOUND_EXTENT;
					bUpdateCamera = true;
					break;

				case VK_PRIOR:
					CameraLoc.Z += 10.0 / ScaleVec.Z;
					//if (CameraLoc.Z > AREA_BOUND_EXTENT) CameraLoc.Z = AREA_BOUND_EXTENT;
					bUpdateCamera = true;
					break;

				case VK_NEXT:
					CameraLoc.Z -= 10.0 / ScaleVec.Z;
					//if (CameraLoc.Z < -AREA_BOUND_EXTENT) CameraLoc.Z = -AREA_BOUND_EXTENT;
					bUpdateCamera = true;
					break;*/
			}
			return 0;

		case WM_COMMAND:
			switch (wParam)
			{
				case ID_FILE_EXIT:
					SendMessage(hWnd, WM_CLOSE, 0, 0);
					break;

				case ID_VIEW_TOOLBOX:
					ToggleToolbox();
					break;

				case ID_HELP_HELP:
					MessageBox(hWnd, "Left mouse button will grab and move whatever object you click on - release the left mouse button to let go\n\nMiddle mouse button grabs and rotates whatever object you click on - release the middle mouse button to let go.\n\nRight mouse button places the selected object type under the mouse cursor", "Particle5 Help", MB_OK | MB_ICONQUESTION);
					break;

				case ID_HELP_ABOUT:
					MessageBox(hWnd, "Particle5 utilizes the PhysX SDK (2.7.0) for rigid-body simulation, collision detection, and response. Electric field computation and physics processing is custom. All particle and object behavior is emergent from basic physics equations - no behavior, aggregate or individual on the part of any object(s), is hard-coded in any way. All questions and comments can be sent to pfhoenix@gmail.com\n\n\t- Martin Actor", "Particle5 About", MB_OK | MB_ICONASTERISK);
					break;
			}
			return 0;

		case WM_DESTROY:
			if (hToolbox)
			{
				DestroyWindow(hToolbox);
				hToolbox = NULL;
			}
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

int CALLBACK ToolboxWndProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hColor1, hColor2, hScale;
	static CHOOSECOLOR cc;
	static COLORREF CustomColors[16];
	LPDRAWITEMSTRUCT pdis;
	HBRUSH hBrush;

	switch (iMsg)
	{
		case WM_INITDIALOG:
			CheckDlgButton(hDlg, IDC_ELECTRON, BST_CHECKED);
			PlaceType = IDC_ELECTRON;
			hColor1 = GetDlgItem(hDlg, IDC_COLOR1);
			hColor2 = GetDlgItem(hDlg, IDC_COLOR2);
			hScale = GetDlgItem(hDlg, IDC_FIELD_COLOR_SCALE);
			SendMessage(hScale, TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG(1, 10));  
			if (!bCanRenderField)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_FIELD_SUBTRACT), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_FIELD_REAL), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_FIELD_LINES), FALSE);
				EnableWindow(hColor1, FALSE);
				EnableWindow(hColor2, FALSE);
				EnableWindow(hScale, FALSE);
			}
			else if (!bRealField)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_FIELD_REAL), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_FIELD_LINES), FALSE);
			}
			cc.lStructSize = sizeof(CHOOSECOLOR);
			cc.hwndOwner = NULL;
			cc.hInstance = NULL;
			cc.rgbResult = RGB(0, 0, 0);
			cc.lpCustColors = CustomColors;
			cc.Flags = CC_RGBINIT | CC_FULLOPEN;
			cc.lCustData = 0;
			cc.lpfnHook = NULL;
			cc.lpTemplateName = NULL;
			break;

		case WM_DRAWITEM:
			pdis = (LPDRAWITEMSTRUCT) lParam;
			if (pdis->CtlID == IDC_COLOR1)
			{
				COLORREF c;
				if (FieldType == IDC_FIELD_SUBTRACT) c = RGB(PosiColor[0] * 255, PosiColor[1] * 255, PosiColor[2] * 255);
				else if ((FieldType == IDC_FIELD_REAL) || (FieldType == IDC_FIELD_LINES)) c = RGB(ForceHColor[0] * 255, ForceHColor[1] * 255, ForceHColor[2] * 255);
				else c = 0;
				hBrush = CreateSolidBrush(c);
				FillRect(pdis->hDC, &(pdis->rcItem), hBrush);
				SetBkColor(pdis->hDC, c);
				DrawText(pdis->hDC, "Color 1", -1, &(pdis->rcItem), DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				DeleteObject(hBrush);
			}
			else if (pdis->CtlID == IDC_COLOR2)
			{
				COLORREF c;
				if (FieldType == IDC_FIELD_SUBTRACT) c = RGB(NegaColor[0] * 255, NegaColor[1] * 255, NegaColor[2] * 255);
				else if ((FieldType == IDC_FIELD_REAL) || (FieldType == IDC_FIELD_LINES)) c = RGB(ForceVColor[0] * 255, ForceVColor[1] * 255, ForceVColor[2] * 255);
				else c = 0;
				hBrush = CreateSolidBrush(c);
				FillRect(pdis->hDC, &(pdis->rcItem), hBrush);
				SetBkColor(pdis->hDC, c);
				DrawText(pdis->hDC, "Color 2", -1, &(pdis->rcItem), DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				DeleteObject(hBrush);
			}
			break;

		case WM_HSCROLL:
			if ((HWND) lParam == hScale)
			{
				int il;
				il = (int) SendMessage(hScale, TBM_GETPOS, 0, 0);
				FieldScale = (float) il;
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_ELECTRON:
				case IDC_POSITRON:
				case IDC_NUCLEUS:
				case IDC_MAGNET:
				case IDC_DELETE:
					if (PlaceType == LOWORD(wParam)) PlaceType = 0;
					else
					{
						CheckDlgButton(hDlg, PlaceType, BST_UNCHECKED);
						PlaceType = LOWORD(wParam);
						if (PlaceType == IDC_NUCLEUS)
						{
							DialogBox(hMainInst, "ELEMENTSBOX", hMainWnd, ElementsDlgProc);
							if (!NucleusType)
							{
								CheckDlgButton(hDlg, PlaceType, BST_UNCHECKED);
								PlaceType = 0;
							}
						}
					}
					break;

				case IDC_FIELD_SUBTRACT:
				case IDC_FIELD_REAL:
				case IDC_FIELD_LINES:
					if (FieldType == LOWORD(wParam))
					{
						if (FieldType == IDC_FIELD_SUBTRACT)
						{
							DestroyWindow(hSFLToolbox);
							hSFLToolbox = NULL;
						}
						FieldType = 0;
					}
					else
					{
						CheckDlgButton(hDlg, FieldType, BST_UNCHECKED);
						if (FieldType == IDC_FIELD_SUBTRACT)
						{
							DestroyWindow(hSFLToolbox);
							hSFLToolbox = NULL;
						}
						FieldType = LOWORD(wParam);
						if (FieldType == IDC_FIELD_SUBTRACT)
							hSFLToolbox = CreateDialog(hMainInst, "SFLTOOLBOX", hMainWnd, &SFLToolboxWndProc);
					}
					InvalidateRect(hColor1, NULL, FALSE);
					InvalidateRect(hColor2, NULL, FALSE);
					break;
			}
			switch (wParam)
			{
				case IDC_COLOR1:
					if (!FieldType) break;
					cc.hwndOwner = hDlg;
					ChooseColor(&cc);
					if (FieldType == IDC_FIELD_SUBTRACT)
					{
						PosiColor[0] = GetRValue(cc.rgbResult) / 255.0f;
						PosiColor[1] = GetGValue(cc.rgbResult) / 255.0f;
						PosiColor[2] = GetBValue(cc.rgbResult) / 255.0f;
						RenderArea->FieldFx->SetFloatArray("g_PosiColor", PosiColor, 4);
					}
					else if ((FieldType == IDC_FIELD_REAL) || (FieldType == IDC_FIELD_LINES))
					{
						ForceHColor[0] = GetRValue(cc.rgbResult) / 255.0f;
						ForceHColor[1] = GetGValue(cc.rgbResult) / 255.0f;
						ForceHColor[2] = GetBValue(cc.rgbResult) / 255.0f;
						RenderArea->FieldFx->SetFloatArray("g_FieldHColor", ForceHColor, 4);
					}
					InvalidateRect(hColor1, NULL, TRUE);
					break;

				case IDC_COLOR2:
					if (!FieldType) break;
					cc.hwndOwner = hDlg;
					ChooseColor(&cc);
					if (FieldType == IDC_FIELD_SUBTRACT)
					{
						NegaColor[0] = GetRValue(cc.rgbResult) / 255.0f;
						NegaColor[1] = GetGValue(cc.rgbResult) / 255.0f;
						NegaColor[2] = GetBValue(cc.rgbResult) / 255.0f;
						RenderArea->FieldFx->SetFloatArray("g_NegaColor", NegaColor, 4);
					}
					else if ((FieldType == IDC_FIELD_REAL) || (FieldType == IDC_FIELD_LINES))
					{
						ForceVColor[0] = GetRValue(cc.rgbResult) / 255.0f;
						ForceVColor[1] = GetGValue(cc.rgbResult) / 255.0f;
						ForceVColor[2] = GetBValue(cc.rgbResult) / 255.0f;
						RenderArea->FieldFx->SetFloatArray("g_FieldVColor", ForceVColor, 4);
					}
					InvalidateRect(hColor1, NULL, TRUE);
					break;

				case IDC_PAUSE:
					if (!bFreezeEngine)
					{
						bFreezeEngine = true;
						SetDlgItemText(hDlg, IDC_PAUSE, "Unpause");
					}
					else
					{
						bFreezeEngine = false;
						SetDlgItemText(hDlg, IDC_PAUSE, "Pause");
					}
					break;
			}
			break;

		case WM_CLOSE:
			ToggleToolbox();
			break;
	}

	return FALSE;
}

BOOL CALLBACK ElementsDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hElements;
	int sel;
	stringstream ss;

	switch (iMsg)
	{
		case WM_INITDIALOG:
			hElements = GetDlgItem(hDlg, IDC_ELEMENTS);
			for (map<int,ELEMENTINFO*>::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				ss.str("");
				ss << it->second->Name << " (" << it->second->AtomicNumber << ")";
				ListBox_AddString(hElements, ss.str().c_str());
			}
			break;

		case WM_COMMAND:
			switch (wParam)
			{
				case IDOK:
					sel = ListBox_GetCurSel(hElements);
					if (sel != LB_ERR) NucleusType = sel + 1;
					else NucleusType = 0;
					EndDialog(hDlg, 0);
					return TRUE;

				case IDCANCEL:
					NucleusType = 0;
					EndDialog(hDlg, 0);
					return TRUE;
			}
			break;
	}

	return FALSE;
}

int CALLBACK SFLToolboxWndProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hSFLMulti, hSFLMod;

	switch(iMsg)
	{
		case WM_INITDIALOG:
			hSFLMulti = GetDlgItem(hDlg, IDC_SFL_MULTIPLIER);
			SendMessage(hSFLMulti, TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG(2, 1000));
			SendMessage(hSFLMulti, TBM_SETPOS, (WPARAM) TRUE, (LPARAM) SubFieldLinesMulti);
			hSFLMod = GetDlgItem(hDlg, IDC_SFL_MODULUS);
			SendMessage(hSFLMod, TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG(2, 100));
			SendMessage(hSFLMod, TBM_SETPOS, (WPARAM) TRUE, (LPARAM) SubFieldLinesMod);
			break;

		case WM_HSCROLL:
			// convert this
			if ((HWND) lParam == hSFLMulti)
			{
				SubFieldLinesMulti = (int) SendMessage(hSFLMulti, TBM_GETPOS, 0, 0);
			}
			else if ((HWND) lParam == hSFLMod)
			{
				SubFieldLinesMod = (int) SendMessage(hSFLMod, TBM_GETPOS, 0, 0);
			}
			break;

		case WM_COMMAND:
			switch (wParam)
			{
				case IDC_SFL_METHOD_TOGGLE:
					bSubFieldLines = !bSubFieldLines;
					if (bSubFieldLines) SetDlgItemText(hDlg, (int) wParam, "Switch to Continuous");
					else SetDlgItemText(hDlg, (int) wParam, "Switch to Lines");
					break;
			}
			break;

	}

	return FALSE;
}

