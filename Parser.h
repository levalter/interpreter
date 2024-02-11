#ifndef PASCAL_PARSER_H
#define PASCAL_PARSER_H

#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>
#include <algorithm>

#include "Lex.h"
#include "Ident.h"
#include "Scanner.h"
using namespace std;

/*==========================================================================
 * Шаблон функции from_st. Достать из стека и положить в переменную i
 *==========================================================================
 */
template <class T, class T_EL>
void from_st ( T & st, T_EL & i ) {
    i = st.top(); st.pop();
}

/*==========================================================================
 *------------------------------Класс Parser--------------------------------
 *==========================================================================
 * Проверка синтаксиса и семантический анализ программы, рекурсивный спуск.
 *==========================================================================
 */
class Parser {
    Lex          curr_lex;  //текущая лексема
    type_of_lex  c_type;
    int          c_val;
    string       curr_proc_name;
    Scanner      scan;
    stack < int >           st_int;
    stack < type_of_lex >   st_lex;
    void  P();
    void  D1();
    void  OD();
    void  OP();
    void  OF();
    void  DP();
    void  D();
    void  B();
    void  S();
    void  E();
    void  E1();
    void  T();
    void  F();
    void  dec ( type_of_lex type);
    void  proc_dec ( type_of_lex type);
    void  check_id ();
    void  check_op ();
    void  check_not ();
    void  eq_type ();
    void  eq_bool ();
    void  check_id_in_read ();
    void  gl () {
        curr_lex  = scan.get_lex ();
        c_type    = curr_lex.get_type ();
        c_val     = curr_lex.get_value ();
    }
public:
    vector <Lex> poliz;
    Parser ( const char *program ) : scan (program) { }
    void  analyze();
};

/*==========================================================================
 * Синтаксический анализ
 *==========================================================================
 */
void Parser::analyze () {
    gl ();
    P ();
    if (c_type != LEX_FIN)
        throw curr_lex;
    //for_each( poliz.begin(), poliz.end(), [](Lex l){ cout << l; });
    int i=0;
    cout << "POLIZ" << endl;
    for ( Lex l : poliz )
        cout << i++ << "| " << l;
    cout << "TID" << endl;
    for (int j=0; j<TID.size(); j++) {
        cout << j << "| " << TID[j].get_name() << " " << TID[j].get_type() << " " << TID[j].get_value() << " " << TID[j].get_proc_name() << endl;
    }
    cout << endl << "Yes!!!" << endl;
}

void Parser::P () {
    int beg_proc;
    //cout << "_P_" << endl;
    if ( c_type == LEX_PROGRAM ) {
        gl ();
    }
    else
        throw curr_lex;

    beg_proc = poliz.size(); // = 0
    poliz.push_back ( Lex ( ) );
    poliz.push_back ( Lex ( POLIZ_GO ) );

    D1 ();
    if ( c_type == LEX_SEMICOLON )
        gl ();
    else
        throw curr_lex;

    poliz[beg_proc] = Lex(POLIZ_LABEL, poliz.size());

    B ();
}

void Parser::D1 () {
    //cout << "_D1_" << endl;
    if ( c_type == LEX_VAR ) {
        OD();
    }
    else if ( c_type == LEX_PROCEDURE ) {
        OP();
        while ( c_type == LEX_COMMA ) {
            gl ();
            D1 ();
        }
    }
    else if ( c_type == LEX_FUNCTION ) {
        OF();
        while ( c_type == LEX_COMMA ) {
            gl ();
            D1 ();
        }
    }
    else if ( c_type == LEX_SEMICOLON ) ;
    else
        throw curr_lex;
}

void Parser::OD () {
    //cout << "_D1_" << endl;
    if ( c_type == LEX_VAR ) {
        gl ();
        D ();
        while ( c_type == LEX_COMMA ) {
            gl ();
            D ();
        }
    }
    else
        throw curr_lex;
}

