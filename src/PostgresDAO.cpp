#include "../include/PostgresDAO.hpp"
#include "../include/Pessoa.hpp"
#include <pqxx/pqxx>
#include <fstream>
#include <stdexcept>
#include "../include/SecretManager.hpp"  
#include <boost/nowide/iostream.hpp>
#include <boost/nowide/cstdio.hpp>
#include <string>
#include <termios.h> // POSIX terminal control definitions

PostgresDAO::PostgresDAO(const std::string& connection_string)
    : connection_string_(connection_string) {
}

PostgresDAO::~PostgresDAO() {
    desconectar();
}

bool PostgresDAO::conectarDB( char strLogin[30], string strPassword ) {

    cout << "Login: " << strLogin << "  - Pass: " << strPassword << endl;

   try {
       // connection_string_ = secret(strLogin, strPassword);
    
        // chmod 600  <-- permissão do secret

        conexao_ = std::make_unique<pqxx::connection>(connection_string_);
        
        if (conexao_->is_open()) {
           // std::cout << "Conexão estabelecida com sucesso!" << std::endl;
            return true;
        } else {
            definirUltimoErro("Falha ao abrir conexão");
            return false;
        }
    } catch (const std::exception& e) {

        definirUltimoErro(std::string("Erro de conexão: ") + e.what());
        return false;
    }
}

bool PostgresDAO::conectar() {

   try {
        connection_string_ = secret();
    
        // chmod 600  <-- permissão do secret

        conexao_ = std::make_unique<pqxx::connection>(connection_string_);
        
        if (conexao_->is_open()) {
           // std::cout << "Conexão estabelecida com sucesso!" << std::endl;
            return true;
        } else {
            definirUltimoErro("Falha ao abrir conexão");
            return false;
        }
    } catch (const std::exception& e) {

        definirUltimoErro(std::string("Erro de conexão: ") + e.what());
        return false;
    }
}

void PostgresDAO::desconectar() {
    if (transacao_) {
        try {
            transacao_->abort();
        } catch (...) {
            // Ignorar erros no abort
        }
        transacao_.reset();
    }
    conexao_.reset();
}

bool PostgresDAO::estaConectado() const {
    return conexao_ && conexao_->is_open();
}

std::string PostgresDAO::secret() {

        SecretManager secret_manager("/etc/secrets/");
        
        // Carrega todos os secrets automaticamente
        secret_manager.load_all_secrets();
        
        // Ou carrega específicos
        // secret_manager.load_secret("db_password");
        // secret_manager.load_secret("api_key");
        
        // Acessa os valores
        std::string db_pass = secret_manager.get_secret("db_password");
        std::string db_user = secret_manager.get_secret("db_user");
        std::string db_name = secret_manager.get_secret("db_name_sca");
        std::string db_host = secret_manager.get_secret("db_host");

        /** 
        std::cout << "DB Password x: " << db_pass << std::endl;
        std::cout << "Usuario: x" << db_user << std::endl;
        std::cout << "DB Name: x" << db_name << std::endl;
        std::cout << "Host: x" << db_host << std::endl;
        */
        //CONECTA_SCA = "host=localhost port=5432 dbname=webregularidade_sca user=desenv password=gliberto";

        string conectaDB = "host="+db_host + " port=5432 dbname=" + db_name + " user=" + db_user + " password="+db_pass;

       // std::cout << "Host de CONEXAO: " << conectaDB << std::endl;

        return conectaDB;   
}


bool PostgresDAO::criarUsuario(const Usuario& usuario) {
    if (!estaConectado()) {
        definirUltimoErro("Não conectado ao banco de dados");
        return false;
    }

    try {
        const std::string query = 
            "INSERT INTO usuarios (nome, email, idade, ativo) "
            "VALUES ($1, $2, $3, $4) RETURNING id";

        pqxx::work txn(*conexao_);
        pqxx::result result = txn.exec_params(
            query, usuario.nome, usuario.email, usuario.idade, usuario.ativo
        );

        txn.commit();

        if (!result.empty()) {
            std::cout << "Usuário criado com ID: " << result[0][0].as<int>() << std::endl;
            return true;
        }
        
        return false;

    } catch (const std::exception& e) {
        definirUltimoErro(std::string("Erro ao criar usuário: ") + e.what());
        return false;
    }
}

