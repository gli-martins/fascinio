#ifndef POSTGRESQL_DUAL_CONNECTION_HPP
#define POSTGRESQL_DUAL_CONNECTION_HPP

#include <pqxx/pqxx>
#include <string>
#include <memory>
#include <stdexcept>
#include "../include/Pessoa.hpp"
#include "../include/Orgao.hpp"


/**
 * @class PostgreSQLDualConnection
 * @brief Classe para gerenciar conexões simultâneas com dois bancos de dados PostgreSQL
 * 
 * Esta classe permite estabelecer e gerenciar conexões com dois bancos de dados
 * PostgreSQL distintos utilizando a biblioteca pqxx.
 */
class PostgreSQLDualConnection {
private:
    std::unique_ptr<pqxx::connection> conn1; ///< Conexão com o primeiro banco de dados
    std::unique_ptr<pqxx::connection> conn2; ///< Conexão com o segundo banco de dados
    
    std::string connectionName1; ///< Nome descritivo da primeira conexão
    std::string connectionName2; ///< Nome descritivo da segunda conexão
    Pessoa mapearResultadoParaPessoa(const pqxx::row& row);
    Orgao mapearResultadoParaOrgao(const pqxx::row& row);

public:
    /**
     * @brief Construtor da classe
     * @param connStr1 String de conexão para o primeiro banco de dados
     * @param connStr2 String de conexão para o segundo banco de dados
     * @param name1 Nome descritivo para a primeira conexão (opcional)
     * @param name2 Nome descritivo para a segunda conexão (opcional)
     * @throws std::runtime_error Se não for possível estabelecer as conexões
     */
    PostgreSQLDualConnection(const std::string& connStr1, 
                            const std::string& connStr2,
                            const std::string& name1 = "Database1",
                            const std::string& name2 = "Database2");


    /**
     * @brief Destrutor da classe - fecha automaticamente as conexões
     */
    ~PostgreSQLDualConnection();

    // Construtor de cópia e operador de atribuição deletados
    PostgreSQLDualConnection(const PostgreSQLDualConnection&) = delete;
    PostgreSQLDualConnection& operator=(const PostgreSQLDualConnection&) = delete;

    // Construtor de movimento e operador de atribuição de movimento
    PostgreSQLDualConnection(PostgreSQLDualConnection&&) = default;
    PostgreSQLDualConnection& operator=(PostgreSQLDualConnection&&) = default;

    /**
     * @brief Executa uma consulta no primeiro banco de dados
     * @param query Consulta SQL a ser executada
     * @return pqxx::result Resultado da consulta
     * @throws std::runtime_error Se a conexão não estiver aberta ou ocorrer erro na consulta
     */
    pqxx::result executeQuerySCA(const std::string& query);

    /**
     * @brief Executa uma consulta no segundo banco de dados
     * @param query Consulta SQL a ser executada
     * @return pqxx::result Resultado da consulta
     * @throws std::runtime_error Se a conexão não estiver aberta ou ocorrer erro na consulta
     */
    pqxx::result executeQuery2(const std::string& query);

    /**
     * @brief Executa uma transação no primeiro banco de dados
     * @param query Comando SQL da transação
     * @throws std::runtime_error Se a conexão não estiver aberta ou ocorrer erro na transação
     */
    void executeTransaction1(const std::string& query);

    /**
     * @brief Executa uma transação no segundo banco de dados
     * @param query Comando SQL da transação
     * @throws std::runtime_error Se a conexão não estiver aberta ou ocorrer erro na transação
     */
    void executeTransaction2(const std::string& query);

    /**
     * @brief Verifica o status das conexões e exibe no console
     */
    void checkConnections() const;

    /**
     * @brief Obtém a referência para a primeira conexão (uso avançado)
     * @return Referência para a conexão do primeiro banco
     * @throws std::runtime_error Se a conexão não estiver disponível
     */
    pqxx::connection& getConnection1();

    /**
     * @brief Obtém a referência para a segunda conexão (uso avançado)
     * @return Referência para a conexão do segundo banco
     * @throws std::runtime_error Se a conexão não estiver disponível
     */
    pqxx::connection& getConnection2();

    /**
     * @brief Verifica se a primeira conexão está aberta
     * @return true se a conexão estiver aberta, false caso contrário
     */
    bool isConnection1Open() const;

    /**
     * @brief Verifica se a segunda conexão está aberta
     * @return true se a conexão estiver aberta, false caso contrário
     */
    bool isConnection2Open() const;

    /**
     * @brief Obtém o nome da primeira conexão
     * @return Nome descritivo da primeira conexão
     */
    std::string getConnectionName1() const { return connectionName1; }

    /**
     * @brief Obtém o nome da segunda conexão
     * @return Nome descritivo da segunda conexão
     */
    std::string getConnectionName2() const { return connectionName2; }

    /**
     * @brief Obtém o nome do banco da primeira conexão
     * @return Nome do banco de dados da primeira conexão
     * @throws std::runtime_error Se a conexão não estiver aberta
     */
    std::string getDatabaseName1() const;

    /**
     * @brief Obtém o nome do banco da segunda conexão
     * @return Nome do banco de dados da segunda conexão
     * @throws std::runtime_error Se a conexão não estiver aberta
     */
    std::string getDatabaseName2() const;

    std::vector<Pessoa> buscarUsuariosPorNome(const std::string& nome);
    std::vector<Orgao> buscarOrgaoVinculado(const std::string& nome);


};

/**
 * @brief Função auxiliar para criar string de conexão PostgreSQL
 * @param host Endereço do host do PostgreSQL
 * @param port Porta do PostgreSQL (padrão: 5432)
 * @param dbname Nome do banco de dados
 * @param user Nome do usuário
 * @param password Senha do usuário
 * @return String de conexão formatada
 */
std::string createConnectionString(const std::string& host, 
                                  const std::string& port = "5432",
                                  const std::string& dbname = "",
                                  const std::string& user = "",
                                  const std::string& password = "");

/**
 * @brief Função auxiliar para criar string de conexão com opções adicionais
 * @param options Map de opções de conexão (chave-valor)
 * @return String de conexão formatada
 */
std::string createConnectionString(const std::map<std::string, std::string>& options);

#endif // POSTGRESQL_DUAL_CONNECTION_HPP