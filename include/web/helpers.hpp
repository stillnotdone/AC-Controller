#ifndef WEB_HELPERS_H
#define WEB_HELPERS_H

#include "ac/controller.hpp"

void web_do_command(ACCommand command);
bool web_should_respond();
void web_update_page();
void web_not_found();
void web_error_response(int code, const char *message);
void web_good_response();
const char *web_bool2string(bool b);

#endif