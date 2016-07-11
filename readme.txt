Задание 1.

Разработать простейший WEB server, отвечающий следующим требованиям:

1) Возвращает статический HTML контент (CGI не нужны).

2) Однопроцессный, но многопоточный.

3) Рассчитан на большое количество одновременных запросов.

4) Корректно возвращает HTTP коды ошибок и заголовки.

5) Никаких стронних библиотек - только STL, posix, glibc

6) Сервер должен компилироваться и запускаться под Linux.


---
Проверял сборку под Windows:
- Microsoft Visual Studio
- mingw32-g++.exe (GCC) 3.4.2 (mingw-special)

Не проверял сборку под Linux, файл Makefile.unix
Под рукой нет, думаю проблем быть не должно.

Александр Сергеевич Киселев
Skype: kas_ru
http://alexanderkiselev.moikrug.ru/
http://www.linkedin.com/in/akiselev

TODO:
- URL Encoding not implemented RFC1738
- not implemented SIGINT, SIGTERM signal action
