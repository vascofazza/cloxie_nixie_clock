#ifndef SERIAL_PARSER_H
#define SERIAL_PARSER_H

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

void cmd_start_timer(SerialCommands* sender)
{
  sender->GetSerial()->println(F("Starting timer for 1 minute"));
  clock_driver->start_timer(60*1*1000);
}

void cmd_stop_timer(SerialCommands* sender)
{
  sender->GetSerial()->println(F("Stopping timer."));
  clock_driver->stop_timer();
}

void cmd_reset_timer(SerialCommands* sender)
{
  sender->GetSerial()->println(F("Resetting timer."));
  clock_driver->reset_timer();
}

void cmd_resume_timer(SerialCommands* sender)
{
  sender->GetSerial()->println(F("Resuming timer."));
  clock_driver->start_timer(-1);
}

//Note: Commands are case sensitive
SerialCommand cmd_reset_wifi_("WIFI_RESET", cmd_reset_wifi);
SerialCommand cmd_start_timer_("TIMER_START", cmd_start_timer);
SerialCommand cmd_stop_timer_("TIMER_STOP", cmd_stop_timer);
SerialCommand cmd_reset_timer_("TIMER_RESET", cmd_reset_timer);
SerialCommand cmd_resume_timer_("TIMER_RESUME", cmd_resume_timer);

void setup_serial_parser()
{
  serial_commands_.SetDefaultHandler(cmd_unrecognized);
  serial_commands_.AddCommand(&cmd_reset_wifi_);
  serial_commands_.AddCommand(&cmd_start_timer_);
  serial_commands_.AddCommand(&cmd_stop_timer_);
  serial_commands_.AddCommand(&cmd_reset_timer_);
  serial_commands_.AddCommand(&cmd_resume_timer_);
}

void serial_parser_loop()
{
  serial_commands_.ReadSerial();
}

#endif