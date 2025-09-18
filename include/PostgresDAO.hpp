#ifndef POSTGRES_DAO_H
#define POSTGRES_DAO_H

#include <pqxx/pqxx>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <vector>
#include "../include/Pessoa.hpp"

struct Usuario {
    int id;
    std::string nome;
    std::string email;
    int idade;
    bool ativo;
    
    Usuario() : id(0), idade(0), ativo(false) {}
    Usuario(int id, std::string nome, std::string email, int idade, bool ativo)
        : id(id), nome(std::move(nome)), email(std::move(email)), idade(idade), ativo(ativo) {}
};

class PostgresDAO {
public:
    // Construtor e destrutor
    PostgresDAO(const std::string& connection_string);
    ~PostgresDAO();

    // Métodos de conexão
    std::string secret();
    bool conectar();
    bool conectarDB(char strLogin[30], string strPassword );

    void desconectar();
    bool estaConectado() const;

    // Operações CRUD para Usuário
    bool criarUsuario(const Usuario& usuario);
    Usuario* buscarUsuarioPorId(int id);
    std::vector<Usuario> buscarTodosUsuarios();
    std::vector<Pessoa> buscarUsuariosPorNome(const std::string& nome);
    bool atualizarUsuario(const Usuario& usuario);
    bool atualizarPessoa(const Pessoa& pessoa);
    bool deletarUsuario(int id);

    // Operações transacionais
    bool iniciarTransacao();
    bool commitTransacao();
    bool rollbackTransacao();

    // Métodos utilitários
    int contarUsuarios();
    bool verificarConexao();
    std::string getUltimoErro() const;
    std::string get_password(const std::string& prompt) const ;

private:
    std::string connection_string_;
    std::unique_ptr<pqxx::connection> conexao_;
    std::unique_ptr<pqxx::work> transacao_;
    mutable std::string ultimo_erro_;

    void definirUltimoErro(const std::string& erro) const;    
    Usuario mapearResultadoParaUsuario(const pqxx::row& row);
    Pessoa mapearResultadoParaPessoa(const pqxx::row& row);
};

#endif // POSTGRES_DAO_H