#include <ProductEdition.h>

//Edition and Limitations ------------------------------------------------
//Для TSURE-версии здесь необходимо указать команды, которые будут недоступны
//Отсутствуют многие возможности, как например
//визуальные грамматики, генерация грамматик, Knowledge Base и проч

#ifdef TSURE_EDITION
	#define CHECK_COMMAND_ACCESS(id) (	(id==ID_TOOLS_VIEW_KNOWLEDGE_BASE) ? 0 :\
										(id==ID_TOOLS_GRAMMATICAL_INFERENCE) ? 0 :\
										(id==ID_TOOLS_IMPORT_GRAMMAR) ? 0 :\
										(id==ID_PROJECT_ADD_TO_KNOWLEDGEBASE) ? 0 :\
										(id==ID_PROJECT_EXPORT_TO_XBNF) ? 0 :\
										(id==ID_TOOLS_STATISTIC) ? 0 :\
										(id==ID_PROJECT_ADD_CODE_TEMPLATE) ? 0 :\
									id)
#else
	#define CHECK_COMMAND_ACCESS(id) id
#endif

// update ui's are listed as WM_COMMAND's so they get routed like commands
#undef ON_UPDATE_COMMAND_UI
#define ON_UPDATE_COMMAND_UI(id, memberFxn) \
	{ WM_COMMAND, CN_UPDATE_COMMAND_UI, (WORD)CHECK_COMMAND_ACCESS(id), (WORD)CHECK_COMMAND_ACCESS(id), AfxSigCmdUI, \
	(AFX_PMSG) \
	(static_cast< void (AFX_MSG_CALL CCmdTarget::*)(CCmdUI*) > \
	(memberFxn)) },

#undef ON_COMMAND
#define ON_COMMAND(id, memberFxn) \
	{ WM_COMMAND, CN_COMMAND, (WORD)CHECK_COMMAND_ACCESS(id), (WORD)CHECK_COMMAND_ACCESS(id), AfxSigCmd_v, \
	static_cast<AFX_PMSG> (memberFxn) },


/*
inline void AdjustCommandToEdition(CCmdUI *pCmdUI){
#ifdef TSURE_EDITION
	pCmdUI->Enable(false);
#endif
}
*/

#ifdef TSURE_EDITION
	#define BEGIN_LIMITED_EDITION if (false) {
	#define END_LIMITED_EDITION 	};
#else
	#define BEGIN_LIMITED_EDITION
	#define END_LIMITED_EDITION
#endif

