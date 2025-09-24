// Pessoa.hpp -- Header file for Principal.cpp

#ifndef ORGAO_HPP  
#define ORGAO_HPP 
//#pragma once

#include <string>
#include <iostream>

using namespace std; // Usando o namespace std


// Declaração de classe ORGAO
class Orgao {
private:
    int id;
    string nome;
    string cnpj;
    string sigla;
    bool ativo;
    string pessoa_nm;

protected:


public:
    Orgao(const int);
    Orgao();
    void setNome(string);
    std::string getNome();
    void setId( int );
    int getId();
    string getCnpj();
    string getSigla();
    bool getAtivo();
    void setCnpj(string);
    void setSigla(string);
    void setAtivo(bool);
    void setPessoaNome(string);
    string getPessoaNome();

    std::string toString();
};

#endif // --ORGAO_HPP
