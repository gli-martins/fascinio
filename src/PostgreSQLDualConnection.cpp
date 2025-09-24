#include "../include/PostgreSQLDualConnection.hpp"
#include "../include/Pessoa.hpp"
#include "../include/Orgao.hpp"
#include <iostream>
#include <map>
#include <sstream>

// Implementação do construtor
PostgreSQLDualConnection::PostgreSQLDualConnection(const std::string& connStr1, 
                                                  const std::string& connStr2,
                                                  const std::string& name1,
                                                  const std::string& name2)
    : connectionName1(name1), connectionName2(name2) {
    
    try {
        // Conectar ao primeiro banco de dados
        conn1 = std::make_unique<pqxx::connection>(connStr1);
        std::cout << "Conexão estabelecida com NOME " << connectionName1 
                  << " (" << conn1->dbname() << ")" << std::endl;
        
        // Conectar ao segundo banco de dados
        conn2 = std::make_unique<pqxx::connection>(connStr2);
        std::cout << "Conexão estabelecida com " << connectionName2 
                  << " (" << conn2->dbname() << ")" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Erro ao conectar: " << e.what() << std::endl;
        throw std::runtime_error("Falha ao estabelecer conexões: " + std::string(e.what()));
    }
}


// Implementação do destrutor
PostgreSQLDualConnection::~PostgreSQLDualConnection() {
    if (conn1 && conn1->is_open()) {
        conn1->close();
        std::cout << "Conexão com " << connectionName1 << " fechada." << std::endl;
    }
    if (conn2 && conn2->is_open()) {
        conn2->close();
        std::cout << "Conexão com " << connectionName2 << " fechada." << std::endl;
    }
}


// Verificar status das conexões
void PostgreSQLDualConnection::checkConnections() const {
        std::cout << "Status das conexões:" << std::endl;
        std::cout << connectionName1 << ": " 
                  << (conn1 && conn1->is_open() ? "CONECTADO" : "DESCONECTADO") 
                  << std::endl;
        std::cout << connectionName2 << ": " 
                  << (conn2 && conn2->is_open() ? "CONECTADO" : "DESCONECTADO") 
                  << std::endl;
    }

    // Métodos para obter as conexões (para uso avançado)
pqxx::connection& PostgreSQLDualConnection::getConnection1() {
    if (!conn1 || !conn1->is_open()) {
        throw std::runtime_error("Conexão 1 não está disponível");
    }
    return *conn1;
}

pqxx::connection& PostgreSQLDualConnection::getConnection2() {
    if (!conn2 || !conn2->is_open()) {
        throw std::runtime_error("Conexão 2 não está disponível");
    }
    return *conn2;
}

// Implementações dos métodos (exemplos)
pqxx::result PostgreSQLDualConnection::executeQuerySCA(const std::string& query) {
    if (!conn1 || !conn1->is_open()) {
        throw std::runtime_error("Conexão 1 não está aberta");
    }
    
    try {
        pqxx::work txn(*conn1);
        pqxx::result result = txn.exec(query);
        txn.commit();
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error("Erro na consulta 1: " + std::string(e.what()));
    }
}

// Executar consulta no segundo banco
pqxx::result PostgreSQLDualConnection::executeQuery2(const std::string& query) {
        if (!conn2 || !conn2->is_open()) {
            throw std::runtime_error("Conexão 2 não está aberta");
        }
        
        try {
            pqxx::work txn(*conn2);
            pqxx::result result = txn.exec(query);
            txn.commit();
            return result;
        } catch (const std::exception& e) {
            throw std::runtime_error("Erro na consulta 2: " + std::string(e.what()));
        }
    }

    // Executar transação no primeiro banco
    void PostgreSQLDualConnection::executeTransaction1(const std::string& query) {
        if (!conn1 || !conn1->is_open()) {
            throw std::runtime_error("Conexão 1 não está aberta");
        }
        
        try {
            pqxx::work txn(*conn1);
            txn.exec(query);
            txn.commit();
        } catch (const std::exception& e) {
            throw std::runtime_error("Erro na transação 1: " + std::string(e.what()));
        }
    }

        // Executar transação no segundo banco
    void PostgreSQLDualConnection::executeTransaction2(const std::string& query) {
        if (!conn2 || !conn2->is_open()) {
            throw std::runtime_error("Conexão 2 não está aberta");
        }
        
        try {
            pqxx::work txn(*conn2);
            txn.exec(query);
            txn.commit();
        } catch (const std::exception& e) {
            throw std::runtime_error("Erro na transação 2: " + std::string(e.what()));
        }
    }

// Implementações similares para os outros métodos...

