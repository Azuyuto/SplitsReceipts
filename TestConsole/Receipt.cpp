#include "Receipt.h"
#include <iostream>
#include <string>
#include <regex>
#include <windows.h> 

using namespace std;

bool IsValidPrice(string price)
{
	int indexDotted = INT_MAX;
	bool hasNumber = false;
	for (int i = 0; i < price.size(); i++)
	{
		if (price[i] == '.' || price[i] == ',')
		{
			indexDotted = i;
			break;
		}
		else if (price[i] >= 48 && price[i] <= 57)
			hasNumber = true;
	}
	int decimalsCount = price.size() - indexDotted - 1;
	if ((decimalsCount >= 2 || indexDotted == INT_MAX) && hasNumber)
	{
		return true;
	}
	return false;
}

string ReplaceCommaToDot(string s)
{
	for (int i = 0; i < s.size(); i++)
	{
		if (s[i] == ',')
			s[i] = '.';
	}
	return s;
}

Receipt::Receipt()
{
	this->FullPrice = 0;
}

Receipt::Receipt(string source)
{
	this->FullPrice = 0;
	// 0. Get appropriate text range
	smatch m;
	regex e("PARAGON FISKALNY");
	regex_search(source, m, e);
	if (m.size() == 0)
	{
		throw std::runtime_error("[error] Cannot found expression: \"PARAGON FISKALNY\"");
	}
	smatch m2;
	regex e2("SUMA PLN");
	regex_search(source, m2, e2);
	if (m2.size() == 0)
	{
		throw std::runtime_error("[error] Cannot found expression: \"SUMA PLN\"");
	}
	string n = source.substr(m.position(0) + 17, m2.position(0) - 1 - m.position(0));
	// 1. Split text into lines 
	int si = 0;
	string poj = "";
	vector<string> v;
	while (si < n.size())
	{
		poj += n[si];
		if (n[si] == '\n')
		{
			v.push_back(poj);
			poj = "";
		}
		si++;
	}
	// 2. Set FullPrice
	smatch m5;
	regex e5("[0-9.,]+");
	regex_search(v.back(), m5, e5);
	if (m5.size() > 0)
	{
		this->FullPrice = stod(ReplaceCommaToDot(m5[0]));
	}
	// 3. Delete unnecessary lines
	for (int i = 0; i < v.size(); i++)
	{
		smatch m7;
		regex e7("SPRZED");
		regex_search(v.back(), m7, e7);
		if (m7.size() > 0)
		{
			while (i < v.size() - 1)
			{
				v.pop_back();
			}
		}
	}
	// 4. Set products
	for (int i = 0; i < v.size(); i++)
	{
		smatch m6;
		regex e6("([0-9., ]+x[0-9,. ]+ [0-9,. ]+)");
		regex_search(v[i], m6, e6);
		if (m6.size() > 0)
		{
			// 5. Set product
			Product* temp = new Product(v[i]);
			this->Products.push_back(*temp);
		}
		else
		{
			// 5. Set discount
			smatch m3;
			regex e3("Rabat");
			regex_search(v[i], m3, e3);
			if (m3.size() > 0)
			{
				smatch m4;
				regex e4("[0-9.,]+");
				int nextRow = i + 1;
				regex_search(v[nextRow], m4, e4);
				if (m4.size() > 0)
				{
					this->Products.back().Price = stod(ReplaceCommaToDot(m4[0]));
				}
				i++;
			}
		}
	}
}

Product::Product(string source)
{
	this->OriginalPrice = 0;
	this->Price = 0;
	this->PricePerAmount = 0;
	this->Amount = 0;
	smatch m;
	regex e("([0-9., ]+x[0-9,. ]+ [0-9,. ]+)");
	regex_search(source, m, e);
	if (m.size() > 0)
	{
		// 0. Name
		string name = source.substr(0, m.position(0) - 2);
		this->Name = name;
		string prices = source.substr(m.position(0));
		vector<string> values;
		string temp;
		for (int i = 0; i < prices.size(); i++)
		{
			if ((prices[i] >= 48 && prices[i] <= 57) || prices[i] == '.' || prices[i] == ',')
			{
				temp += prices[i];

			}
			else if (IsValidPrice(temp))
			{
				values.push_back(temp);
				temp = "";
			}
		}
		// 1. Set OriginalPrice & Price
		if (values.size() > 0)
		{
			this->OriginalPrice = stod(ReplaceCommaToDot(values.back()));
			this->Price = stod(ReplaceCommaToDot(values.back()));
			values.pop_back();
		}
		// 2. Set PricePerAmount
		if (values.size() > 0)
		{
			this->PricePerAmount = stod(ReplaceCommaToDot(values.back()));
			values.pop_back();
		}
		// 3. Set Amount
		if (values.size() > 0)
		{
			this->Amount = stod(ReplaceCommaToDot(values.back()));
			values.pop_back();
		}
	}
}