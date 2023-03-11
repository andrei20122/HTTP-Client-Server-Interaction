#include <bits/stdc++.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <climits>
#include <cstring>
#include <string.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include "json.hpp"
#include "buffer.h"
#include "helpers.h"
#include "requests.h"
#define MAX_LEN_BOOK 300
using namespace std;

bool isNumber(const string& str)
{
    for (char const &c : str) {
        if (isdigit(c) == 0) return false;
    }
    return true;
}


string cookie;
int main (int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;
    int jwt_check = 0;
    char jwt_token[300];
    char *err;
    bool check_log = false;
    
    char* res;
    //la fiecare comanda deschid conexiunea  si o inchid 
    while(1){
        char comanda_buffer[100];
        cin >> comanda_buffer;
       
        if(strcmp(comanda_buffer,"register")==0)
        {   
            sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
            
            string userN,passw;
            cout << "username=";
            cin >> userN;
            cout << "password=";
            cin >> passw;
            
            nlohmann::json data_body;
            data_body["username"] = userN;
            data_body["password"] = passw;

            char **send_parametrii_post = (char**)(malloc(sizeof(char*)));
            send_parametrii_post[0] = (char*)(malloc(sizeof(char)*200));
            strcpy(send_parametrii_post[0], data_body.dump().c_str());

            message = compute_post_request("34.241.4.235","/api/v1/tema/auth/register","application/json",
                                                    send_parametrii_post,1,NULL,0,NULL);
            
            
            send_to_server(sockfd,message);
            response = receive_from_server(sockfd);
            cout << message <<"\n";
            
            err = strstr(response, "error");
            if (err) {
                cout << "User-ul exista deja!!"<< "\n";
            } else {
                cout << "S-a inregistrat cineva nou!"<< "\n";
            }

            close(sockfd);


        }
        else if(strcmp(comanda_buffer,"login") == 0)
        {
            if (check_log == true) {
                cout << "Exista deja logat!"<< "\n";
                continue;
            }
            sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
            string userN,passw;
            
            cout << "username=";
            cin >> userN;
            cout << "password=";
            cin >> passw;
            
            nlohmann::json data_body;
            data_body["username"] = userN;
            data_body["password"] = passw;

            char **send_parametrii_post = (char**)(malloc(sizeof(char*)));
            send_parametrii_post[0] = (char*)(malloc(sizeof(char)*200));
            strcpy(send_parametrii_post[0], data_body.dump().c_str());

            message = compute_post_request("34.241.4.235","/api/v1/tema/auth/login","application/json",
                                                    send_parametrii_post,1,NULL,0,NULL);
            
            
            send_to_server(sockfd,message);
            response = receive_from_server(sockfd);

            err = strstr(response, "error");
            if (err) {
                cout <<"Credentialele nu se potrivesc"<< "\n";
            }
            else {
                
                char *a = strstr(response, "Set-Cookie");
                char *aux = a + 12;
                char *b = strstr(aux, ";");
                res = (char*)malloc(300*sizeof(char));
                strncpy(res, aux, b - aux + 1);
                cookie = res;
                
                check_log = true;
                cout << "Logare realizata cu succes " << "\n";
            }
            //cout << cookie;
            close(sockfd);
        }
        else if(strcmp(comanda_buffer,"enter_library") == 0)
        {
            if (check_log != true) {
                cout << "Inainte de a intra in librarie, logati-va :///!"<<"\n";
                continue;
            }

            sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
            char **send_parametrii_get = (char**)(malloc(sizeof(char*)));
            send_parametrii_get[0] = (char*)(malloc(sizeof(char)*200));
            strcpy(send_parametrii_get[0], cookie.c_str());
            cookie = res;
            message = compute_get_request("34.241.4.235","/api/v1/tema/library/access",NULL,send_parametrii_get,1,NULL);
            send_to_server(sockfd,message);
            response = receive_from_server(sockfd);
           
          
               
            char* end_token = strstr(response, "\"}");
            *(end_token) = '\0';
            char* start_token = strstr(response, "{\"token\":");
            start_token = start_token + 10;
            strncpy(jwt_token, start_token, 350);
            jwt_check = 1;
            cout <<"token trimis cu succes" << "\n";
           
            
            //cout << message;
            cout << response << "\n";
            cout << "====TOKEN UL ESTE===="<<"\n";
            cout << jwt_token;
            

            close(sockfd);
        }
        else if(strcmp(comanda_buffer,"get_books") == 0)
        { 
            if ( jwt_check != 1 || check_log == false) {
                cout << "Nu suntem la, libraria buna" << "\n";
                continue;
            }
            sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
            
            char **send_parametrii_get = (char**)(malloc(2*sizeof(char*)));
            send_parametrii_get[0] = (char*)(malloc(sizeof(char)*310));
            send_parametrii_get[1] = (char*)(malloc(sizeof(char)*310));
           
            strcpy(send_parametrii_get[1],cookie.c_str());
            cookie = res;
            strcpy(send_parametrii_get[0], "Authorization: Bearer ");
            strcat(send_parametrii_get[0],jwt_token);

            message = compute_get_request("34.241.4.235","/api/v1/tema/library/books",NULL,send_parametrii_get,2,jwt_token);
            send_to_server(sockfd,message);
            response = receive_from_server(sockfd);


           err = strstr(response, "error");
            if (err) {
                cout<<"Error.....idk:((((("<<"\n";
            }
            else {
                char* start_json = strstr(response, "[{\"");
                cout << start_json << "\n";
            }
            close(sockfd);
            

          
        }
        else if(strcmp(comanda_buffer,"get_book") == 0)
        {
            if ( jwt_check != 1 || check_log == false) {
                cout << "Nu suntem la, libraria buna, incercati sa va logati" << "\n";
                continue;
            }
            sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
           
            char id[100];
            cout << "id="; 
            cin >> id;
            char url[500];
            
            strcpy(url, "/api/v1/tema/library/books/");strcat(url, id);
            char **send_parametrii_get = (char**)(malloc(sizeof(char*)));
            send_parametrii_get[0] = (char*)(malloc(sizeof(char)*310));
        
            message = compute_get_request("34.241.4.235",url,NULL,send_parametrii_get,1,jwt_token);
            send_to_server(sockfd,message);
            response = receive_from_server(sockfd);
            

            char *aux = strdup(response);
            char *error = strtok(strstr(aux, " "), " ");
           

            if (error[0] == '2') {
                cout << "[" << basic_extract_json_response(response) << endl;
            } else {
                char *mess_new = basic_extract_json_response(response);
                mess_new[strlen(mess_new) - 2] = '\0';
                cout << mess_new + 10 << endl;
            }

            close(sockfd);

        }
        else if(strcmp(comanda_buffer,"add_book") == 0)
        { 
            if ( jwt_check != 1 || check_log == false) {
                cout << "Nu suntem la, libraria buna" << "\n";
                continue;
            }

            sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
            
            string title,author,genre,publisher;
            string page_count;

            string back_slesh_ignore;
            getline(cin,back_slesh_ignore,'\n');
            cout <<"title="; getline(cin,title,'\n');
            cout << "author="; getline(cin,author,'\n');
            cout << "genre="; getline(cin,genre,'\n');
            if(isNumber(page_count)){
                 cout << "page_count="; //am declarat page_count ul ca un string si 
                 //verific daca este numar sau nu ca sa nu mai primesc eroare 
                 cin >> page_count;
            }
            string ignore; getline(cin,ignore,'\n');
            cout << "publisher="; getline(cin,publisher,'\n');
            

            nlohmann::json data_body_add_book;
            data_body_add_book["title"] = title;
            data_body_add_book["author"] = author;
            data_body_add_book["genre"] = genre;
            data_body_add_book["page_count"] = page_count;
            data_body_add_book["publisher"] = publisher;
            
            char **send_parametrii_post = (char**)(malloc(sizeof(char*)));
            send_parametrii_post[0] = (char*)(malloc(sizeof(char)*310));
           
            strcpy(send_parametrii_post[0], data_body_add_book.dump().c_str());
            message = compute_post_request("34.241.4.235","/api/v1/tema/library/books","application/json",
                                                    send_parametrii_post,1,NULL,0,jwt_token);


            send_to_server(sockfd,message);
            response = receive_from_server(sockfd);

            cout << response << "\n";
            // cout<<"\n";
            // cout << message;

            
            err = strstr(response, "error");
            if (err) {
                cout << "Datele introduse sunt gresite, mai incercati" << "\n";
            }
            else {
                cout << "Cartea a fost adaugata cu succes :))))" << "\n";
            }


            close(sockfd);

        }
        else if(strcmp(comanda_buffer,"delete_book") == 0)
        {
              if ( jwt_check != 1) {
                cout << "Nu suntem la, libraria buna" << "\n";
                continue;
            }
            sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);

            char id[100];
            cout << "id="; 
            cin >> id;
            char url[500];
            
            strcpy(url, "/api/v1/tema/library/books/");strcat(url, id);
            char **send_parametrii_get = (char**)(malloc(sizeof(char*)));
            send_parametrii_get[0] = (char*)(malloc(sizeof(char)*310));
        
            message = compute_delete("34.241.4.235",url,NULL,send_parametrii_get,1,jwt_token);
            send_to_server(sockfd,message);
            response = receive_from_server(sockfd);

            
            err = strstr(response, "error");
            if (err) {
                cout << "Datele introduse sunt gresite, mai incercati" << "\n";
            }
            else {
                cout << "Cartea a fost stearsa cu succes :)))))" << "\n";
            }



            close(sockfd);
        }
        else if(strcmp(comanda_buffer,"logout") == 0)
        {
            if ( jwt_check != 1) {
                cout << "Nu sunteti conectat, mai incercati" << "\n";
                continue;
            }

            sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);

            char **send_parametrii_get = (char**)(malloc(sizeof(char*)));
            send_parametrii_get[0] = (char*)(malloc(sizeof(char)*200));
            strcpy(send_parametrii_get[0], cookie.c_str());
            cookie = res;
            message = compute_get_request("34.241.4.235","/api/v1/tema/auth/logout",NULL,send_parametrii_get,1,NULL);
            send_to_server(sockfd,message);
            response = receive_from_server(sockfd);
        
            
            err = strstr(response, "error");
            if (err) {
                cout << "Error.....idk:(((((" << "\n";
            }
            else {
                cout << "Deconectare realizata cu success"<<"\n";
            }
            
            check_log = false;
            jwt_check = 0;
            
            close(sockfd);
        }
        else if(strcmp(comanda_buffer,"exit") == 0){
            
            cout << "Va mai asteptam pe la noi!! :)))" << "\n";
            break;
        }
        else{
            cout << "Comanda introdusa gresit, mai incercati!!!" << "\n";
        }
            

    }

   

    close(sockfd);



    return 0;
}