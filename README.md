# 1hrOthelloCustom
Inspired from the reversi section in a book called *小一時間でゲームをつくる*.

The content inside the book utilizes <conio.h>, which is only offered in Windows OS.
The book in also more targeted towards the use of C language, instead of C++.
Hence, I decided to make an othello game that employs more C++ features while keeping it compatible for most OS (e.g., Windows, MacOS, Linux).

The package <SFML/Graphics.hpp> is applied in this code. Please keep in mind that you will need to manually install SFML.
This code also requires a load of a specific font file (misaki_gothic_2nd.ttf), which is provided in this repository.

Run the command lines below in terminal to initiate "1hrothello.cpp".
```
g++ 1hrothello.cpp -o main -lsfml-graphics -lsfml-window -lsfml-system
./main
```