void Parser::OP () {
    if ( c_type == LEX_PROCEDURE ) {
        gl ();
        if ( c_type != LEX_ID )
            throw curr_lex;
        st_int.push(c_val);
        dec ( LEX_PID );
        gl ();
        if ( c_type != LEX_LPAREN )
            throw curr_lex;
        gl ();
        if ( c_type == LEX_RPAREN ) ;
        else if ( c_type == LEX_ID ) {
            DP();
            while ( c_type == LEX_COMMA ) {
                gl ();
                DP ();
            }
            if ( c_type != LEX_RPAREN )
                throw curr_lex;
        }
        else
            throw curr_lex;
        gl ();
        if ( c_type != LEX_SEMICOLON )
            throw curr_lex;
        gl ();
        OD();
        if ( c_type != LEX_SEMICOLON )
            throw curr_lex;
        gl ();
        B();
        //st_proc.push(poliz.size());
        //cout<<"size="<<poliz.size()<<endl;
        //poliz.push_back ( Lex ( ) );
        poliz.push_back ( Lex ( POLIZ_GO ) );
    }
    else
        throw curr_lex;
}

void Parser::OF () {
    if ( c_type == LEX_FUNCTION ) {
        gl ();
        if ( c_type != LEX_ID )
            throw curr_lex;
        st_int.push(c_val);
        dec ( LEX_FID );
        gl ();
        if ( c_type != LEX_LPAREN )
            throw curr_lex;
        gl ();
        if ( c_type == LEX_RPAREN ) ;
        else if ( c_type == LEX_ID ) {
            DP();
            while ( c_type == LEX_COMMA ) {
                gl ();
                DP ();
            }
            if ( c_type != LEX_RPAREN )
                throw curr_lex;
        }
        else
            throw curr_lex;
        gl ();
        if ( c_type != LEX_COLON )
            throw curr_lex;
        gl ();
        if ( c_type == LEX_INT ) {
            gl ();
        }
        else if ( c_type == LEX_BOOL ) {
            gl ();
        }
        else
            throw curr_lex;
        if ( c_type != LEX_SEMICOLON )
            throw curr_lex;
        gl ();
        OD();
        if ( c_type != LEX_SEMICOLON )
            throw curr_lex;
        gl ();
        B();
        //st_proc.push(poliz.size());
        //cout<<"size="<<poliz.size()<<endl;
        //poliz.push_back ( Lex ( ) );
        poliz.push_back ( Lex ( POLIZ_GO ) );
    }
    else
        throw curr_lex;
}

void Parser::DP () {
    //cout << "_D_" << endl;
    if ( c_type != LEX_ID )
        throw curr_lex;
    else {
        st_int.push ( c_val );
        gl ();
        while ( c_type == LEX_COMMA ) {
            gl ();
            if ( c_type != LEX_ID )
                throw curr_lex;
            else {
                st_int.push ( c_val );
                gl ();
            }
        }
        if ( c_type != LEX_COLON )
            throw curr_lex;
        else {
            gl ();
            if ( c_type == LEX_INT ) {
                proc_dec ( LEX_INT );
                gl ();
            }
            else
            if ( c_type == LEX_BOOL ) {
                proc_dec ( LEX_BOOL );
                gl ();
            }
            else
                throw curr_lex;
        }
    }
}

