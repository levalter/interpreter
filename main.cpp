#include <iostream>
#include <string>

#include "Lex.h"
#include "Scanner.h"
#include "Interpretator.h"

using namespace std;

//Таблица служебных слов
const char *
        Scanner::TW    [] = { "", "and", "begin", "boolean", "do", "else", "end", "if", "false", "integer", "not", "or", "program",
                              "read", "then", "true", "var", "while", "write", "procedure", "function", "array", "of", NULL };

//Таблица разделителей
const char *
        Scanner::TD    [] = {".", ";", ",", ":", ":=", "(", ")", "=", "<", ">", "+", "-", "*", "/", "<=", "!=", ">=", "[", "]", NULL };

/*==========================================================================
 *----------------------------------Main------------------------------------
 *==========================================================================
 * Запуск интерпретации, вывод сообщений об ошибке
 *==========================================================================
 */
int main () {
    try {
        Interpretator I ( "pascal.txt"  );
        I.interpretation ();
        return 0;
    }

    //лексическая ошибка
    catch ( char c ) {
        cout << "unexpected symbol " << c << endl;
        return 1;
    }

    //синтаксическая ошибка
    catch ( Lex l ) {
        cout << "unexpected lexeme" << l << endl;
        return 1;
    }

    //семантическая ошибка, ошибка выполнения
    catch ( const char *source ) {
        cout << source << endl;
        return 1;
    }
}