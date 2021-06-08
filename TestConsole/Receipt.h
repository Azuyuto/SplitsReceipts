#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Product
{
public:
    double Amount;
    string Name;
    double OriginalPrice;
    double Price;
    double PricePerAmount;
    Product(string source);
};

class Receipt
{
public:
    double FullPrice;
    vector<Product> Products;
    Receipt();
    Receipt(string source);
};