void Parser::D () {
    vector <string> name_id;
    string array_name;
    //cout << "_D_" << endl;
    if ( c_type != LEX_ID )
        throw curr_lex;
    else {
        st_int.push ( c_val );
        name_id.push_back(TID[c_val].get_name());
        gl ();
        while ( c_type == LEX_COMMA ) {
            gl ();
            if ( c_type != LEX_ID )
                throw curr_lex;
            else {
                st_int.push ( c_val );
                name_id.push_back(TID[c_val].get_name());
                gl ();
            }
        }
        if ( c_type != LEX_COLON )
            throw curr_lex;
        else {
            gl ();
            if ( c_type == LEX_INT ) {
                dec ( LEX_INT );
                gl ();
            }
            else if ( c_type == LEX_BOOL ) {
                dec ( LEX_BOOL );
                gl ();
            }
            else if ( c_type == LEX_ARRAY ) {
                gl ();
                if ( c_type != LEX_LMAS )
                    throw curr_lex;
                gl ();
                if ( c_type != LEX_NUM )
                    throw curr_lex;

                for (int j=0; j<name_id.size(); j++) {
                    for (int i = 0; i < c_val; i++) {
                        TID.push_back(Ident(name_id[j]));
                    }
                }

                gl ();
                if ( c_type != LEX_RMAS )
                    throw curr_lex;
                gl ();
                if ( c_type != LEX_OF )
                    throw curr_lex;
                gl ();
                if ( c_type == LEX_INT ) {
                    cout << "INT ARRAY!!!" << endl;
                    dec ( LEX_ARR );
                    gl ();
                }
                else if ( c_type == LEX_BOOL ) {
                    cout << "BOOL ARRAY!!!" << endl;
                    dec ( LEX_ARR );
                    gl ();
                }
                else curr_lex;
            }
            else
                throw curr_lex;
        }
    }
}

void Parser::B () {
    //cout << "_B_" << endl;
    if ( c_type == LEX_BEGIN ) {
        gl ();
        if ( c_type == LEX_END ) {
            gl ();
        }
        else {
            S ();
            while ( c_type == LEX_SEMICOLON ) {
                gl ();
                S ();
            }
            if ( c_type == LEX_END ) {
                gl ();
            }
            else {
                throw curr_lex;
            }
        }
    }
    else
        throw curr_lex;
}

void Parser::S () {
    //cout << "_S_" << endl;
    int pl0, pl1, pl2, pl3;
    int proc_val;

    if ( c_type == LEX_IF ) {
        gl ();
        E ();
        eq_bool ();
        pl2 = poliz.size();
        poliz.push_back ( Lex() );
        poliz.push_back ( Lex(POLIZ_FGO) );
        if ( c_type == LEX_THEN ) {
            gl ();
            S ();
            pl3 = poliz.size ();
            poliz.push_back ( Lex () );

            poliz.push_back ( Lex ( POLIZ_GO ) );
            poliz[pl2] = Lex ( POLIZ_LABEL, poliz.size() );

            if ( c_type == LEX_ELSE ) {
                gl ();
                S ();
                poliz[pl3] = Lex ( POLIZ_LABEL, poliz.size() );
            }
            else
                throw curr_lex;
        }
        else
            throw curr_lex;
    }//end if
    else if ( c_type == LEX_WHILE ) {
        pl0 = poliz.size ();
        gl ();
        E ();
        eq_bool ();
        pl1 = poliz.size ();
        poliz.push_back ( Lex () );
        poliz.push_back ( Lex (POLIZ_FGO) );
        if ( c_type == LEX_DO ) {
            gl();
            S();
            poliz.push_back ( Lex ( POLIZ_LABEL, pl0 ) );
            poliz.push_back ( Lex ( POLIZ_GO) );
            poliz[pl1] = Lex ( POLIZ_LABEL, poliz.size() );
        }
        else
            throw curr_lex;
    }//end while
    else if ( c_type == LEX_READ ) {
        gl ();
        if ( c_type == LEX_LPAREN ) {
            gl ();
            if ( c_type == LEX_ID ) {
                check_id_in_read ();
                poliz.push_back ( Lex( POLIZ_ADDRESS, c_val) );
                gl();
            }
            else
                throw curr_lex;
            if ( c_type == LEX_RPAREN ) {
                gl ();
                poliz.push_back ( Lex (LEX_READ) );
            }
            else
                throw curr_lex;
        }
        else
            throw curr_lex;
    }//end read
    else if ( c_type == LEX_WRITE ) {
        gl ();
        if ( c_type == LEX_LPAREN ) {
            gl ();
            E ();
            if ( c_type == LEX_RPAREN ) {
                gl ();
                poliz.push_back ( Lex ( LEX_WRITE ) );
            }
            else
                throw curr_lex;
        }
        else
            throw curr_lex;
    }//end write
    else if ( c_type == LEX_ID && TID[c_val].get_type() != LEX_PID) {
        check_id ();
        poliz.push_back (Lex ( POLIZ_ADDRESS, c_val ) );
        gl();
        if ( c_type == LEX_ASSIGN ) {
            gl ();
            E ();
            eq_type ();
            poliz.push_back ( Lex ( LEX_ASSIGN ) );
        }
        else
            throw curr_lex;
    }//assign-end
    else if ( c_type == LEX_ID && TID[c_val].get_type() == LEX_PID) {
        //cout << "procedure..." << endl;
        //cout << "__TID\n" << TID[c_val].get_name() << " " << TID[c_val].get_type() << " " << TID[c_val].get_value() << endl;
        poliz.push_back ( Lex(POLIZ_LABEL, poliz.size()+3) );
        //poliz.push_back ()
        proc_val = c_val;
        gl ();
        if ( c_type == LEX_LPAREN ) {
            gl ();
            if ( c_type == LEX_RPAREN ) ;
            else if ( c_type == LEX_ID || c_type == LEX_NUM) {
                //DP();
                if (c_type == LEX_ID)
                    poliz.push_back ( Lex ( LEX_ID, c_val ) );
                else
                    poliz.push_back ( Lex ( LEX_NUM, c_val ) );
                gl ();
                while ( c_type == LEX_COMMA ) {
                    gl ();
                    if (c_type == LEX_ID)
                        poliz.push_back ( Lex ( LEX_ID, c_val ) );
                    else
                        poliz.push_back ( Lex ( LEX_NUM, c_val ) );
                    gl ();
                    //DP ();
                }
                if ( c_type != LEX_RPAREN )
                    throw curr_lex;
            }
            else
                throw curr_lex;
            gl ();
        }
        else
            throw curr_lex;

        poliz.push_back ( Lex(POLIZ_LABEL, TID[proc_val].get_value()) );
        poliz.push_back ( Lex ( POLIZ_GO ) );
    }
    else
        B();
}

