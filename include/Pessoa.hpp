// Pessoa.hpp -- Header file for Principal.cpp

#ifndef PESSOA_HPP  
#define PESSOA_HPP 
//#pragma once

#include <string>
#include <iostream>

using namespace std; // Usando o namespace std


// Declaração de classe PESSOA
class Pessoa {
private:
    int id;
    string nome;
    double altura;
    string login;
    string email;
    string cpf;

protected:


public:
    Pessoa(const string, int, double );
    Pessoa();
    void setNome(string);
    std::string getNome();
    void setId( int );
    int getId();
    void setAltura( double );
    double getAltura();
    string getLogin();
    string getEmail();
    string getCpf();
    void setLogin(string);
    void setEmail(string);
    void setCpf(string);

    std::string toString();
};

#endif // --PESSOA_HPP
