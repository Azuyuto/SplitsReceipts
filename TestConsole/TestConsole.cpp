#include <iostream>
#include <map>
#include <string>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <fstream> 
#include <ctime>
#include "Receipt.h"
#include "libxl.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <windows.h> 
#include <vector>

using namespace std;
using namespace libxl;

string pol(string x)
{
	int n = x.size();
	x += " ";
	map <string, string> doubleMapa;
	map <string, string> singleMapa;
	doubleMapa["Ä„"] = "A";
	doubleMapa["Ä†"] = "C";
	doubleMapa["Ä‡"] = "C";
	doubleMapa["Ĺ›"] = "S";
	doubleMapa["ĹĽ"] = "Z";
	doubleMapa["Ł»"] = "Z";
	doubleMapa["Ĺş"] = "Z";
	doubleMapa["Ăł"] = "O";
	doubleMapa["Ĺ„"] = "N";
	doubleMapa["Ä…"] = "a";
	doubleMapa["Ä™"] = "e";
	doubleMapa["Ĺ›"] = "s";
	doubleMapa["ĹĽ"] = "z";
	doubleMapa["Ĺş"] = "z";
	doubleMapa["Ăł"] = "o";
	doubleMapa["Ĺ„"] = "n";
	doubleMapa["Ăł"] = "o";
	singleMapa["Ĺ‚"] = "L";
	singleMapa["Ĺﾂ"] = "L";

	singleMapa["Ä"] = "E";
	singleMapa["Ĺ"] = "L";
	string s = "";
	for (int i = 0; i < n; i++) {
		string doubleS = string(1, x[i]) + string(1, x[i + 1]);
		string singleS = string(1, x[i]);
		if (doubleMapa.find(doubleS) != doubleMapa.end())
		{
			s += doubleMapa[doubleS];
			i++;
		}
		else if (singleMapa.find(singleS) != singleMapa.end())
		{
			s += singleMapa[singleS];
		}
		else
		{
			s += string(1, x[i]);
		}
	}
	return s;
}

string GetCurrentTimeForFileName()
{
	auto time = std::time(nullptr);
	stringstream ss;
	ss << put_time(localtime(&time), "%F_%T");
	auto s = ss.str();
	replace(s.begin(), s.end(), ':', '-');
	return s;
}

wstring GetWString(string s, bool isDouble = false)
{
	wstring w;
	for (int i = 0; i < s.length(); ++i)
		if (s[i] == '.' && isDouble)
			w += wchar_t(',');
		else
			w += wchar_t(s[i]);
	return w;
}

string DoubleToString(double l, int precision)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << l;
	std::string s = stream.str();
	return s;
}

void SetConsoleColor(int consoleColor)
{
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, consoleColor);
}

void WriteConsole(string text, int consoleColor)
{
	SetConsoleColor(consoleColor);
	cout << text << "\n";
}

vector<string> list_directory(const string& directory)
{
	WIN32_FIND_DATAA findData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	string full_path = directory + "\\*";
	vector<string> dir_list;

	hFind = FindFirstFileA(full_path.c_str(), &findData);

	if (hFind == INVALID_HANDLE_VALUE)
		throw runtime_error("Invalid handle value! Please check your path...");

	while (FindNextFileA(hFind, &findData) != 0)
	{
		dir_list.push_back(string(findData.cFileName));
	}

	FindClose(hFind);
	return dir_list;
}

