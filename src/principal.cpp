#include <iostream>
#include <cstdlib>
#include <ncurses.h>
#include <ios>      // Para std::boolalpha e std::noboolalpha
#include <vector>
#include <string>
#include <any>
#include <limits>
#include <iomanip>
#include "../include/Pessoa.hpp"
#include <pqxx/pqxx>
#include <stdlib.h> // Necessário para system()
#include <fstream>
#include <stdexcept>
#include <boost/algorithm/string.hpp>
//#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>
#include <boost/any.hpp>
#include <boost/variant.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/nowide/iostream.hpp>
#include <boost/nowide/cstdio.hpp>
#include "../include/PostgreSQLDualConnection.hpp"
#include "../include/SecretManager.hpp"  



// g++ -std=c++17 -o meu_programa Select.cpp -I/usr/include/postgresql -lpq

using namespace std; //Usando o namespace std
using std::cout;
using std::cin;
using namespace boost::gregorian;
using namespace boost::posix_time;

#define VERSAO 0.1


//Acessa String de Conexao ao Banco na Variavel de Ambiente
//const char* env_var = std::getenv("PGSQL_SCA_URI");
//std::string connection_string = env_var;
//std::string connection_string = "";
// Criar instância do DAO
//PostgresDAO dao(connection_string);

    

std::string secret(int db) {

        SecretManager secret_manager("/etc/secrets/");
        
        // Carrega todos os secrets automaticamente
        secret_manager.load_all_secrets();
        
        // Ou carrega específicos
        // secret_manager.load_secret("db_password");
        // secret_manager.load_secret("api_key");
       // std::string db_name_sca = "";
       // std::string db_name_reg = "";
        std::string conectaDB = "";

        // Acessa os valores
        if (db == 1){
        std::string db_name_sca = secret_manager.get_secret("db_name_sca");
        std::string db_pass = secret_manager.get_secret("db_password");
        std::string db_user = secret_manager.get_secret("db_user");
        std::string db_host = secret_manager.get_secret("db_host");
        conectaDB = "host="+db_host + " port=5432 dbname=" + db_name_sca + " user=" + db_user + " password="+db_pass;        
        } else {
        std::string db_name_reg = secret_manager.get_secret("db_name_reg");
        std::string db_pass = secret_manager.get_secret("db_password");
        std::string db_user = secret_manager.get_secret("db_user");
        std::string db_host = secret_manager.get_secret("db_host");
        conectaDB = "host="+db_host + " port=5432 dbname=" + db_name_reg + " user=" + db_user + " password="+db_pass;
        }


        return conectaDB;   
}

// Criar strings de conexão

    // Verificar status das conexões
    // dbConn.checkConnections();

    std::string connStr1 = secret(1);
    std::string connStr2 = secret(2);

    // Criar instância da classe
    PostgreSQLDualConnection dbDao(connStr1, connStr2, "Banco Principal", "Banco Secundário");


// Função para exibir os argumentos
void mostrarArgumentos(const vector<string>& args) {
    cout << "Argumentos recebidos:\n";
    for (size_t i = 0; i < args.size(); ++i) {
        cout << "[" << i << "] " << args[i] << "\n";
    }
}

void limparBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void cadastrar(string strNome, int intIdade, double dblAltura){
    initscr();
    cbreak();
    //noecho();
    echo();
    keypad(stdscr, TRUE);
    //curs_set(0); // Esconder cursor
    curs_set(1); // Mostra cursor

    printw("\n %s \n", string(160, '=').c_str());
    refresh();

    char buffer[160];
    const char* coluna_1 = " | NOME";
    const char* coluna_2 = "| CPF";
    const char* coluna_3 = "| E-Mail";
    const char* coluna_4 = "| LOGIN";
    const char* coluna_5 = "| ID";

    // Criando a string formatada primeiro
   // snprintf(buffer, sizeof(buffer), "Item: %-5s | Valor: %4d", texto, numero);
    snprintf(buffer, sizeof(buffer), "%-56s %-20s %-39s %-32s %-5s ", coluna_1, coluna_2, coluna_3, coluna_4, coluna_5);

    printw("%s\n", buffer); // Passa a string formatada para printw
    refresh();
    printw(" %s \n", string(160, '-').c_str());
    refresh();

    Pessoa fulano( strNome, intIdade, dblAltura );
    
    const char* charArray = fulano.getNome().c_str();
  //  printf("%s\n", charArray );
    printw("Você digitou: %s \n", charArray);
    refresh();                      // Atualiza a tela
    getch();                        // Espera entrada do usuário

    endwin();

}

int consultarUsuario(string param){

        dbDao.checkConnections();
      //  clear();
        // Buscar todos os usuários
        auto usuarios = dbDao.buscarUsuariosPorNome( param );

        std::cout << "Registros encontrados: " << usuarios.size() << std::endl;
        std::cout << std::string(160, '=') << std::endl;

        std::cout << "│ " << std::left  
        << std::setw(58) << "NOME"
        << std::setw(20) << "| CPF"
        << std::setw(40) << "| E-Mail"
        << std::setw(32) << "| LOGIN"
        << std::setw(5) << "| ID" << std::endl;
        std::cout << std::string(160, '-') << std::endl;

        for (size_t i = 0; i < usuarios.size(); ++i) {
            std::cout << i+1 << ". " << std::setw(57) << usuarios[i].getNome() 
            << std::setw(20) << usuarios[i].getCpf()
            << std::setw(40) << usuarios[i].getEmail()
            << std::setw(32) << usuarios[i].getLogin() 
            << std::setw(5) << usuarios[i].getId() << std::endl;
        }

        std::cout << std::string(160, '-') << std::endl;
       // dao.desconectar();

    return 0;
}