std::string PostgreSQLDualConnection::getDatabaseName1() const {
    if (!conn1 || !conn1->is_open()) {
        throw std::runtime_error("Conexão 1 não está aberta");
    }
    return conn1->dbname();
}

std::string PostgreSQLDualConnection::getDatabaseName2() const {
    if (!conn2 || !conn2->is_open()) {
        throw std::runtime_error("Conexão 2 não está aberta");
    }
    return conn2->dbname();
}

// Métodos para verificar se as conexões estão abertas
    bool PostgreSQLDualConnection::isConnection1Open() const {
        return conn1 && conn1->is_open();
    }

    bool PostgreSQLDualConnection::isConnection2Open() const {
        return conn2 && conn2->is_open();
    }

// Implementação da função auxiliar
std::string createConnectionString(const std::string& host, 
                                  const std::string& port,
                                  const std::string& dbname,
                                  const std::string& user,
                                  const std::string& password) {
    std::stringstream ss;
    if (!host.empty()) ss << "host=" << host << " ";
    if (!port.empty()) ss << "port=" << port << " ";
    if (!dbname.empty()) ss << "dbname=" << dbname << " ";
    if (!user.empty()) ss << "user=" << user << " ";
    if (!password.empty()) ss << "password=" << password << " ";
    
    return ss.str();
}

std::string createConnectionString(const std::map<std::string, std::string>& options) {
    std::stringstream ss;
    for (const auto& [key, value] : options) {
        if (!value.empty()) {
            ss << key << "=" << value << " ";
        }
    }
    return ss.str();
}

std::vector<Pessoa> PostgreSQLDualConnection::buscarUsuariosPorNome(const std::string& nome) {
    
    std::vector<Pessoa> usuarios;
    
    // Verificar status das conexões
    //    checkConnections();


    try {
        const std::string query = "SELECT sca_usu_id AS id, sca_usu_login AS login, sca_usu_email AS email, sca_usu_cpf AS cpf, " 
            " sca_usu_nome AS nome FROM sca_usu_usuario WHERE sca_usu_nome ILIKE $1 ORDER BY sca_usu_nome;";

        std::string parametro_nome =  nome + "%";
        
        pqxx::work txn(*conn1);
        pqxx::result result = txn.exec(query, pqxx::params(parametro_nome));

        for (const auto& row : result) {
            usuarios.push_back(mapearResultadoParaPessoa(row));
        }

        return usuarios;

    } catch (const std::exception& e) {
       // definirUltimoErro(std::string("Erro ao buscar usuários: ") + e.what());
        return usuarios;
    }
}

std::vector<Orgao> PostgreSQLDualConnection::buscarOrgaoVinculado(const std::string& cod_sca) {
    
    std::vector<Orgao> orgaos;
    
    try {
        const std::string query = "select woo.wre_org_codigo as id, woo.wre_org_nome as orgao_nm, woo.wre_org_cnpj as cnpj, woo.wre_org_sigla as sigla, "
        " woo.wre_org_ativo as ativo, wrr.wre_sca_nome as pessoa_nm from wre_org_orgaos woo " 
            " join wre_res_responsaveis wrr on woo.wre_org_codigo = wrr.wre_org_codigo"
            " where wrr.wre_sca_id = $1;";

        std::string parametro_nome =  cod_sca;
        
        pqxx::work txn(*conn2);
        pqxx::result result = txn.exec(query, pqxx::params(parametro_nome));

        for (const auto& row : result) {
            orgaos.push_back(mapearResultadoParaOrgao(row));
        }

        return orgaos;

    } catch (const std::exception& e) {
       // definirUltimoErro(std::string("Erro ao buscar usuários: ") + e.what());
        return orgaos;
    }
}

Pessoa PostgreSQLDualConnection::mapearResultadoParaPessoa(const pqxx::row& row) {
    Pessoa pessoa;
    pessoa.setId(row["id"].as<int>());
    pessoa.setNome(row["nome"].as<std::string>());
    pessoa.setEmail(row["email"].as<std::string>());
    pessoa.setLogin(row["login"].as<std::string>());
    pessoa.setCpf( row["cpf"].as<std::string>());
    return pessoa;
}


Orgao PostgreSQLDualConnection::mapearResultadoParaOrgao(const pqxx::row& row) {
    Orgao orgao;
    orgao.setId(row["id"].as<int>());
    orgao.setNome(row["orgao_nm"].as<std::string>());
    orgao.setCnpj(row["cnpj"].as<std::string>());
    orgao.setSigla(row["sigla"].as<std::string>());
    orgao.setAtivo( row["ativo"].as<bool>());
    orgao.setPessoaNome(row["pessoa_nm"].as<std::string>());
    return orgao;
}