Usuario* PostgresDAO::buscarUsuarioPorId(int id) {
    if (!estaConectado()) {
        definirUltimoErro("Não conectado ao banco de dados");
        return nullptr;
    }

    try {
        const std::string query = 
            "SELECT id, nome, email, idade, ativo FROM usuarios WHERE id = $1";

        pqxx::work txn(*conexao_);
        pqxx::result result = txn.exec_params(query, id);

        if (!result.empty()) {
            Usuario* usuario = new Usuario();
            *usuario = mapearResultadoParaUsuario(result[0]);
            return usuario;
        }

        return nullptr;

    } catch (const std::exception& e) {
        definirUltimoErro(std::string("Erro ao buscar usuário: ") + e.what());
        return nullptr;
    }
}

std::vector<Usuario> PostgresDAO::buscarTodosUsuarios() {
    std::vector<Usuario> usuarios;
    
    if (!estaConectado()) {
        definirUltimoErro("Não conectado ao banco de dados");
        return usuarios;
    }

    try {
        const std::string query = 
            "SELECT id, nome, email, idade, ativo FROM usuarios ORDER BY nome";

        pqxx::work txn(*conexao_);
        pqxx::result result = txn.exec(query);

        for (const auto& row : result) {
            usuarios.push_back(mapearResultadoParaUsuario(row));
        }

        return usuarios;

    } catch (const std::exception& e) {
        definirUltimoErro(std::string("Erro ao buscar usuários: ") + e.what());
        return usuarios;
    }
}

std::vector<Pessoa> PostgresDAO::buscarUsuariosPorNome(const std::string& nome) {
    
    std::vector<Pessoa> usuarios;
    
    if (!estaConectado()) {
        definirUltimoErro("Não conectado ao banco de dados");
        return usuarios;
    }

    try {
        const std::string query = "SELECT sca_usu_id AS id, sca_usu_login AS login, sca_usu_email AS email, sca_usu_cpf AS cpf, " 
            " sca_usu_nome AS nome FROM sca_usu_usuario WHERE sca_usu_nome ILIKE $1 ORDER BY sca_usu_nome;";

        std::string parametro_nome =  nome + "%";
        
        pqxx::work txn(*conexao_);
       // pqxx::result result = txn.exec_params(query, parametro_nome);
        pqxx::result result = txn.exec(query, pqxx::params(parametro_nome));


        for (const auto& row : result) {
            usuarios.push_back(mapearResultadoParaPessoa(row));
        }

        return usuarios;

    } catch (const std::exception& e) {
        definirUltimoErro(std::string("Erro ao buscar usuários: ") + e.what());
        return usuarios;
    }
}

bool PostgresDAO::atualizarUsuario(const Usuario& usuario) {
    if (!estaConectado()) {
        definirUltimoErro("Não conectado ao banco de dados");
        return false;
    }

    try {
        const std::string query = 
            "UPDATE usuarios SET nome = $1, email = $2, idade = $3, ativo = $4 "
            "WHERE id = $5";

        pqxx::work txn(*conexao_);
        pqxx::result result = txn.exec_params(
            query, usuario.nome, usuario.email, usuario.idade, usuario.ativo, usuario.id
        );

        txn.commit();

        if (result.affected_rows() > 0) {
            std::cout << "Usuário atualizado com sucesso!" << std::endl;
            return true;
        }
        
        return false;

    } catch (const std::exception& e) {
        definirUltimoErro(std::string("Erro ao atualizar usuário: ") + e.what());
        return false;
    }
}

bool PostgresDAO::deletarUsuario(int id) {
    if (!estaConectado()) {
        definirUltimoErro("Não conectado ao banco de dados");
        return false;
    }

    try {
        const std::string query = "DELETE FROM sca_usu_usuario WHERE sca_usu_id = $1";

        pqxx::work txn(*conexao_);
        pqxx::result result = txn.exec_params(query, id);

        txn.commit();

        if (result.affected_rows() > 0) {
            std::cout << "Usuário deletado com sucesso!" << std::endl;
            return true;
        }
        
        return false;

    } catch (const std::exception& e) {
        definirUltimoErro(std::string("Erro ao deletar usuário: ") + e.what());
        return false;
    }
}