int consultarOrgao(string param){

        dbDao.checkConnections();
       // clear();
        // Buscar Orgaos Vinculados
        auto orgaos = dbDao.buscarOrgaoVinculado( param );

        std::cout << "Registros encontrados: " << orgaos.size() << std::endl;
        std::cout << std::string(160, '=') << std::endl;

        std::cout << "│ " << std::left  
        << std::setw(58) << "NOME"
        << std::setw(20) << "| CNPJ"
        << std::setw(40) << "| SIGLA"
        << std::setw(32) << "| ATIVO"
        << std::setw(5) << "| ID" << std::endl;
        std::cout << std::string(160, '-') << std::endl;
        if (orgaos.size() > 0){
            std::cout << "│ " << std::left << std::setw(56)  <<  orgaos[0].getPessoaNome() << std::endl;
        }
        std::cout << std::string(160, '-') << std::endl;

        for (size_t i = 0; i < orgaos.size(); ++i) {
            std::cout << i+1 << ". " << std::setw(59) << orgaos[i].getNome().substr(0, 50)
            << std::setw(20) << orgaos[i].getCnpj()
            << std::setw(40) << orgaos[i].getSigla()
            << std::setw(32) << std::boolalpha << orgaos[i].getAtivo() 
            << std::setw(5) << orgaos[i].getId() << std::endl;
        } 
        std::cout << std::string(160, '-') << std::endl;

    return 0;
}

int main(int argc, char* argv[]) {

    int opcao = 0;
    int intIdade = 0;
    int retorno = 0;
    int intDel = 0;
   // bool bolFeito = false;
    string strBoost;
    //char strLogin[20];
    //char strPassword[50];

    double dblAltura = 0;
    char strNome[80];
    string strArgumento;


    //Converter os argumentos para um vector de strings
    vector<string> argumentos(argv, argv + argc);
    
    for (size_t i = 0; i < argumentos.size(); ++i) {
       // cout << "[" << i << "] " << argumentos[i] << "\n";
    }


    if (argumentos.size() > 1){
       // cout << "[ TEM  ] " << argumentos[1] << "\n";
        strArgumento = argumentos[1];
        retorno = consultarUsuario(strArgumento);
        cout << "\nPressione Enter para continuar...";
        cin.get();
    }


    
        // Hora atual
        ptime agora = second_clock::local_time();
        std::cout << " \nAgora: " << agora << std::endl;

        /** 
        std::string texto = "Boost C++ Libraries são poderosas";
    
        // Converter para maiúsculas
        boost::to_upper(texto);
        std::cout << "Maiúsculas: " << texto << "\n";

        cout << "Digite seu LOGIN: " << endl;
        cin.getline( strLogin, 20);

        std::string strPassword = dao.get_password("Digite sua senha: ");
      //  boost::nowide::cout << "Senha digitada (não mostrada): " 
        //               << std::string(strPassword.size(), '*') << std::endl;
        cout << "A SENHA digitada: " << strPassword << endl;

        bolFeito = dao.conectar();

        if (bolFeito) {
        cout << "CONECTADO com Sucesso!" << endl;
        }else{
        cout << "NAO CONECTOU!" << endl;
        }
        */

        opcao = 0; 
        do {
        cout << "\n... Versao: " << VERSAO << endl;
        cout << "\n=== MENU DE ACESSO ===" << endl;
        cout << "1. Adicionar item" << endl;
        cout << "2. Pesquisa por Nome" << endl;
        cout << "3. Pesquisar por ID" << endl;
        cout << "4. Deletar item" << endl;
        cout << "5. Sair" << endl;
        cout << "Escolha uma opção (1-5): ";
        
        // Validação da entrada
        while(!(cin >> opcao)) {
            cout << "Entrada inválida! Digite um número: ";
            limparBuffer();
        }
        limparBuffer();

            switch(opcao) {
            case 1:
                    cout << "Digite seu nome: " << endl;
                    cin.getline( strNome, 80);
                    cout << "Digite sua Idade : " << endl;
                    cin >> intIdade;
                    cout << "Digite sua Altura: " << endl;
                    cin >> dblAltura;
                    cadastrar(strNome, intIdade, dblAltura);
                    cout << "Item adicionado com sucesso!" << endl;
                    cin.get();

                break;
            case 2:
                cout << "Pesquisar por: ";
                cin.getline( strNome, 10);
                strBoost = strNome;

                boost::trim(strBoost);

                retorno = consultarUsuario(strBoost);
                
                if (retorno != 0){
                    cout << "Houve ERRO na pesquisa." << endl;
                }
                break;
            case 3:
                cout << "Pesquisar por ID" << endl;
                cout << "Digite o ID : ";
                cin.getline( strNome, 10);
                strBoost = strNome;

                boost::trim(strBoost);

                retorno = consultarOrgao(strBoost);
                break;
            case 4:
                cout << "Digite o ID para Excluir:" << endl;
                cin >> intDel;
                
                if (!dbDao.isConnection1Open()) {
                        std::cerr << "Falha na conexão: " << std::endl;
                        return 1;
                }
                
                cin.get();
                break;

            case 5:
                cout << "Saindo..." << endl;
                break;
            default:
                cout << "Opção deve ser entre 1 e 4!" << endl;
        }
        
        if(opcao != 5) {
            cout << "\nPressione Enter para continuar...";
            cin.get();
            system("clear");
        }
        
    } while(opcao != 5);
    clear();
    
}
 
