#pragma once
#include "utils/rapidxml/rapidxml.hpp"
#include "utils/rapidxml/rapidxml_print.hpp"

#define RAPIDXML_HEAP_SIZE 256

class CSerialRapidxmlDoc
{
private:
	rapidxml::xml_document<> m_doc;

public:
	CSerialRapidxmlDoc()
	{

	}

	~CSerialRapidxmlDoc()
	{

	}

	rapidxml::xml_document<>& doc()
	{
		return m_doc;
	}

	//生成堆内存，给xml对象在添加节点时，不可添加临时变量

	char* get(bool data)
	{
		char heap[RAPIDXML_HEAP_SIZE];
		sprintf_s(heap, "%d", data ? 1:0);
		return m_doc.allocate_string(heap);
	}

	char* get(const char* text)
	{
		return m_doc.allocate_string(text);
	}

	char* get(int data)
	{
		char heap[RAPIDXML_HEAP_SIZE];
		sprintf_s(heap, "%d", data);
		return m_doc.allocate_string(heap);
	}

	char* get(unsigned data)
	{
		char heap[RAPIDXML_HEAP_SIZE];
		sprintf_s(heap, "%d", data);
		return m_doc.allocate_string(heap);
	}

	char* get(long data)
	{
		char heap[RAPIDXML_HEAP_SIZE];
		sprintf_s(heap, "%ld", data);
		return m_doc.allocate_string(heap);
	}

	char* get(long long data)
	{
		char heap[RAPIDXML_HEAP_SIZE];
		sprintf_s(heap, "%lld", data);
		return m_doc.allocate_string(heap);
	}

	char* get(float data)
	{
		char heap[RAPIDXML_HEAP_SIZE];
		sprintf_s(heap, "%f", data);
		return m_doc.allocate_string(heap);
	}

	char* get(double data)
	{
		char heap[RAPIDXML_HEAP_SIZE];
		sprintf_s(heap, "%lf", data);
		return m_doc.allocate_string(heap);
	}
};