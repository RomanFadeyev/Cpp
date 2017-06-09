// MTClient.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include <string>
#include <memory>

using namespace std;

bool OpenDialog(string & result) 
{
	char openName[MAX_PATH]; *openName = '\0';
	OPENFILENAME ofn = {/*sizeof(OPENFILENAME)*/OPENFILENAME_SIZE_VERSION_400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	ofn.hInstance = GetModuleHandle(NULL);
	ofn.lpstrFile = openName;
	ofn.nMaxFile = sizeof(openName);
	ofn.lpstrFilter = "Project files (*.pjt)\x00*.pjt\x00";
	ofn.lpstrTitle = "Open Project File";

	if (::GetOpenFileName(&ofn)) {
		result=openName;
		return true;
	} 
	return false;
}

#define PAUSE getchar()

int _tmain(int argc, _TCHAR* argv[])
{

	CoInitialize(0);

	//Create the instance of Multitranslator
	cout<<"Connecting to server... press enter..."; PAUSE;
	mtdev::IApplicationPtr * pspApplication=NULL;
	try{
		pspApplication = new mtdev::IApplicationPtr (__uuidof(mtdev::Application));
	}
	catch (...) {
	}
	
	if (pspApplication==NULL) {
		cout<<"Server not found. Run mtdev.exe in order to register server.";
		return 0;
	}
	
	std::auto_ptr<mtdev::IApplicationPtr> spApplication(pspApplication);

	//Show the main window of MT
	cout<<"Showing the program... press enter..."; PAUSE;
	(*spApplication)->Visible=true;

	//Open a project 
	string strProjectPath;
	cout<<"Choose path in open dialog: ";
	if (OpenDialog(strProjectPath))
	{
		cout<<"\nOpening project '"<<strProjectPath.c_str()<<"'..."; PAUSE;
		mtdev::IDocumentPtr spDoc=NULL;
		mtdev::IProjectDocumentPtr spProjectDoc=NULL;
		try{
			spDoc = (*spApplication)->OpenDocument(strProjectPath.c_str());
		}
		catch (...) {
		}
		if (spDoc.GetInterfacePtr()!=NULL)
			spDoc.QueryInterface(__uuidof(mtdev::IProjectDocument),&spProjectDoc);

		if (spProjectDoc==NULL)
		{
			cout<<"Cannot open specified file. Exiting....";
			return 0;
		}

		cout<<"TargetPath: "<<(const char*)spProjectDoc->TargetPath<<endl; 
		cout<<"Params: "<<(const char*)spProjectDoc->Params<<endl;

		//Build project
		cout<<"Press enter to start building project..."; PAUSE;
		if (!spProjectDoc->Build()){
			cout<<"Error occured during building the project"; 
		}
		else{
			//Run the project
			cout<<"Press enter to start running project..."; PAUSE;
			long nExitCode;
			spProjectDoc->Run(&nExitCode);
		}

		if (spProjectDoc->Modified){
			cout<<"The project has been modified. Press enter to save changes..."; PAUSE;
			spProjectDoc->Save();
		}
	}


	cout<<"Press enter to unload server..."; PAUSE;
	(*spApplication)->Quit(TRUE);

	return 0;
}