int PostgresDAO::contarUsuarios() {
    if (!estaConectado()) {
        definirUltimoErro("Não conectado ao banco de dados");
        return -1;
    }

    try {
        const std::string query = "SELECT COUNT(*) FROM usuarios";

        pqxx::work txn(*conexao_);
        pqxx::result result = txn.exec(query);

        if (!result.empty()) {
            return result[0][0].as<int>();
        }
        
        return 0;

    } catch (const std::exception& e) {
        definirUltimoErro(std::string("Erro ao contar usuários: ") + e.what());
        return -1;
    }
}

bool PostgresDAO::verificarConexao() {
    if (!estaConectado()) {
        return false;
    }

    try {
        pqxx::work txn(*conexao_);
        pqxx::result result = txn.exec("SELECT 1");
        return !result.empty();
    } catch (const std::exception& e) {
        definirUltimoErro(std::string("Erro ao verificar conexão: ") + e.what());
        return false;
    }
}

bool PostgresDAO::iniciarTransacao() {
    if (!estaConectado()) {
        definirUltimoErro("Não conectado ao banco de dados");
        return false;
    }

    try {
        transacao_ = std::make_unique<pqxx::work>(*conexao_);
        return true;
    } catch (const std::exception& e) {
        definirUltimoErro(std::string("Erro ao iniciar transação: ") + e.what());
        return false;
    }
}

bool PostgresDAO::commitTransacao() {
    if (!transacao_) {
        definirUltimoErro("Nenhuma transação ativa");
        return false;
    }

    try {
        transacao_->commit();
        transacao_.reset();
        return true;
    } catch (const std::exception& e) {
        definirUltimoErro(std::string("Erro ao commitar transação: ") + e.what());
        return false;
    }
}

bool PostgresDAO::rollbackTransacao() {
    if (!transacao_) {
        definirUltimoErro("Nenhuma transação ativa");
        return false;
    }

    try {
        transacao_->abort();
        transacao_.reset();
        return true;
    } catch (const std::exception& e) {
        definirUltimoErro(std::string("Erro ao fazer rollback: ") + e.what());
        return false;
    }
}

Usuario PostgresDAO::mapearResultadoParaUsuario(const pqxx::row& row) {
    return Usuario(
        row["id"].as<int>(),
        row["nome"].as<std::string>(),
        row["email"].as<std::string>(),
        row["idade"].as<int>(),
        row["ativo"].as<bool>()
    );
}


Pessoa PostgresDAO::mapearResultadoParaPessoa(const pqxx::row& row) {
    Pessoa pessoa;
    pessoa.setId(row["id"].as<int>());
    pessoa.setNome(row["nome"].as<std::string>());
    pessoa.setEmail(row["email"].as<std::string>());
    pessoa.setLogin(row["login"].as<std::string>());
    pessoa.setCpf( row["cpf"].as<std::string>());
    return pessoa;
}

void PostgresDAO::definirUltimoErro(const std::string& erro) const {
    ultimo_erro_ = erro;
    std::cerr << "Erro DAO: " << erro << std::endl;
}

std::string PostgresDAO::getUltimoErro() const {
    return ultimo_erro_;
}

std::string PostgresDAO::get_password(const std::string& prompt = "Senha: ") const {
    boost::nowide::cout << prompt << std::flush;
    
    // Desabilitar eco do terminal
    #ifdef _WIN32
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(hStdin, &mode);
        SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
    #else
        termios oldt;
        tcgetattr(STDIN_FILENO, &oldt);
        termios newt = oldt;
        newt.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    #endif

    std::string password;
    std::getline(std::cin, password);
    
    // Restaurar configurações do terminal
    #ifdef _WIN32
        SetConsoleMode(hStdin, mode);
    #else
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    #endif
    
    boost::nowide::cout << std::endl;
    return password;
}