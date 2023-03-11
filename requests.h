#ifndef _REQUESTS_
#define _REQUESTS_

// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(char *host, char *url, char *query_params,// am schimbat putin functia aceasta din lab
                          char **cookies, int cookies_count, const char *token);// ca sa mi ia si token ul

// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count, const char *token);//same ca fc de mai sus, ma ajuta asa mai mult 
							//pentru Authorization barrer

char *compute_delete(char *host, char *url, char *query_params,// am creat fc pt delete care practic e same ca si compute_get_request
                          char **cookies, int cookies_count, const char *token);// si am modificat doar GET cu DELETE

#endif
