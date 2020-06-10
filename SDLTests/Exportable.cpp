#include "Exportable.h"

namespace NodeEdit
{
	//ExportInput CreateExportInput(Input* src)
	//{
	//	ExportInput newIn;
	//	if (src->link)
	//	{
	//		
	//		newIn.ParentNode = new ExportableNode(src->ParentNode);
	//		//newIn.Label = src->Label;
	//		newIn.port = src->port;
	//		newIn.type = src->type;
	//		newIn.link = CreateExportOutput(src->link);
	//	}
	//	return newIn;
	//}
	//
	//ExportOutput CreateExportOutput(Output* src)
	//{
	//	ExportOutput newOut;
	//	//DONT CREATE LINKS FROM HERE TO PREVENT MASSIVE RECURSION - THEY MUST BE RESOLVED ON IMPORT
	//	if (src->link)
	//	{
	//		newOut.ParentNode = new ExportableNode(src->ParentNode);
	//		//newOut.Label = src->Label;
	//		newOut.port = src->port;
	//		newOut.type = src->type;
	//
	//	}
	//	
	//	return newOut;
	//}
}