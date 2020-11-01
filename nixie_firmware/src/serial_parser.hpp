#ifndef SERIAL_PARSER_HPP
#define SERIAL_PARSER_HPP

#include <SerialCommands.h> //https://github.com/ppedro74/Arduino-SerialCommands
#include "wifi.hpp"

static char serial_command_buffer_[32];
static SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");

//This is the default handler, and gets called when no other command matches.
void cmd_unrecognized(SerialCommands* sender, const char* cmd)
{
  sender->GetSerial()->print(F("Unrecognized command ["));
  sender->GetSerial()->print(cmd);
  sender->GetSerial()->println(F("]"));
}

void cmd_reset_wifi(SerialCommands* sender)
{
  sender->GetSerial()->println(F("Resetting Wifi settings."));
  reset_wifi_settings();
}

//Note: Commands are case sensitive
SerialCommand cmd_reset_wifi_("RESET", cmd_reset_wifi);

void setup_serial_parser()
{
  serial_commands_.SetDefaultHandler(cmd_unrecognized);
  serial_commands_.AddCommand(&cmd_reset_wifi_);
}

void serial_parser_loop()
{
  serial_commands_.ReadSerial();
}

#endif