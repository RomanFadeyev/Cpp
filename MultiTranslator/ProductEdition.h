/********************************************************************
	created:	2004/06/05
	created:	5:6:2004   10:27
	filename: 	X:\MultiTranslator\sources\ProductEdition.h
	file path:	X:\MultiTranslator\sources
	file base:	projectedition
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Объявление версии проекта, Edition, Limitation и проч
*********************************************************************/

#define USE_INTERNAL_HIGHLIGHTER

//Версия от TSURE
#define TSURE_EDITION
//#define EDUCATIONAL_EDITION
//#define BETA_VERSION
#ifdef  TSURE_EDITION
	#define COMPANY_NAME	"TSURE"
	#define	LEGAL_COPYRIGHT	"Copyright c 2005, Department of Computer Engineering, Taganrog State University of Radio Engineering"
	#define	LEGAL_TRADEMARKS "All rights reserved, 2005"
	#define PRODUCT_NAME	"MultiTranslator"
	#define PRODUCT_VERSION 2, 8
	#define PRODUCT_VERSION_STR "2, 8"
	#define PRODUCT_AUTHORS "DCE"
	#define COMPANY_EMAIL   "polenov@dce.tsure.ru"
	#define COMPANY_URL		"www.tsure.ru"
#else
	#define COMPANY_NAME	"TSURE"
	#define	LEGAL_COPYRIGHT	"Copyright c 2005, Roman Fadeyev"
//	#define COMPANY_NAME	"SmartHead Solutions"
//	#define	LEGAL_COPYRIGHT	"Copyright c 2004, SmartHead Solutions"
	#define	LEGAL_TRADEMARKS "All rights reserved, 2005"
	#define PRODUCT_NAME	"MultiTranslator"
	#define PRODUCT_VERSION 3, 0
	#define PRODUCT_VERSION_STR "3, 0"
	#define PRODUCT_AUTHORS "Roman Fadeyev"
	#define COMPANY_EMAIL   "support@multitranslator.com"
	#define COMPANY_URL		"www.multitranslator.com"
#endif

#ifdef EDUCATIONAL_EDITION
	#define PRODUCT_VERSION_EDITION " [Educational]"
#elif defined (BETA_VERSION)
	#define PRODUCT_VERSION_EDITION " [BETA]"
#else
	#define PRODUCT_VERSION_EDITION 
#endif