void Parser::E () {
    //cout << "_E_" << endl;
    E1 ();
    if ( c_type == LEX_EQ  || c_type == LEX_LSS || c_type == LEX_GTR ||
         c_type == LEX_LEQ || c_type == LEX_GEQ || c_type == LEX_NEQ ) {
        st_lex.push ( c_type );
        gl ();
        E1 ();
        check_op ();
    }
}

void Parser::E1 () {
    //cout << "_E1_" << endl;
    T ();
    while ( c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_OR) {
        st_lex.push ( c_type );
        gl ();
        T ();
        check_op ();
    }
}

void Parser::T () {
    //cout << "_T_" << endl;
    F ();
    while ( c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_AND) {
        st_lex.push ( c_type );
        gl ();
        F ();
        check_op ();
    }
}

void Parser::F () {
    int proc_val;
    //cout << "_F_" << endl;
    if ( c_type == LEX_ID && TID[c_val].get_type() != LEX_FID) {
        //check_id ();
        poliz.push_back ( Lex ( LEX_ID, c_val ) );
        gl ();
        if (c_type == LEX_LMAS) {
            cout << "MAS!!" << endl;
            gl();
            if ( c_type != LEX_NUM )
                throw curr_lex;
            gl ();
            if ( c_type != LEX_RMAS )
                throw curr_lex;
            gl();
        }
    }
    else if ( c_type == LEX_ID && TID[c_val].get_type() == LEX_FID) {
        //cout << "procedure..." << endl;
        //cout << "__TID\n" << TID[c_val].get_name() << " " << TID[c_val].get_type() << " " << TID[c_val].get_value() << endl;
        st_lex.push ( LEX_INT );
        poliz.push_back ( Lex(POLIZ_LABEL, poliz.size()+3) );
        //poliz.push_back ()
        proc_val = c_val;
        gl ();
        if ( c_type == LEX_LPAREN ) {
            gl ();
            if ( c_type == LEX_RPAREN ) ;
            else if ( c_type == LEX_ID || c_type == LEX_NUM) {
                //DP();
                if (c_type == LEX_ID)
                    poliz.push_back ( Lex ( LEX_ID, c_val ) );
                else
                    poliz.push_back ( Lex ( LEX_NUM, c_val ) );
                gl ();
                while ( c_type == LEX_COMMA ) {
                    gl ();
                    if (c_type == LEX_ID)
                        poliz.push_back ( Lex ( LEX_ID, c_val ) );
                    else
                        poliz.push_back ( Lex ( LEX_NUM, c_val ) );
                    gl ();
                    //DP ();
                }
                if ( c_type != LEX_RPAREN )
                    throw curr_lex;
            }
            else
                throw curr_lex;
            gl ();
        }
        else
            throw curr_lex;

        poliz.push_back ( Lex(POLIZ_LABEL, TID[proc_val].get_value()) );
        poliz.push_back ( Lex ( POLIZ_GO ) );
    }
    else if ( c_type == LEX_NUM ) {
        st_lex.push ( LEX_INT );
        poliz.push_back ( curr_lex );
        gl ();
    }
    else if ( c_type == LEX_TRUE ) {
        st_lex.push ( LEX_BOOL );
        poliz.push_back ( Lex (LEX_TRUE, 1) );
        gl ();
    }
    else if ( c_type == LEX_FALSE) {
        st_lex.push ( LEX_BOOL );
        poliz.push_back ( Lex (LEX_FALSE, 0) );
        gl ();
    }
    else if ( c_type == LEX_NOT ) {
        gl ();
        F ();
        check_not ();
    }
    else if ( c_type == LEX_LPAREN ) {
        gl ();
        E ();
        if ( c_type == LEX_RPAREN)
            gl ();
        else
            throw curr_lex;
    }
    else
        throw curr_lex;
}

