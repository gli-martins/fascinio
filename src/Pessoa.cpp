// Implementacao da classe PESSOA

#include <string>
#include <iostream>
#include "../include/Pessoa.hpp"  


using namespace std; // Usando o namespace std
using std::cout;
using std::cin;

    Pessoa::Pessoa(string pnome, int pid, double paltura ) {
        setNome(pnome);
        setId(pid);
        setAltura(paltura);
    }

    Pessoa::Pessoa() {
    }

    void Pessoa::setNome(string pnome){
            nome = pnome;
    }
    
    std::string Pessoa::getNome() {
        return nome;
    }
    
    std::string Pessoa::toString(){
        return getNome();
       // cout << "*Nome: " << getNome() << endl;
       // cout << "*CPF: " << getCpf() << endl;

    }

    void Pessoa::setId( int pid ){
        id = pid;
    }

    int Pessoa::getId(){
        return id;
    }

    void Pessoa::setAltura( double paltura ){
        altura = paltura;
    }

    double Pessoa::getAltura(){
        return altura;
    }

    string Pessoa::getLogin(){
        return login;
    }
 
    string Pessoa::getEmail(){
        return email;
    }
    
    string Pessoa::getCpf(){
        return cpf;
    }

    void Pessoa::setLogin(string plogin){
        login = plogin;
    }

    void Pessoa::setEmail(string pemail){
        email = pemail;
    }

    void Pessoa::setCpf(string pcpf){
        cpf = pcpf;
    };
