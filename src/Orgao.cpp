// Implementacao da classe Orgao

#include <string>
#include <iostream>
#include "../include/Orgao.hpp"  


using namespace std; // Usando o namespace std
using std::cout;
using std::cin;

    Orgao::Orgao(const int pid) {
        setId(pid);
    }

    Orgao::Orgao() {
    }

    void Orgao::setNome(string pnome){
            nome = pnome;
    }
    
    std::string Orgao::getNome() {
        return nome;
    }
    
    std::string Orgao::toString(){
        return getNome();

    }

    void Orgao::setId( int pid ){
        id = pid;
    }

    int Orgao::getId(){
        return id;
    }


    std::string Orgao::getCnpj(){
        return cnpj;
    }

    string Orgao::getSigla(){
        return sigla;
    }
 
    bool Orgao::getAtivo(){
        return ativo;
    }
    
    void Orgao::setCnpj(string pcnpj){
        cnpj = pcnpj;
    }

    void Orgao::setSigla(string psigla){
        sigla = psigla;
    }

    void Orgao::setAtivo(bool pativo){
        ativo = pativo;
    };

    void Orgao::setPessoaNome(string pNmPessoa){
        pessoa_nm = pNmPessoa;
    }

    string Orgao::getPessoaNome(){
        return pessoa_nm;
    }