/*==========================================================================
 * Семантический анализ. Проверка необходимых условий
 *==========================================================================
 */
void Parser::dec ( type_of_lex type ) {
    int i;
    while ( !st_int.empty () ) {
        from_st ( st_int, i );
        if ( TID[i].get_declare () )
            throw "twice";
        else {
            TID[i].put_declare ();
            TID[i].put_type ( type );
            if (type == LEX_PID || type == LEX_FID ) {
                TID[i].put_value(poliz.size());
                TID[i].put_assign();
                curr_proc_name = TID[i].get_name();
            }
        }
    }
}

void Parser::proc_dec ( type_of_lex type ) {
    int i;
    while ( !st_int.empty () ) {
        from_st ( st_int, i );
        if ( TID[i].get_declare () && (TID[i].get_proc_name() == curr_proc_name) )
            throw "twice";
        else {
            TID[i].put_declare ();
            TID[i].put_type ( type );
            TID[i].put_proc_name( curr_proc_name );
        }
    }
}

void Parser::check_id () {
    if ( TID[c_val].get_declare() )
        st_lex.push ( TID[c_val].get_type () );
    else
        throw "not declared";
}

void Parser::check_op () {
    type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOL;

    from_st ( st_lex, t2 );
    from_st ( st_lex, op );
    from_st ( st_lex, t1 );

    if ( op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH )
        r = LEX_INT;
    if ( op == LEX_OR || op == LEX_AND )
        t = LEX_BOOL;
    if ( t1 == t2  &&  t1 == t )
        st_lex.push (r);
    else
        throw "wrong types are in operation";
    poliz.push_back (Lex (op) );
}

void Parser::check_not () {
    if (st_lex.top() != LEX_BOOL)
        throw "wrong type is in not";
    else
        poliz.push_back ( Lex (LEX_NOT) );
}

void Parser::eq_type () {
    type_of_lex t;
    from_st ( st_lex, t );
    if ( t != st_lex.top () )
        throw "wrong types are in :=";
    st_lex.pop();
}

void Parser::eq_bool () {
    if ( st_lex.top () != LEX_BOOL )
        throw "expression is not boolean";
    st_lex.pop ();
}

void Parser::check_id_in_read () {
    if ( !TID [c_val].get_declare() )
        throw "not declared";
}

#endif //PASCAL_PARSER_H