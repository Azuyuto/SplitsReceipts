# SplitsReceipts

[![N|Solid](https://raw.githubusercontent.com/Azuyuto/SplitsReceipts/master/Photos/photo1.PNG)](https://raw.githubusercontent.com/Azuyuto/SplitsReceipts/master/Photos/photo1.PNG)

Program służący do dzielenia wartości z paragonu.
> Aktualna wersja Alpha zmiena tylko zdjęcia paragonu z folderu /receipts na pliki .xls do folderu /results. Aby uruchomić program otwieramy solucję w Visual Studio.

!Uwaga wymagane jest zainstalowanie tesseract za pomocą vcpkg.

1. Instalujemy vcpkg
> cd C:/
> git clone https://github.com/microsoft/vcpkg
2. Instalujemy tesseract
> .\vcpkg\vcpkg install tesseract:x64-windows
3. Tworzymy integracje
> .\vcpkg\vcpkg integrate install