int main()
{
	vector <string> files;
	WriteConsole("Hello user,\nPut photos of your receipts into the folder /receipts and expect them in excel format in a folder /results :)", 15);
	WriteConsole("Get files from folder /receipts ...", 10);
	try
	{
		files = list_directory("./receipts");
	}
	catch (exception e)
	{
		SetConsoleColor(12);
		cout << e.what() << "\n";
		SetConsoleColor(7);
		exit(1);
	}
	WriteConsole("Get files from folder /receipts... [success]", 10);
	WriteConsole("Found " + std::to_string(files.size() - 1) + " files", 3);

	for (int i = 1; i < files.size(); i++)
	{
		string input_image = "./receipts/" + files[i];
		WriteConsole("Start converting " + input_image, 3);
		const char* datapath = "./tessdata";
		string outText;

		tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
		if (api->Init(datapath, "pol_best")) {
			WriteConsole("[error] Could not initialize tesseract.", 12);
			exit(1);
		}

		Pix* image = pixRead(input_image.c_str());
		api->SetImage(image);
		WriteConsole("Convert image to text...", 10);
		WriteConsole("...", 7);
		outText = api->GetUTF8Text();
		outText = pol(outText);
		WriteConsole("Convert image to text... [success]", 10);
		WriteConsole("Convert text to class...", 10);
		Receipt* r = new Receipt();
		try
		{
			r = new Receipt(outText);
		}
		catch (exception e)
		{
			SetConsoleColor(12);
			cout << e.what() << "\n";
			SetConsoleColor(7);
			api->End();
			pixDestroy(&image);
			exit(1);
		}
		WriteConsole("Convert text to class... [success]", 10);
		api->End();
		pixDestroy(&image);

		WriteConsole("Creating excel file...", 10);
		Book* book = xlCreateBook();
		if (book)
		{
			Sheet* sheet = book->addSheet(L"Sheet1");
			if (sheet)
			{
				sheet->setCol(0, 0, 30);
				sheet->setCol(2, 2, 15);
				Color colors[] = { COLOR_LIGHTTURQUOISE, COLOR_PALEBLUE };
				Format* format1 = book->addFormat();
				format1->setFillPattern(FILLPATTERN_SOLID);
				format1->setPatternForegroundColor(colors[0]);

				Format* format2 = book->addFormat();
				format2->setFillPattern(FILLPATTERN_SOLID);
				format2->setPatternForegroundColor(colors[1]);

				sheet->writeStr(2, 0, L"Przedmiot", format1);
				sheet->writeStr(2, 1, L"Ilosc", format1);
				sheet->writeStr(2, 2, L"Cena za sztuke/1", format1);
				sheet->writeStr(2, 3, L"Cena", format1);
				for (int i = 0; i < r->Products.size(); i++)
				{
					sheet->writeStr(3 + i, 0, GetWString(r->Products[i].Name).c_str());
					sheet->writeNum(3 + i, 1, r->Products[i].Amount);
					sheet->writeNum(3 + i, 2, floor(r->Products[i].PricePerAmount * 100.0) / 100.0);
					sheet->writeNum(3 + i, 3, floor(r->Products[i].Price * 100.0) / 100.0);
				}
				sheet->writeStr(3 + r->Products.size(), 0, L"Suma przedmiotow", format1);
				sheet->writeStr(3 + r->Products.size(), 1, L"", format1);
				sheet->writeStr(3 + r->Products.size(), 2, L"", format1);
				wstring expr = GetWString("SUM(D4:D" + std::to_string(3 + r->Products.size()) + ')');
				sheet->writeFormula(3 + r->Products.size(), 3, expr.c_str(), format1);

				sheet->writeStr(4 + r->Products.size(), 0, L"Suma z paragonu", format2);
				sheet->writeStr(4 + r->Products.size(), 1, L"", format2);
				sheet->writeStr(4 + r->Products.size(), 2, L"", format2);
				sheet->writeNum(4 + r->Products.size(), 3, r->FullPrice, format2);
			}
			WriteConsole("Creating excel file... [success]", 10);
			string fileName = "results/receipt_" + GetCurrentTimeForFileName() + ".xls";
			WriteConsole("Save file as " + fileName + " ...", 10);
			wstring wFileName = GetWString(fileName);
			book->save(wFileName.c_str());
			book->release();
			WriteConsole("Save file as " + fileName + " ... [success]", 10);
		}
		r->Products.clear();
		delete r;
	}

	WriteConsole("[End] See folder /results", 2);
	SetConsoleColor(7);
	return EXIT_SUCCESS;
}

